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
    gStyle->SetCanvasColor(kWhite);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);

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
    gStyle->SetTitleOffset(1.4, "X");
    gStyle->SetTitleOffset(0.70, "Y"); // 宽画幅下，Y轴标题贴近一点更好看

    // 刻度线长度 (增加一点长度，更有质感)
    gStyle->SetTickLength(0.02, "XY");
    
    // 统计框 (StatBox) - 如果需要显示，必须调整字体和位置，否则会很丑
    gStyle->SetStatFont(42);
    gStyle->SetStatFontSize(0.04);
    gStyle->SetStatBorderSize(2); // 边框加粗
    // 设置绘图风格
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
    // gStyle->SetTextSize(0.05);
    // gStyle->SetLabelSize(0.05, "XYZ");
    // gStyle->SetTitleSize(0.06, "XYZ");
    // gStyle->SetTitleOffset(1.4, "Y");
    gStyle->SetPadLeftMargin(0.18);
    gStyle->SetPadBottomMargin(0.15);

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
    hEff->SetTitle("Efficiency (Truth Var);Variable;Efficiency");
    hEff->Divide(hMCRec, hMCGen, 1, 1, "B"); 

    // 4.2 Reco效率 (Reco Variable based)
    TH1F* hEffReco = (TH1F*)hMCRecReco->Clone("hEffReco");
    hEffReco->SetTitle("Efficiency (Reco Var);Variable;Efficiency");
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
    hCorrectedNoBias->SetTitle("Corrected (Raw / TruthEff);Variable;#Events");
    hCorrectedNoBias->Divide(hEff);

    // 5.3 Reco效率修正: Raw Data / Reco Eff (自动包含Bias和Smearing)
    TH1F* hCorrectedRecoEff = (TH1F*)hDataRaw->Clone("hCorrectedRecoEff");
    hCorrectedRecoEff->SetTitle("Corrected (Raw / RecoEff);Variable;#Events");
    hCorrectedRecoEff->Divide(hEffReco);

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
    
    TLegend* leg1 = new TLegend(0.55, 0.7, 0.9, 0.9);
    leg1->AddEntry(hDataRaw, "Data (Raw)", "lp");
    leg1->AddEntry(hMCRecRecoScaled, "MC Rec (RecoVar)", "l");
    leg1->Draw();

    // Pad 2: MC 生成 vs MC 重建 (Truth Var)
    c1->cd(2);
    hMCGen->SetLineColor(kBlue);
    hMCGen->Draw("HIST");
    hMCRec->SetLineColor(kRed);
    hMCRec->Draw("HIST SAME");
    
    TLegend* leg2 = new TLegend(0.65, 0.7, 0.9, 0.9);
    leg2->AddEntry(hMCGen, "MC Gen", "l");
    leg2->AddEntry(hMCRec, "MC Rec (TruthVar)", "l");
    leg2->Draw();

    // Pad 3: 偏差直方图 (Global Resolution)
    c1->cd(3);
    hDev->SetLineColor(kBlue);
    hDev->SetLineWidth(2);
    hDev->Draw();
    
    TLegend* leg3 = new TLegend(0.55, 0.7, 0.9, 0.9);
    leg3->AddEntry(hDev, "Bias Distribution", "l");
    leg3->Draw();

    // Pad 4: 效率曲线对比
    c1->cd(4);
    hEff->SetLineColor(kGreen+2);
    hEff->SetLineWidth(2);
    hEff->SetMarkerColor(kGreen+2);
    hEff->SetMarkerStyle(21);
    hEff->SetMinimum(0.0);
    hEff->SetMaximum(1.2);
    hEff->Draw("E");
    
    hEffReco->SetLineColor(kOrange+7);
    hEffReco->SetLineWidth(2);
    hEffReco->SetMarkerColor(kOrange+7);
    hEffReco->SetMarkerStyle(22);
    hEffReco->Draw("E SAME");

    /*
    hEffBiasCorr->SetLineColor(kMagenta+2);
    hEffBiasCorr->SetMarkerStyle(23);
    hEffBiasCorr->Draw("E SAME");
    */

    TLegend* leg4 = new TLegend(0.55, 0.6, 0.9, 0.9);
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
        yMax *= 1.2;
    }

    // Pad 5: 无Bias修正 (NoBias + TruthEff) vs Truth
    c1->cd(5);
    hCorrectedNoBias->SetLineColor(kAzure+7);
    hCorrectedNoBias->SetMarkerStyle(24);
    hCorrectedNoBias->SetMinimum(0.0);
    hCorrectedNoBias->SetMaximum(yMax);
    hCorrectedNoBias->Draw("E");

    // 标注超出Y轴范围的点
    for (int i = 1; i <= hCorrectedNoBias->GetNbinsX(); ++i) {
        double content = hCorrectedNoBias->GetBinContent(i);
        if (content > yMax) {
            double x = hCorrectedNoBias->GetBinCenter(i);
            // 画一个向上的红色箭头
            TArrow *arrow = new TArrow(x, yMax * 0.8, x, yMax * 0.95, 0.02, "|>");
            arrow->SetLineColor(kRed);
            arrow->SetFillColor(kRed);
            arrow->SetLineWidth(2);
            arrow->Draw();
            
            // 标注具体数值
            TLatex *latex = new TLatex(x, yMax * 0.96, Form("%.0f", content));
            latex->SetTextSize(0.04);
            latex->SetTextAlign(21); // Center-Bottom
            latex->SetTextColor(kRed);
            latex->Draw();
        }
    }

    if (hDataGen) {
        hDataGen->SetLineColor(kMagenta);
        hDataGen->SetLineStyle(2);
        hDataGen->SetLineWidth(2);
        hDataGen->Draw("HIST SAME");
        
        TLegend* leg5 = new TLegend(0.55, 0.7, 0.9, 0.9);
        leg5->AddEntry(hCorrectedNoBias, "Corr (Raw/TruthEff)", "lp");
        leg5->AddEntry(hDataGen, "Truth", "l");
        leg5->Draw();
    }

    // Pad 6: Reco效率修正 (Raw / RecoEff) vs Truth
    c1->cd(6);
    hCorrectedRecoEff->SetLineColor(kOrange+1);
    hCorrectedRecoEff->SetMarkerStyle(25);
    hCorrectedRecoEff->SetMinimum(0.0);
    hCorrectedRecoEff->SetMaximum(yMax);
    hCorrectedRecoEff->Draw("E");

    if (hDataGen) {
        hDataGen->Draw("HIST SAME");
        TLegend* leg6 = new TLegend(0.55, 0.7, 0.9, 0.9);
        leg6->AddEntry(hCorrectedRecoEff, "Corr (Raw/RecoEff)", "lp");
        leg6->AddEntry(hDataGen, "Truth", "l");
        leg6->Draw();
    }

    // Pad 7: 修正结果对比
    c1->cd(7);
    // hCorrected->Draw("E"); // Standard (BiasCorrEff)
    hCorrectedNoBias->SetMinimum(0.0);
    hCorrectedNoBias->SetMaximum(yMax); // Ensure consistent scale
    hCorrectedNoBias->Draw("E"); // No Bias
    hCorrectedRecoEff->Draw("E SAME"); // Reco Eff
    if (hDataGen) hDataGen->Draw("HIST SAME");

    TLegend* leg7 = new TLegend(0.55, 0.6, 0.9, 0.9);
    // leg7->AddEntry(hCorrected, "BiasCorrEff", "lp");
    leg7->AddEntry(hCorrectedNoBias, "No Bias", "lp");
    leg7->AddEntry(hCorrectedRecoEff, "Reco Eff", "lp");
    if (hDataGen) leg7->AddEntry(hDataGen, "Truth", "l");
    leg7->Draw();

    // Pad 8: 统计信息输出
    c1->cd(8);
    TPaveText *pt = new TPaveText(0.1, 0.3, 0.9, 0.7, "NDC");
    pt->SetFillColor(kWhite);
    pt->SetBorderSize(1);
    pt->SetTextAlign(12); // Left-Center
    pt->SetTextSize(0.05);
    
    pt->AddText("Event Statistics (Integral):");
    pt->AddText("--------------------------------");
    
    double errRaw, errGen, errNoBias, errRecoEff;
    double nRaw = hDataRaw->IntegralAndError(1, hDataRaw->GetNbinsX(), errRaw);
    pt->AddText(Form("N_{RealData} (Raw): %.1f #pm %.1f", nRaw, errRaw));
    
    if (hDataGen) {
        double nGen = hDataGen->IntegralAndError(1, hDataGen->GetNbinsX(), errGen);
        pt->AddText(Form("N_{Truth} (Gen): %.1f #pm %.1f", nGen, errGen));
    } else {
        pt->AddText("N_{Truth}: N/A");
    }
    
    pt->AddText("--------------------------------");
    double nNoBias = hCorrectedNoBias->IntegralAndError(1, hCorrectedNoBias->GetNbinsX(), errNoBias);
    pt->AddText(Form("N_{Corr} (NoBias): %.1f #pm %.1f", nNoBias, errNoBias));
    
    double nRecoEff = hCorrectedRecoEff->IntegralAndError(1, hCorrectedRecoEff->GetNbinsX(), errRecoEff);
    pt->AddText(Form("N_{Corr} (RecoEff): %.1f #pm %.1f", nRecoEff, errRecoEff));
    
    pt->Draw();

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
