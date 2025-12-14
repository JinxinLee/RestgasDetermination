#include <iostream>
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
 * root -l 'efficiency_correction.C("data_ana.root", "mc_ana.root", "mc_sim.root")'
 */
void efficiency_correction(
    TString realDataFile = "data_ana_final.root", // 真实数据文件 (经过ana)
    TString mcRecFile = "mc_ana_final.root",      // MC重建数据文件 (经过ana)
    TString mcGenFile = "mc_sim.root",            // MC生成数据文件 (sim输出，包含MCTrack)
    TString realDataGenFile = "",                 // 真实数据对应的生成文件 (可选，用于验证)
    TString plotVar = "pvz",                      // 需要修正的变量名 (在ana文件的ntpDp树中)
    TString mcRecVar = "pvz_mc",                  // MC重建变量 (用于效率分子，通常使用MC真值)
    TString genVar = "MCTrack.fStartZ",           // 对应的生成级变量 (在sim文件的pndsim树中)
    TString recCut = "pvz != -999.0",             // 重建数据的筛选条件
    TString genCut = "MCTrack.fMotherID==-1&&MCTrack.fPdgCode==2212", // 生成级粒子的筛选条件
    int nBins = 100,
    double xMin = -50.0,
    double xMax = 50.0,
    double devMin = -1.0,
    double devMax = 1.0
)
{
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
    gStyle->SetLineWidth(4);      
    gStyle->SetFrameLineWidth(4); // 坐标轴边框
    gStyle->SetHistLineWidth(4);  // 直方图线条
    gStyle->SetFuncWidth(4);      // 函数/拟合曲线
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
    tMCRec->Add(mcRecFile);
    if (tMCRec->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in MC Rec file(s): " << mcRecFile << std::endl; 
        return; 
    }

    // MC Rec (Truth Variable) - 用于标准效率计算
    TH1F* hMCRec = new TH1F("hMCRec", "MC Reconstructed (Truth Var);Z (cm);#Events", nBins, xMin, xMax);
    tMCRec->Draw(mcRecVar + ">>hMCRec", recCut);
    hMCRec->SetDirectory(0);

    // MC Rec (Reco Variable) - 用于Reco-based效率计算
    TH1F* hMCRecReco = new TH1F("hMCRecReco", "MC Reconstructed (Reco Var);Z (cm);#Events", nBins, xMin, xMax);
    tMCRec->Draw(plotVar + ">>hMCRecReco", recCut);
    hMCRecReco->SetDirectory(0);

    // 计算偏差 (Reco - Truth)
    // 1. 全局偏差分布 (1D)
    TH1F* hDev = new TH1F("hDev", "Global Resolution;Rec - Truth (cm);#Events", 100, devMin, devMax);
    tMCRec->Draw(Form("%s - %s >> hDev", plotVar.Data(), mcRecVar.Data()), recCut);
    hDev->SetDirectory(0);
    double globalBias = hDev->GetMean();

    // 2. 偏差随Z的变化 (Profile) - 用于动态修正
    TProfile* pBias = new TProfile("pBias", "Bias vs Z (Profile);Z_{reco} (cm);<Bias> (cm)", nBins, xMin, xMax);
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
    tData->Add(realDataFile);
    if (tData->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in Data file(s): " << realDataFile << std::endl; 
        return; 
    }

    // Data (Raw) - 保持原始数据，不直接修正Bias
    TH1F* hDataRaw = new TH1F("hDataRaw", "Real Data (Raw);Z (cm);#Events", nBins, xMin, xMax);
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
    tMCGen->Add(mcGenFile);
    if (tMCGen->GetEntries() == 0) {
        delete tMCGen;
        tMCGen = new TChain("cbmsim");
        tMCGen->Add(mcGenFile);
    }
    
    if (tMCGen->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'cbmsim' or 'pndsim' in MC Gen file(s): " << mcGenFile << std::endl; 
        return; 
    }

    TH1F* hMCGen = new TH1F("hMCGen", "MC Generated;Z (cm);#Events", nBins, xMin, xMax);
    
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
        tDataGen->Add(realDataGenFile);
        if (tDataGen->GetEntries() == 0) {
            delete tDataGen;
            tDataGen = new TChain("cbmsim");
            tDataGen->Add(realDataGenFile);
        }

        if (tDataGen->GetEntries() > 0) {
            hDataGen = new TH1F("hDataGen", "Real Data Generated (Truth);Z (cm);#Events", nBins, xMin, xMax);
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
        } else {
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
    TCanvas* c1 = new TCanvas("c1", "Efficiency Correction Analysis", 5000, 2500);
    c1->Divide(4, 2); // 4列2行

    // Pad 1: 原始数据(Raw) vs MC重建(Reco Var) - 形状对比
    c1->cd(1);
    hDataRaw->SetLineColor(kBlack);
    hDataRaw->SetMarkerStyle(20);
    hDataRaw->Draw("E");
    
    TH1F* hMCRecRecoScaled = (TH1F*)hMCRecReco->Clone("hMCRecRecoScaled");
    if (hMCRecReco->Integral() > 0) hMCRecRecoScaled->Scale(hDataRaw->Integral() / hMCRecReco->Integral());
    hMCRecRecoScaled->SetLineColor(kRed);
    hMCRecRecoScaled->Draw("HIST SAME");
    
    TLegend* leg1 = new TLegend(0.6, 0.78, 0.9, 0.9);
    leg1->SetFillStyle(0);
    leg1->SetBorderSize(0);
    leg1->AddEntry(hDataRaw, "Data (Raw)", "lp");
    leg1->AddEntry(hMCRecRecoScaled, "MC Rec (RecoVar)", "l");
    leg1->Draw();

    // Pad 2: MC 生成 vs MC 重建 (Truth Var)
    c1->cd(2);
    hMCGen->SetLineColor(kBlue);
    hMCGen->Draw("HIST");
    hMCRec->SetLineColor(kRed);
    hMCRec->Draw("HIST SAME");
    
    TLegend* leg2 = new TLegend(0.6, 0.78, 0.9, 0.9);
    leg2->SetFillStyle(0);
    leg2->SetBorderSize(0);
    leg2->AddEntry(hMCGen, "MC Gen", "l");
    leg2->AddEntry(hMCRec, "MC Rec (TruthVar)", "l");
    leg2->Draw();

    // Pad 3: 偏差直方图 (Global Resolution)
    c1->cd(3);
    hDev->SetLineColor(kBlue);
    //hDev->SetLineWidth(3);
    hDev->Draw();
    
    TLegend* leg3 = new TLegend(0.6, 0.78, 0.9, 0.9);
    leg3->SetFillStyle(0);
    leg3->SetBorderSize(0);
    leg3->AddEntry(hDev, "Bias Distribution", "l");
    leg3->Draw();

    // Pad 4: 效率曲线对比
    c1->cd(4);
    hEff->SetLineColor(kGreen+2);
    //hEff->SetLineWidth(3);
    hEff->SetMarkerColor(kGreen+2);
    hEff->SetMarkerStyle(21);
    hEff->SetMinimum(0.0);
    hEff->SetMaximum(1.2);
    hEff->Draw("E");
    
    hEffReco->SetLineColor(kOrange+7);
    //hEffReco->SetLineWidth(3);
    hEffReco->SetMarkerColor(kOrange+7);
    hEffReco->SetMarkerStyle(22);
    hEffReco->Draw("E SAME");

    /*
    hEffBiasCorr->SetLineColor(kMagenta+2);
    hEffBiasCorr->SetMarkerStyle(23);
    hEffBiasCorr->Draw("E SAME");
    */

    TLegend* leg4 = new TLegend(0.6, 0.78, 0.9, 0.9);
    leg4->SetFillStyle(0);
    leg4->SetBorderSize(0);
    leg4->AddEntry(hEff, "Eff (Truth Var)", "lp");
    leg4->AddEntry(hEffReco, "Eff (Reco Var)", "lp");
    // leg4->AddEntry(hEffBiasCorr, "Eff (Bias Corr)", "lp");
    leg4->Draw();

    // 统一纵坐标范围 (以Truth为标准，忽略修正后的异常高点)
    double yMax = 0;
    if (hDataGen) {
        yMax = hDataGen->GetMaximum() * 1.2;
    } else {
        // 如果没有Truth，则使用修正数据的最大值
        if (hCorrectedNoBias->GetMaximum() > yMax) yMax = hCorrectedNoBias->GetMaximum();
        if (hCorrectedRecoEff->GetMaximum() > yMax) yMax = hCorrectedRecoEff->GetMaximum();
        if (hCorrectedHybrid->GetMaximum() > yMax) yMax = hCorrectedHybrid->GetMaximum();
        yMax *= 1.2;
    }

    // Pad 5: Hybrid Correction (Raw / HybridEff) vs Truth
    c1->cd(5);
    hCorrectedHybrid->SetLineColor(kOrange+1);
    hCorrectedHybrid->SetLineWidth(3);
    hCorrectedHybrid->SetMarkerColor(kOrange+1);
    hCorrectedHybrid->SetMarkerStyle(21);
    hCorrectedHybrid->SetMinimum(0.0);
    hCorrectedHybrid->SetMaximum(yMax);
    hCorrectedHybrid->Draw("E");

    if (hDataGen) {
        hDataGen->Draw("HIST SAME");
        TLegend* leg5 = new TLegend(0.6, 0.78, 0.9, 0.9);
        leg5->SetFillStyle(0);
        leg5->SetBorderSize(0);
        leg5->AddEntry(hCorrectedHybrid, "Corr (Hybrid)", "lp");
        leg5->AddEntry(hDataGen, "Truth", "l");
        leg5->Draw();
    }

    // Pad 6: 修正结果对比
    c1->cd(6);
    // hCorrected->Draw("E"); // Standard (BiasCorrEff)
    hCorrectedHybrid->SetMinimum(0.0);
    hCorrectedHybrid->SetMaximum(yMax); // Ensure consistent scale
    hCorrectedHybrid->Draw("E"); // Hybrid
    if (hDataGen) hDataGen->Draw("HIST SAME");

    TLegend* leg6 = new TLegend(0.6, 0.72, 0.9, 0.9);
    leg6->SetFillStyle(0);
    leg6->SetBorderSize(0);
    // leg6->AddEntry(hCorrected, "BiasCorrEff", "lp");
    leg6->AddEntry(hCorrectedHybrid, "Hybrid Corr", "lp");
    if (hDataGen) leg6->AddEntry(hDataGen, "Truth", "l");
    leg6->Draw();

    // Pad 7: Difference (Corrected - Truth)
    c1->cd(7);
    if (hDataGen) {
        TH1F* hDiff = (TH1F*)hCorrectedHybrid->Clone("hDiff");
        hDiff->SetTitle("Difference (Corrected - Truth);Z (cm);#Delta Events");
        hDiff->Add(hDataGen, -1.0); // hDiff = hCorrectedHybrid - hDataGen
        hDiff->SetLineColor(kRed);
        hDiff->SetLineWidth(2);
        hDiff->SetMarkerStyle(20);
        hDiff->SetMarkerColor(kRed);
        hDiff->Draw("E");
        
        // Draw a zero line
        TLine *line = new TLine(xMin, 0, xMax, 0);
        line->SetLineStyle(2);
        line->SetLineColor(kBlack);
        line->Draw();
        
        TLegend* leg7 = new TLegend(0.6, 0.78, 0.9, 0.9);
        leg7->SetFillStyle(0);
        leg7->SetBorderSize(0);
        leg7->AddEntry(hDiff, "Corr - Truth", "lp");
        leg7->Draw();
    }

    // Pad 8: 统计信息输出
    c1->cd(8);
    TPaveText *pt = new TPaveText(0.1, 0.3, 0.9, 0.7, "NDC");
    pt->SetFillColor(kWhite);
    pt->SetBorderSize(1);
    pt->SetTextAlign(12); // Left-Center
    pt->SetTextSize(0.05);
    
    pt->AddText("Event Statistics (Integral):");
    pt->AddText("--------------------------------");
    
    double errRaw, errGen, errHybrid;
    double nRaw = hDataRaw->IntegralAndError(1, hDataRaw->GetNbinsX(), errRaw);
    pt->AddText(Form("N_{RealData} (Raw): %.1f #pm %.1f", nRaw, errRaw));
    
    if (hDataGen) {
        double nGen = hDataGen->IntegralAndError(1, hDataGen->GetNbinsX(), errGen);
        pt->AddText(Form("N_{Truth} (Gen): %.1f #pm %.1f", nGen, errGen));
    } else {
        pt->AddText("N_{Truth}: N/A");
    }
    
    pt->AddText("--------------------------------");
    double nHybrid = hCorrectedHybrid->IntegralAndError(1, hCorrectedHybrid->GetNbinsX(), errHybrid);
    pt->AddText(Form("N_{Corr} (Hybrid): %.1f #pm %.1f", nHybrid, errHybrid));
    
    double nRecoEff = hCorrectedRecoEff->IntegralAndError(1, hCorrectedRecoEff->GetNbinsX(), errRecoEff);
    pt->AddText(Form("N_{Corr} (RecoEff): %.1f #pm %.1f", nRecoEff, errRecoEff));
    
    pt->Draw();

    gPad->RedrawAxis();

    // 保存结果
    c1->SaveAs("efficiency_correction_result.png");
    
    TFile* fOut = new TFile("corrected_data_output.root", "RECREATE");
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
    std::cout << "Plot saved to efficiency_correction_result.png" << std::endl;
    std::cout << "Root file saved to corrected_data_output.root" << std::endl;
}
