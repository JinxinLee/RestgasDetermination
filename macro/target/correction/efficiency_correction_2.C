#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include "TEfficiency.h"
#include "TStyle.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TProfile.h"
#include "TF1.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TObjArray.h"
#include "TObjString.h"

// Helper function to add files to chain, supporting range format "path_%d.root,start,end"
void AddToChain(TChain* chain, TString input) {
    if (input.Contains(",")) {
        TObjArray* tokens = input.Tokenize(",");
        if (tokens->GetEntries() >= 3) {
            TString format = ((TObjString*)tokens->At(0))->GetString();
            int start = ((TObjString*)tokens->At(1))->GetString().Atoi();
            int end = ((TObjString*)tokens->At(2))->GetString().Atoi();
            std::cout << "[Info] Adding files from " << start << " to " << end << " using format: " << format << std::endl;
            for (int i = start; i <= end; ++i) {
                chain->Add(Form(format.Data(), i));
            }
        } else {
             std::cout << "[Warning] Invalid file range format. Expected 'path_%d.root,start,end'. Using as single file." << std::endl;
             chain->Add(input);
        }
        delete tokens;
    } else {
        chain->Add(input);
    }
}

/**
 * 效率修正脚本
 * 
 * 功能：
 * 1. 读取真实数据 (Data Rec)
 * 2. 读取MC重建数据 (MC Rec)
 * 3. 读取MC生成数据 (MC Gen) - 用于计算分母
 * 4. 计算效率 Efficiency = MC Rec / MC Gen
 * 5. 对真实数据进行修正 Corrected Data = Data Rec / Efficiency
 * 
 * 使用方法：
 * root -l 'efficiency_correction_2.C("data_ana.root", "mc_ana.root", "mc_sim.root")'
 * root -b -q -l 'efficiency_correction_2.C("../data/test_poca/reco/test_poca_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_sim.root", "../data/test_poca/reco/test_poca_*_sim.root","10percent_fullgas")'
 * 
 * 指定文件编号范围 (New):
 * root -l 'efficiency_correction_2.C("data_%d.root,1,100", "mc_%d.root,1,500", "mc_sim_%d.root,1,500")'
 */
void efficiency_correction_2(
    TString realDataFile = "data_ana_final.root", // 真实数据文件 (经过ana)
    TString mcRecFile = "mc_ana_final.root",      // MC重建数据文件 (经过ana)
    TString mcGenFile = "mc_sim.root",            // MC生成数据文件 (sim输出，包含MCTrack)
    TString realDataGenFile = "",                 // 真实数据对应的生成文件 (可选，用于验证)
    TString outputName = "efficiency_correction_result", // 输出文件名前缀
    TString plotVar = "pvz",                      // 需要修正的变量名 (在ana文件的ntpDp树中)
    TString mcRecVar = "pvz_mc",                  // MC重建变量 (用于效率分子，通常使用MC真值)
    TString genVar = "MCTrack.fStartZ",           // 对应的生成级变量 (在sim文件的pndsim树中)
    TString recCut = "pvz != -999.0",             // 重建数据的筛选条件
    TString genCut = "MCTrack.fMotherID==-1&&MCTrack.fPdgCode==2212", // 生成级粒子的筛选条件
    int nBins = 300,
    double xMin = -300.0,
    double xMax = 600.0,
    double devMin = -1.0,
    double devMax = 1.0
)
{
    // ---------------------------------------------------------
    // Custom Binning Logic (Center 100 bins, Outer 200 bins)
    // ---------------------------------------------------------
    std::vector<double> edges;
    double centerMin = -30.0;
    double centerMax = 30.0;
    int nCenter = 100;
    int nOuterTotal = 200;

    // Check bounds
    double rMin = xMin;
    double rMax = xMax;
    
    // Calculate widths
    double widthLeft = (centerMin > rMin) ? (centerMin - rMin) : 0;
    double widthRight = (rMax > centerMax) ? (rMax - centerMax) : 0;
    double widthOuter = widthLeft + widthRight;
    
    int nLeft = 0;
    int nRight = 0;
    
    if (widthOuter > 0) {
        nLeft = (int)(nOuterTotal * widthLeft / widthOuter);
        nRight = nOuterTotal - nLeft;
    } else {
        // Fallback or entirely inside?
        // If xMin/xMax are within -50,50, just use uniform?
        // But assuming standard usage.
    }
    
    // 1. Generate Left bins
    if (nLeft > 0) {
        double stepLeft = widthLeft / nLeft;
        for (int i = 0; i < nLeft; ++i) {
            edges.push_back(rMin + i * stepLeft);
        }
    } 
    
    // 2. Generate Center bins
    if (centerMax > centerMin) {
        double stepCenter = (centerMax - centerMin) / nCenter;
        for (int i = 0; i < nCenter; ++i) {
            edges.push_back(centerMin + i * stepCenter);
        }
    }

    // 3. Generate Right bins
    if (nRight > 0) {
        double stepRight = widthRight / nRight;
        for (int i = 0; i <= nRight; ++i) { // include last edge
             edges.push_back(centerMax + i * stepRight);
        }
    } else {
        edges.push_back(centerMax); // Close the interval if no right part
    }

    // Convert to array
    int nBinsVar = edges.size() - 1;
    double* xBinsVar = edges.data();
    
    std::cout << "[Info] Using Variable Binning:" << std::endl;
    std::cout << "  Range: " << rMin << " -> " << rMax << std::endl;
    std::cout << "  Inner: " << centerMin << " -> " << centerMax << " (" << nCenter << " bins)" << std::endl;
    std::cout << "  Outer Left: " << nLeft << " bins" << std::endl;
    std::cout << "  Outer Right: " << nRight << " bins" << std::endl;
    std::cout << "  Total Bins: " << nBinsVar << std::endl;

    // ---------------------------------------------------------
    // 基础设置
    // ---------------------------------------------------------
    // 强制使用纯白色背景
    //gStyle->SetCanvasColor(kWhite);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    // 设置全局图例无边框
    gStyle->SetLegendBorderSize(0);
    // (可选) 设置全局图例背景透明 (实测某些ROOT版本对Legend的全局FillStyle支持不稳定，建议用方法1手动设透明)
    gStyle->SetLegendFillColor(0);

    // ---------------------------------------------------------
    // 关键：线宽与点大小 (针对 5000px 宽度优化)
    // ---------------------------------------------------------
    // 默认线宽通常是 1px，在 5000px 图上几乎不可见。建议设为 3 到 5。
    gStyle->SetLineWidth(6);      
    gStyle->SetFrameLineWidth(6); // 坐标轴边框
    gStyle->SetHistLineWidth(6);  // 直方图线条
    gStyle->SetFuncWidth(6);      // 函数/拟合曲线
    gStyle->SetGridWidth(2);      // 网格线（如果开启）

    // 标记点 (Marker) 大小，默认是 1.0，建议放大到 2.0 - 3.0
    gStyle->SetMarkerSize(2.5);
    gStyle->SetMarkerStyle(20);   // 推荐使用实心圆点，在大图上最清晰

    // ---------------------------------------------------------
    // 字体设置 (ROOT字体大小是占 Pad 高度的百分比)
    // ---------------------------------------------------------
    // 5000x2500 是 2:1 的宽图。
    // 0.05 的意思是占高度的 5%，即 2500 * 0.05 = 125 像素高（非常清晰）。
    
    // 坐标轴刻度数值 (Label)
    gStyle->SetLabelSize(0.05, "XY"); 
    gStyle->SetLabelFont(42, "XY");   // 42号字体 (Helvetica) 比默认的 62号更标准

    // 坐标轴标题 (Title)
    gStyle->SetTitleSize(0.06, "XY"); 
    gStyle->SetTitleFont(42, "XY");

    // 顶部图表标题
    gStyle->SetTitleSize(0.06, "t");  
    gStyle->SetTitleFont(42, "t");

    // ---------------------------------------------------------
    // 布局微调
    // ---------------------------------------------------------
    // 调整标题与轴的距离。由于画布很宽，Y轴标题可能会离轴太远，需适当减小 Offset
    gStyle->SetTitleOffset(0.95, "X");
    gStyle->SetTitleOffset(1.2, "Y"); // 宽画幅下，Y轴标题贴近一点更好看

    // 刻度线长度 (增加一点长度，更有质感)
    gStyle->SetTickLength(0.02, "XY");
    
    // 统计框 (StatBox) - 如果需要显示，必须调整字体和位置，否则会很丑
    gStyle->SetStatFont(42);
    gStyle->SetStatFontSize(0.04);
    gStyle->SetStatBorderSize(2); // 边框加粗
    // 设置绘图风格
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
    gStyle->SetPadBottomMargin(0.15); // 底部留白给 X 轴标题 (默认约 0.1)
    gStyle->SetPadLeftMargin(0.15);   // 左侧留白给 Y 轴标题 (默认约 0.1)
    gStyle->SetPadRightMargin(0.05);  // 右侧稍微紧凑点
    gStyle->SetPadTopMargin(0.08);
    // gStyle->SetTextSize(0.05);
    // gStyle->SetLabelSize(0.05, "XYZ");
    // gStyle->SetTitleSize(0.06, "XYZ");
    // gStyle->SetTitleOffset(1.4, "Y");
    // gStyle->SetPadLeftMargin(0.18);
    // gStyle->SetPadBottomMargin(0.15);

    // ---------------------------------------------------------
    // 1. 获取MC重建分布 (Numerator for Efficiency) & 计算偏差
    // ---------------------------------------------------------
    TChain* tMCRec = new TChain("ntpDp");
    AddToChain(tMCRec, mcRecFile);
    if (tMCRec->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in MC Rec file(s): " << mcRecFile << std::endl; 
        return; 
    }

    // High granularity histograms for Efficiency plot (using Variable Bins)
    
    // MC Rec (Truth Variable) - 用于标准效率计算
    TH1F* hMCRec = new TH1F("hMCRec", "MC Rec (Truth Var);Z (cm);#Events", nBinsVar, xBinsVar);
    tMCRec->Draw(mcRecVar + ">>hMCRec", recCut);
    hMCRec->SetDirectory(0);

    // MC Rec (Reco Variable) - 用于Reco-based效率计算
    TH1F* hMCRecReco = new TH1F("hMCRecReco", "MC Rec (Reco Var);Z (cm);#Events", nBinsVar, xBinsVar);
    tMCRec->Draw(plotVar + ">>hMCRecReco", recCut);
    hMCRecReco->SetDirectory(0);

    // 计算偏差 (Reco - Truth)
    // 1. 全局偏差分布 (1D)
    TH1F* hDev = new TH1F("hDev", "Global Resolution;Rec - Truth (cm);#Events", 100, devMin, devMax);
    tMCRec->Draw(Form("%s - %s >> hDev", plotVar.Data(), mcRecVar.Data()), recCut);
    hDev->SetDirectory(0);
    double globalBias = hDev->GetMean();

    // 2. 偏差随Z的变化 (Profile) - 用于动态修正
    // Use nBinsVar for profile to match efficiency granularity
    TProfile* pBias = new TProfile("pBias", "Bias vs Z (Profile);Z_{reco} (cm);<Bias> (cm)", nBinsVar, xBinsVar);
    // Draw y:x >> profile
    tMCRec->Draw(Form("%s - %s : %s >> pBias", plotVar.Data(), mcRecVar.Data(), plotVar.Data()), recCut, "prof");
    pBias->SetDirectory(0);

    // 3. 拟合偏差曲线 (使用线性函数 pol1)
    /*
    TF1* fBias = new TF1("fBias", "pol1", xMin, xMax);
    pBias->Fit(fBias, "Q"); // Q for quiet
    double p0 = fBias->GetParameter(0);
    double p1 = fBias->GetParameter(1);
    */

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Global Bias (Mean): " << globalBias << " cm" << std::endl;
    // std::cout << "Position-dependent Bias Fit (pol1): " << p0 << " + " << p1 << " * Z" << std::endl;
    // std::cout << "Applying dynamic bias correction..." << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    // ---------------------------------------------------------
    // 2. 获取真实数据分布 (Numerator for Data)
    // ---------------------------------------------------------
    TChain* tData = new TChain("ntpDp");
    AddToChain(tData, realDataFile);
    if (tData->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in Data file(s): " << realDataFile << std::endl; 
        return; 
    }

    // Data (Raw) - 保持原始数据，不直接修正Bias
    TH1F* hDataRaw = new TH1F("hDataRaw", "Real Data (Raw);Z (cm);#Events", nBinsVar, xBinsVar);
    tData->Draw(plotVar + ">>hDataRaw", recCut);
    hDataRaw->SetDirectory(0);

    // ---------------------------------------------------------
    // 3. 获取MC生成分布 (Denominator for Efficiency)
    // --------------------------------------------------------- 

    // ---------------------------------------------------------
    // 3. 获取MC生成分布 (Denominator for Efficiency)
    // ---------------------------------------------------------
    // 注意：这是计算绝对效率的关键。必须使用包含所有生成粒子的sim文件。
    TChain* tMCGen = new TChain("pndsim");
    AddToChain(tMCGen, mcGenFile);
    if (tMCGen->GetEntries() == 0) {
        delete tMCGen;
        tMCGen = new TChain("cbmsim");
        AddToChain(tMCGen, mcGenFile);
    }
    
    if (tMCGen->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'cbmsim' or 'pndsim' in MC Gen file(s): " << mcGenFile << std::endl; 
        return; 
    }

    // Use nBinsVar for Efficiency calculation
    TH1F* hMCGen = new TH1F("hMCGen", "MC Generated;Z (cm);#Events", nBinsVar, xBinsVar);
    
    std::cout << "Drawing Gen variable: " << genVar << " with cut: " << genCut << std::endl;
    // 注意：这里假设genVar可以直接访问。如果是TClonesArray，可能需要更复杂的Draw语法
    // 例如 "MCTrack.fP" 或 "MCTrack[0].fP"
    tMCGen->Draw(genVar + ">>hMCGen", genCut); 
    hMCGen->SetDirectory(0);

    // ---------------------------------------------------------
    // 3.5 获取真实数据的生成分布 (Optional Truth for Verification)
    // ---------------------------------------------------------
    TH1F* hDataGen = nullptr;
    if (realDataGenFile != "") {
        TChain* tDataGen = new TChain("pndsim");
        AddToChain(tDataGen, realDataGenFile);
        if (tDataGen->GetEntries() == 0) {
            delete tDataGen;
            tDataGen = new TChain("cbmsim");
            AddToChain(tDataGen, realDataGenFile);
        }

        if (tDataGen->GetEntries() > 0) {
            // Keep DataGen consistent
            hDataGen = new TH1F("hDataGen", "Real Data Generated (Truth);Z (cm);#Events", nBinsVar, xBinsVar);
            std::cout << "Drawing Real Data Gen variable..." << std::endl;
            tDataGen->Draw(genVar + ">>hDataGen", genCut);
            hDataGen->SetDirectory(0);
        } else {
            std::cout << "Warning: Cannot find tree 'cbmsim' or 'pndsim' in Real Data Gen file(s)." << std::endl;
        }
    }

    // ---------------------------------------------------------
    // 4. 计算效率 (Efficiency)
    // ---------------------------------------------------------
    // 4.1 标准效率 (Truth Variable based)
    TH1F* hEff = (TH1F*)hMCRec->Clone("hEff");
    hEff->SetTitle("Efficiency (Truth Var);Z (cm);Efficiency");
    hEff->Divide(hMCRec, hMCGen, 1, 1, "B"); 

    // 4.2 Reco效率 (Reco Variable based)
    TH1F* hEffReco = (TH1F*)hMCRecReco->Clone("hEffReco");
    hEffReco->SetTitle("Efficiency (Reco Var);Z (cm);Efficiency");
    hEffReco->Divide(hMCRecReco, hMCGen, 1, 1, "B");

    // 4.3 Bias-Corrected Efficiency (Shifted Lookup)
    // 构造一个考虑了Bias的效率曲线：Eff_corr(z_rec) = Eff_std(z_rec - Bias(z_rec))
    // 这样在修正 Data(z_rec) 时，使用的是其对应真值位置 z_true 的效率
    /*
    TH1F* hEffBiasCorr = (TH1F*)hEff->Clone("hEffBiasCorr");
    hEffBiasCorr->SetTitle("Efficiency (Bias Corrected);Z_{reco} (cm);Efficiency");
    hEffBiasCorr->Reset();
    
    for (int i = 1; i <= hEffBiasCorr->GetNbinsX(); ++i) {
        double z_rec = hEffBiasCorr->GetBinCenter(i);
        // 计算对应的真值位置: z_true = z_rec - Bias(z_rec)
        double bias_val = p0 + p1 * z_rec;
        double z_true = z_rec - bias_val;
        
        // 查找 z_true 在标准效率曲线(hEff)中的值
        int bin_true = hEff->FindBin(z_true);
        double eff_val = 0.0;
        double eff_err = 0.0;
        
        // 确保在范围内
        if (bin_true >= 1 && bin_true <= hEff->GetNbinsX()) {
            eff_val = hEff->GetBinContent(bin_true);
            eff_err = hEff->GetBinError(bin_true);
        }
        
        hEffBiasCorr->SetBinContent(i, eff_val);
        hEffBiasCorr->SetBinError(i, eff_err);
    }
    */

    // ---------------------------------------------------------
    // 5. 修正真实数据 (Correction)
    // ---------------------------------------------------------
    // 5.1 标准修正 (新): Raw Data / Bias-Corrected Eff
    // 这保留了 Data 的 Reco 坐标，但使用了正确的(偏移后的)效率值进行归一化
    /*
    TH1F* hCorrected = (TH1F*)hDataRaw->Clone("hCorrected");
    hCorrected->SetTitle("Corrected (Raw / BiasCorrEff);Variable;#Events");
    hCorrected->Divide(hEffBiasCorr);
    */

    // 5.2 无Bias修正: Raw Data / Standard Truth Eff (忽略Bias)
    TH1F* hCorrectedNoBias = (TH1F*)hDataRaw->Clone("hCorrectedNoBias");
    hCorrectedNoBias->SetTitle("Corrected (Raw / TruthEff);Z (cm);#Events");
    hCorrectedNoBias->Divide(hEff);

    // 5.3 Reco效率修正: Raw Data / Reco Eff (自动包含Bias和Smearing)
    TH1F* hCorrectedRecoEff = (TH1F*)hDataRaw->Clone("hCorrectedRecoEff");
    hCorrectedRecoEff->SetTitle("Corrected (Raw / RecoEff);Z (cm);#Events");
    hCorrectedRecoEff->Divide(hEffReco);

    // 5.4 Hybrid Correction: (-2, 2) uses Truth Eff (pvz_mc), others use Reco Eff (pvz)
    TH1F* hCorrectedHybrid = (TH1F*)hDataRaw->Clone("hCorrectedHybrid");
    hCorrectedHybrid->SetTitle("Corrected (Hybrid);Z (cm);#Events");
    hCorrectedHybrid->Reset(); // Clear content to fill manually

    for (int i = 1; i <= hDataRaw->GetNbinsX(); ++i) {
        double content = hDataRaw->GetBinContent(i);
        double error = hDataRaw->GetBinError(i);
        double center = hDataRaw->GetBinCenter(i);
        double eff = 0;
        double eff_err = 0;

        if (center >= -2.0 && center <= 2.0) {
             // Use Truth Eff (hEff)
             int binEff = hEff->FindBin(center);
             eff = hEff->GetBinContent(binEff);
             eff_err = hEff->GetBinError(binEff);
        }
        else {
             // Use Reco Eff (hEffReco)
             int binEff = hEffReco->FindBin(center);
             eff = hEffReco->GetBinContent(binEff);
             eff_err = hEffReco->GetBinError(binEff);
        }

        if (eff > 0) {
            double new_content = content / eff;
            // Error propagation: (C/E) * sqrt((dC/C)^2 + (dE/E)^2)
            double rel_err_content = (content > 0) ? error / content : 0;
            double rel_err_eff = eff_err / eff;
            double new_error = new_content * sqrt(rel_err_content*rel_err_content + rel_err_eff*rel_err_eff);
            
            hCorrectedHybrid->SetBinContent(i, new_content);
            hCorrectedHybrid->SetBinError(i, new_error);
        }
    }

    // ---------------------------------------------------------
    // 6. 绘图与保存
    // ---------------------------------------------------------
    // 修改：只保留原Pad 6(Hybrid Correction)和Pad 8(Rel Diff)
    // 采用左右布局 (Left: Correction Result, Right: Relative Difference)
    TCanvas* c1 = new TCanvas("c1", "Efficiency Correction Analysis", 6000, 3000);
    // c1->Divide(2, 1); 

    // ------------------------------------------
    // New Pad 1 (Left): Hybrid Correction vs Truth
    // ------------------------------------------
    // ---------------------------------------------------------
    // ---------------------------------------------------------
    // 6. 绘图 (c1) - Physical Axis (Visual Width = Physical Width)
    // ---------------------------------------------------------
    c1->cd(1);
    gPad->SetLeftMargin(0.08);
    gPad->SetRightMargin(0.04);
    gPad->SetBottomMargin(0.12); 
    gPad->SetTopMargin(0.08); 
    gPad->SetTicks(1, 1);
    gPad->SetLogy();

    // 统一纵坐标范围
    double yMax = 0;
    if (hDataGen) {
        yMax = hDataGen->GetMaximum() * 5.0; 
    } else {
        yMax = hDataRaw->GetMaximum() * 5.0; 
        if (hCorrectedHybrid && hCorrectedHybrid->GetMaximum() > yMax) yMax = hCorrectedHybrid->GetMaximum() * 5.0;
    }

    TH1F* hCorrectedHybridDisp = nullptr;
    TH1F* hDataGenDisp = nullptr;

    double yMinPlot = 0.5; // Fixed Min for Log Plot

    // Calculate Reference Width (Center region Physical)
    double widthRef = 1.0;
    if (nCenter > 0 && centerMax > centerMin) {
        widthRef = (centerMax - centerMin) / nCenter;
    }

    if (hCorrectedHybrid) {
        // Clone original histogram (which has the correct Physical Axis)
        hCorrectedHybridDisp = (TH1F*)hCorrectedHybrid->Clone("hCorrectedHybridDisp");
        hCorrectedHybridDisp->Reset(); 

        for(int i=1; i<=nBinsVar; ++i) {
            double physWidth = hCorrectedHybrid->GetBinWidth(i);
            // Scale content to represent "Events per [widthRef]" density
            double scale = (physWidth > 0) ? (widthRef / physWidth) : 1.0; 
            
            hCorrectedHybridDisp->SetBinContent(i, hCorrectedHybrid->GetBinContent(i) * scale);
            hCorrectedHybridDisp->SetBinError(i, hCorrectedHybrid->GetBinError(i) * scale);
        }

        hCorrectedHybridDisp->SetTitle(""); 
        hCorrectedHybridDisp->SetLineColor(kOrange+1);
        hCorrectedHybridDisp->SetLineWidth(6);     
        hCorrectedHybridDisp->SetMarkerColor(kOrange+1);
        hCorrectedHybridDisp->SetMarkerStyle(21);
        hCorrectedHybridDisp->SetMarkerSize(3);  
        hCorrectedHybridDisp->SetMinimum(yMinPlot); 
        hCorrectedHybridDisp->SetMaximum(yMax); 
        
        // Axis Titles
        hCorrectedHybridDisp->GetXaxis()->SetLabelSize(0.035);
        hCorrectedHybridDisp->GetXaxis()->SetTitle("#font[132]{#it{z} (cm)}"); 
        hCorrectedHybridDisp->GetXaxis()->SetTitleSize(0.05);
        hCorrectedHybridDisp->GetXaxis()->SetTitleOffset(1.0);
        hCorrectedHybridDisp->GetXaxis()->CenterTitle(); 
        
        hCorrectedHybridDisp->GetYaxis()->SetLabelSize(0.035);
        hCorrectedHybridDisp->GetYaxis()->SetTitle("#font[132]{#Events}"); 
        hCorrectedHybridDisp->GetYaxis()->SetTitleSize(0.05);
        hCorrectedHybridDisp->GetYaxis()->SetTitleOffset(0.7);
        hCorrectedHybridDisp->GetYaxis()->CenterTitle(); 
        
        hCorrectedHybridDisp->Draw("E"); 
        
        // Physical visual lines
        TLine* l1 = new TLine(-30, yMinPlot, -30, yMax);
        l1->SetLineStyle(2); l1->SetLineColor(kRed); l1->SetLineWidth(3); l1->Draw();
        TLine* l2 = new TLine(30, yMinPlot, 30, yMax);
        l2->SetLineStyle(2); l2->SetLineColor(kRed); l2->SetLineWidth(3); l2->Draw();
    }

    TLegend* leg6 = new TLegend(0.41, 0.88, 0.6, 0.75); // Moved to Left-Middle to avoid overlap with Inset
    leg6->SetFillStyle(0);
    leg6->SetBorderSize(0);
    leg6->SetTextFont(132);
    leg6->SetTextSize(0.045); 
    if (hCorrectedHybridDisp) leg6->AddEntry(hCorrectedHybridDisp, "Corrected", "lp");

    if (hDataGen) {
        hDataGenDisp = (TH1F*)hDataGen->Clone("hDataGenDisp");
        hDataGenDisp->Reset();

        for(int i=1; i<=nBinsVar; ++i) {
            double physWidth = hDataGen->GetBinWidth(i);
            double scale = (physWidth > 0) ? (widthRef / physWidth) : 1.0; 
            
            hDataGenDisp->SetBinContent(i, hDataGen->GetBinContent(i) * scale);
            // Error scaling... usually not plotted?
        }
        
        hDataGenDisp->SetLineColor(kAzure+2);
        hDataGenDisp->SetLineWidth(5); 
        hDataGenDisp->Draw("HIST SAME");
        leg6->AddEntry(hDataGenDisp, "Truth", "l");
        
        // --- Statistics Calculation (Keep original logic, just display) ---
        // 1. Global (All Range)
        double errHybrid, errGen;
        double nHybrid = hCorrectedHybrid->IntegralAndError(1, hCorrectedHybrid->GetNbinsX(), errHybrid);
        double nGen = hDataGen->IntegralAndError(1, hDataGen->GetNbinsX(), errGen);
        
        double relDevGlobal = 0, errRelDevGlobal = 0;
        if (nGen > 0) {
            double ratioGlobal = nHybrid / nGen;
            relDevGlobal = (ratioGlobal - 1.0) * 100.0;
            errRelDevGlobal = 100.0 * ratioGlobal * sqrt(pow(errHybrid/nHybrid, 2) + pow(errGen/nGen, 2));
        }

        // 2. Center Region ([-30, 30])
        int binStartCenter = nLeft + 1;
        int binEndCenter = nLeft + nCenter;
        
        double errHybridC, errGenC;
        double nHybridC = hCorrectedHybrid->IntegralAndError(binStartCenter, binEndCenter, errHybridC);
        double nGenC = hDataGen->IntegralAndError(binStartCenter, binEndCenter, errGenC);
        
        double relDevCenter = 0, errRelDevCenter = 0;
        double ratioCenterReco = 0, ratioCenterGen = 0;

        if (nGenC > 0) {
            double ratioCenter = nHybridC / nGenC;
            relDevCenter = (ratioCenter - 1.0) * 100.0;
            errRelDevCenter = 100.0 * ratioCenter * sqrt(pow(errHybridC/nHybridC, 2) + pow(errGenC/nGenC, 2));
        }

        if (nHybrid > 0) ratioCenterReco = nHybridC / nHybrid;
        if (nGen > 0) ratioCenterGen = nGenC / nGen;

        // --- Stats Printing ---
        TLatex* latex = new TLatex();
        latex->SetNDC();
        latex->SetTextSize(0.045); 
        latex->SetTextColor(kBlack);
        latex->SetTextAlign(12);
        
        double xPos = 0.11;
        double yPos = 0.86;
        double step = 0.06;

        latex->DrawLatex(xPos, yPos, Form("#font[132]{Global #Delta#it{N}/#it{N} = %.2f #pm %.2f %%}", relDevGlobal, errRelDevGlobal));
        yPos -= step;
        latex->DrawLatex(xPos, yPos, Form("#font[132]{Center #Delta#it{N}/#it{N} = %.2f #pm %.2f %%}", relDevCenter, errRelDevCenter));
        // yPos -= step;
        // latex->DrawLatex(xPos, yPos, Form("#font[132]{Center Ratio (Reco) = %.4f}", ratioCenterReco));
        // yPos -= step;
        // latex->DrawLatex(xPos, yPos, Form("#font[132]{Center Ratio (Truth) = %.4f}", ratioCenterGen));
    }

    // No custom axis labels needed for physical plot
    leg6->Draw();

    // ------------------------------------------
    // Inset Pad (Top Right) - Zoom [-30, 30]
    // ------------------------------------------
    TPad *padInset = new TPad("padInset", "inset", 0.5, 0.48, 0.95, 0.88);
    padInset->SetFillStyle(4000); 
    padInset->SetFillColor(kWhite);
    padInset->SetLeftMargin(0.12);
    padInset->SetRightMargin(0.05);
    padInset->SetBottomMargin(0.15);
    padInset->SetTopMargin(0.05);
    padInset->SetTicks(1, 1);
    padInset->SetLogy(); // Keep LogY
    padInset->Draw();
    padInset->cd();

    TH1F* hCorrectedHybridDispInset = nullptr;
    if(hCorrectedHybridDisp) {
        hCorrectedHybridDispInset = (TH1F*)hCorrectedHybridDisp->Clone("hCorrectedHybridDispInset");
        hCorrectedHybridDispInset->GetXaxis()->SetRangeUser(-30, 30);
        hCorrectedHybridDispInset->SetMinimum(100.0);
        
        // Resize fonts for smaller pad (make them visible)
        hCorrectedHybridDispInset->GetXaxis()->SetLabelSize(0.06); 
        hCorrectedHybridDispInset->GetXaxis()->SetTitleSize(0.07);
        hCorrectedHybridDispInset->GetXaxis()->SetTitleOffset(0.9);
        hCorrectedHybridDispInset->GetYaxis()->SetLabelSize(0.06);
        hCorrectedHybridDispInset->GetYaxis()->SetTitleSize(0.07);
        hCorrectedHybridDispInset->GetYaxis()->SetTitleOffset(0.7);
        
        hCorrectedHybridDispInset->Draw("E");
    }

    if (hDataGenDisp) {
        TH1F* hDataGenDispInset = (TH1F*)hDataGenDisp->Clone("hDataGenDispInset");
        hDataGenDispInset->GetXaxis()->SetRangeUser(-30, 30);
        hDataGenDispInset->Draw("HIST SAME");
    }
    
    c1->cd(1);

    /* 
    // ------------------------------------------
    // New Pad 2 (Right): Relative Difference
    // ------------------------------------------
    c1->cd(2);
    gPad->SetLeftMargin(0.18); 
    gPad->SetRightMargin(0.04);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.08); 
    gPad->SetTicks(1, 1);
    gPad->SetGridy();

    if (hDataGen && hCorrectedHybrid) {
        TH1F* hRelDiffHybrid = (TH1F*)hCorrectedHybrid->Clone("hRelDiffHybrid");
        hRelDiffHybrid->Add(hDataGen, -1.0); 
        hRelDiffHybrid->Divide(hDataGen); // Enable Divide for correct Ratio

        // Flatten
        TH1F* hRelDiffFlat = new TH1F("hRelDiffFlat", "", nBinsVar, 0, nBinsVar);
        for(int i=1; i<=nBinsVar; ++i) {
             hRelDiffFlat->SetBinContent(i, hRelDiffHybrid->GetBinContent(i));
             hRelDiffFlat->SetBinError(i, hRelDiffHybrid->GetBinError(i));
        }

        hRelDiffFlat->SetMinimum(-0.5); 
        hRelDiffFlat->SetMaximum(0.5);

        // Axis
        hRelDiffFlat->GetXaxis()->SetLabelSize(0); // Hide default labels
        hRelDiffFlat->GetXaxis()->SetTitle("#font[132]{#it{z}} (cm)"); 
        hRelDiffFlat->GetXaxis()->SetTitleSize(0.045);
        hRelDiffFlat->GetXaxis()->SetTitleOffset(1.1);
        hRelDiffFlat->GetXaxis()->CenterTitle(); 
        
        hRelDiffFlat->GetYaxis()->SetLabelSize(0.035);
        hRelDiffFlat->GetYaxis()->SetTitle("#font[132]{#frac{#Delta#it{N}}{#it{N}}}"); 
        hRelDiffFlat->GetYaxis()->SetTitleSize(0.045);
        hRelDiffFlat->GetYaxis()->SetTitleOffset(1.3);
        hRelDiffFlat->GetYaxis()->CenterTitle();
        hRelDiffFlat->GetYaxis()->SetNdivisions(505);

        TH1F* hRelDiffPos = (TH1F*)hRelDiffFlat->Clone("hRelDiffPos");
        TH1F* hRelDiffNeg = (TH1F*)hRelDiffFlat->Clone("hRelDiffNeg");
        // Reset errors for clean points
        for(int i=0; i<=nBinsVar+1; ++i) {
            hRelDiffPos->SetBinError(i,0);
            hRelDiffNeg->SetBinError(i,0);
        }
        
        hRelDiffPos->SetMarkerColor(kOrange+1);
        hRelDiffPos->SetMarkerStyle(20); hRelDiffPos->SetMarkerSize(2.3);
        hRelDiffNeg->SetMarkerColor(kAzure+2);
        hRelDiffNeg->SetMarkerStyle(20); hRelDiffNeg->SetMarkerSize(2.3);
        
        for (int i=1; i<=nBinsVar; ++i) {
            double c = hRelDiffFlat->GetBinContent(i);
            if (c >= 0) hRelDiffNeg->SetBinContent(i, -9999);
            else hRelDiffPos->SetBinContent(i, -9999);
        }
        
        hRelDiffFlat->Draw("AXIS"); // Draw axes from main histo
        hRelDiffPos->Draw("P SAME");
        hRelDiffNeg->Draw("P SAME");

        // Custom Labels (Right Pad)
        double yLabelR = -0.55; // Slightly below axis min (-0.5)
        
        TLatex* tAxisR = new TLatex();
        tAxisR->SetTextSize(0.04);
        tAxisR->SetTextAlign(23); 
        tAxisR->SetTextFont(42);

        tAxisR->DrawLatex(0, yLabelR, Form("%.0f", xMin));
        tAxisR->DrawLatex(nLeft, yLabelR, "-30");
        tAxisR->DrawLatex(nLeft+nCenter, yLabelR, "30");
        tAxisR->DrawLatex(nBinsVar, yLabelR, Form("%.0f", xMax));

        // Mark boundaries
        TLine* l3 = new TLine(nLeft, -0.5, nLeft, 0.5);
        l3->SetLineStyle(2); l3->SetLineColor(kGray+2); l3->Draw();
        TLine* l4 = new TLine(nLeft+nCenter, -0.5, nLeft+nCenter, 0.5);
        l4->SetLineStyle(2); l4->SetLineColor(kGray+2); l4->Draw();

        
        TLine *line = new TLine(xMin, 0.0, xMax, 0.0);
        line->SetLineStyle(2);
        line->SetLineColor(kRed);
        line->SetLineWidth(4); // 加粗
        line->Draw();
    }
    */

    c1->SaveAs(outputName + ".png");

    // ---------------------------------------------------------
    // 7. 新Canvas绘制效率对比 (Truth vs Reco) - Physical Axis
    // ---------------------------------------------------------
    TCanvas* c2 = new TCanvas("c2", "Efficiency Comparison", 6000, 3000);
    c2->SetLeftMargin(0.08);
    c2->SetRightMargin(0.04);
    c2->SetBottomMargin(0.12); 
    c2->SetTopMargin(0.08); 
    c2->SetTicks(1, 1);
    c2->SetGrid();
    
    // Draw efficiencies directly on physical axis
    
    // Styles
    hEff->SetLineColor(kGreen+2);
    hEff->SetMarkerColor(kGreen+2);
    hEff->SetMarkerStyle(20);
    hEff->SetMarkerSize(4);
    hEff->SetLineWidth(4);

    hEffReco->SetLineColor(kOrange+7);
    hEffReco->SetMarkerColor(kOrange+7);
    hEffReco->SetMarkerStyle(21);
    hEffReco->SetMarkerSize(4);
    hEffReco->SetLineWidth(4);

    // Axes
    hEff->SetTitle("");
    hEff->GetXaxis()->SetLabelSize(0.035);
    hEff->GetXaxis()->SetTitle("#font[132]{#it{z}} (cm)");
    hEff->GetXaxis()->SetTitleSize(0.05);
    hEff->GetXaxis()->SetTitleOffset(1.0);
    hEff->GetXaxis()->CenterTitle();
    hEff->GetXaxis()->SetRangeUser(-30, 30);

    hEff->GetYaxis()->SetLabelSize(0.035);
    hEff->GetYaxis()->SetTitle("#font[132]{Efficiency}");
    hEff->GetYaxis()->SetTitleSize(0.05);
    hEff->GetYaxis()->SetTitleOffset(0.7);
    hEff->GetYaxis()->CenterTitle();
    
    hEff->SetMinimum(0.0);
    double maxEff = hEff->GetMaximum();
    if (hEffReco->GetMaximum() > maxEff) maxEff = hEffReco->GetMaximum();
    hEff->SetMaximum(maxEff * 1.1);

    hEff->Draw("E");
    hEffReco->Draw("E SAME");

   /* // Physical visual lines
    TLine* l5 = new TLine(-30, 0, -30, 1.2);
    l5->SetLineStyle(2); l5->SetLineColor(kRed); l5->SetLineWidth(3); l5->Draw();
    TLine* l6 = new TLine(30, 0, 30, 1.2);
    l6->SetLineStyle(2); l6->SetLineColor(kRed); l6->SetLineWidth(3); l6->Draw();
   */ 
    TLegend* legEff = new TLegend(0.7, 0.75, 0.9, 0.9); // 放在右下角
    legEff->SetFillStyle(0);
    legEff->SetBorderSize(0);
    legEff->SetTextFont(132);
    legEff->SetTextSize(0.045); 
    legEff->AddEntry(hEff, "Uniform Input", "lp");
    legEff->AddEntry(hEffReco, "Realistic Input", "lp");
    legEff->Draw();

    c2->SaveAs(outputName + "_eff.png");

    // ---------------------------------------------------------
    // 8. 打印统计信息
    // ---------------------------------------------------------
    std::cout << "========================================" << std::endl;
    std::cout << "          Final Statistics              " << std::endl;
    std::cout << "========================================" << std::endl;

    double errData, errTruth, errCorr;
    // N_data (Raw)
    double nData = hDataRaw->IntegralAndError(1, hDataRaw->GetNbinsX(), errData);
    
    // N_truth (Gen)
    double nTruth = 0;
    if(hDataGen) {
        // Truth assumes no error for the ratio denominator in user request context ("except truth others with error")
        // But IntegralAndError calculates sqrt(N). We'll capture it but might not print it if requested.
        nTruth = hDataGen->IntegralAndError(1, hDataGen->GetNbinsX(), errTruth);
    }

    // N_corrected (Hybrid)
    double nCorr = hCorrectedHybrid->IntegralAndError(1, hCorrectedHybrid->GetNbinsX(), errCorr);

    // Print N_data / N_truth
    if (nTruth > 0) {
        double ratio = nData / nTruth;
        // Error propagation for Ratio = Data/Truth. 
        // User said "except truth others with error", implying Truth is constant? 
        // If Truth has no error: errRatio = errData / Truth.
        // If Truth is MC/Gen count, it usually has Poisson error.
        // Assuming Truth is fixed reference:
        double errRatio = errData / nTruth; 
        // If we want to include Truth error: 
        // double errRatio = ratio * sqrt( pow(errData/nData, 2) + pow(errTruth/nTruth, 2) );
        
        // Interpreting "except truth others with error" as: Print Data(w/ err)/Truth(no err)
        std::cout << "N_data / N_truth : " << ratio << " +/- " << errRatio << std::endl;
        std::cout << "(N_data: " << nData << " +/- " << errData << ", N_truth: " << nTruth << ")" << std::endl;
    } else {
        std::cout << "N_data / N_truth : Undefined (N_truth missing)" << std::endl;
        std::cout << "N_data: " << nData << " +/- " << errData << std::endl;
    }

    // Print N_corrected
    std::cout << "N_corrected      : " << nCorr << " +/- " << errCorr << std::endl;
    std::cout << "========================================" << std::endl;

    
    TFile* fOut = new TFile(outputName + ".root", "RECREATE");
    hDataRaw->Write("hDataRaw");
    hMCRec->Write();
    hMCRecReco->Write();
    hMCGen->Write();
    hEff->Write();
    hEffReco->Write();
    // hEffBiasCorr->Write(); // 保存Bias修正后的效率
    // hCorrected->Write("hCorrected_BiasCorrEff");
    hCorrectedNoBias->Write("hCorrected_NoBias");
    hCorrectedRecoEff->Write("hCorrected_RecoEff");
    hDev->Write(); 
    pBias->Write(); 
    if (hDataGen) hDataGen->Write();
    fOut->Close();

    std::cout << "Analysis Complete." << std::endl;
    std::cout << "Plot saved to " << outputName << ".png" << std::endl;
    std::cout << "Root file saved to " << outputName << ".root" << std::endl;
}

