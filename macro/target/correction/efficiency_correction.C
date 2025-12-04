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
    // 设置绘图风格
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
    gStyle->SetTextSize(0.05);
    gStyle->SetLabelSize(0.05, "XYZ");
    gStyle->SetTitleSize(0.06, "XYZ");
    gStyle->SetTitleOffset(0.8, "Y");
    gStyle->SetPadLeftMargin(0.15);
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
    TF1* fBias = new TF1("fBias", "pol1", xMin, xMax);
    pBias->Fit(fBias, "Q"); // Q for quiet
    double p0 = fBias->GetParameter(0);
    double p1 = fBias->GetParameter(1);

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Global Bias (Mean): " << globalBias << " cm" << std::endl;
    std::cout << "Position-dependent Bias Fit (pol1): " << p0 << " + " << p1 << " * Z" << std::endl;
    std::cout << "Applying dynamic bias correction..." << std::endl;
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

    // Data (Raw) - 未修正偏差
    TH1F* hDataRaw = new TH1F("hDataRaw", "Real Data (Raw);Z (cm);#Events", nBins, xMin, xMax);
    tData->Draw(plotVar + ">>hDataRaw", recCut);
    hDataRaw->SetDirectory(0);

    // Data (Bias Corrected) - 动态修正偏差
    // Z_corr = Z_raw - (p0 + p1 * Z_raw)
    TH1F* hDataBiasCorr = new TH1F("hDataBiasCorr", "Real Data (Dyn Bias Corr);Z (cm);#Events", nBins, xMin, xMax);
    tData->Draw(Form("%s - (%f + %f*%s) >> hDataBiasCorr", plotVar.Data(), p0, p1, plotVar.Data()), recCut);
    hDataBiasCorr->SetDirectory(0); 

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

    // ---------------------------------------------------------
    // 5. 修正真实数据 (Correction)
    // ---------------------------------------------------------
    // 5.1 标准修正: Bias Corrected Data / Truth Eff
    TH1F* hCorrected = (TH1F*)hDataBiasCorr->Clone("hCorrected");
    hCorrected->SetTitle("Corrected (BiasCorr + TruthEff);Variable;#Events");
    hCorrected->Divide(hEff);

    // 5.2 无Bias修正: Raw Data / Truth Eff
    TH1F* hCorrectedNoBias = (TH1F*)hDataRaw->Clone("hCorrectedNoBias");
    hCorrectedNoBias->SetTitle("Corrected (NoBias + TruthEff);Variable;#Events");
    hCorrectedNoBias->Divide(hEff);

    // 5.3 Reco效率修正: Raw Data / Reco Eff (通常不需要Bias修正，因为Reco Eff包含了位置迁移)
    TH1F* hCorrectedRecoEff = (TH1F*)hDataRaw->Clone("hCorrectedRecoEff");
    hCorrectedRecoEff->SetTitle("Corrected (Raw + RecoEff);Variable;#Events");
    hCorrectedRecoEff->Divide(hEffReco);

    // ---------------------------------------------------------
    // 6. 绘图与保存
    // ---------------------------------------------------------
    TCanvas* c1 = new TCanvas("c1", "Efficiency Correction Analysis", 2400, 1200);
    c1->Divide(4, 2); // 4列2行

    // Pad 1: 原始数据(已修正Bias) vs MC重建(Truth Var)
    c1->cd(1);
    hDataBiasCorr->SetLineColor(kBlack);
    hDataBiasCorr->SetMarkerStyle(20);
    hDataBiasCorr->Draw("E");
    
    TH1F* hMCRecScaled = (TH1F*)hMCRec->Clone("hMCRecScaled");
    if (hMCRec->Integral() > 0) hMCRecScaled->Scale(hDataBiasCorr->Integral() / hMCRec->Integral());
    hMCRecScaled->SetLineColor(kRed);
    hMCRecScaled->Draw("HIST SAME");
    
    TLegend* leg1 = new TLegend(0.55, 0.7, 0.9, 0.9);
    leg1->AddEntry(hDataBiasCorr, "Data (BiasCorr)", "lp");
    leg1->AddEntry(hMCRecScaled, "MC Rec (TruthVar)", "l");
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

    // Pad 3: 偏差 (Profile & Fit)
    c1->cd(3);
    pBias->SetLineColor(kBlue);
    pBias->SetMarkerStyle(20);
    pBias->SetMinimum(devMin);
    pBias->SetMaximum(devMax);
    pBias->Draw();
    
    fBias->SetLineColor(kRed);
    fBias->Draw("SAME");
    
    TLegend* leg3 = new TLegend(0.55, 0.7, 0.9, 0.9);
    leg3->AddEntry(pBias, "Bias Profile", "lp");
    leg3->AddEntry(fBias, Form("Fit: %.3f + %.3f*Z", p0, p1), "l");
    leg3->Draw();

    // Pad 4: 效率曲线对比
    c1->cd(4);
    hEff->SetLineColor(kGreen+2);
    hEff->SetMarkerStyle(21);
    hEff->SetMinimum(0.0);
    hEff->SetMaximum(1.1);
    hEff->Draw("E");
    
    hEffReco->SetLineColor(kOrange+7);
    hEffReco->SetMarkerStyle(22);
    hEffReco->Draw("E SAME");

    TLegend* leg4 = new TLegend(0.55, 0.7, 0.9, 0.9);
    leg4->AddEntry(hEff, "Eff (Truth Var)", "lp");
    leg4->AddEntry(hEffReco, "Eff (Reco Var)", "lp");
    leg4->Draw();

    // Pad 5: 无Bias修正 (NoBias + TruthEff) vs Truth
    c1->cd(5);
    hCorrectedNoBias->SetLineColor(kAzure+7);
    hCorrectedNoBias->SetMarkerStyle(24);
    hCorrectedNoBias->Draw("E");

    if (hDataGen) {
        hDataGen->SetLineColor(kMagenta);
        hDataGen->SetLineStyle(2);
        hDataGen->SetLineWidth(2);
        hDataGen->Draw("HIST SAME");
        
        TLegend* leg5 = new TLegend(0.55, 0.7, 0.9, 0.9);
        leg5->AddEntry(hCorrectedNoBias, "Corr (NoBias+TruthEff)", "lp");
        leg5->AddEntry(hDataGen, "Truth", "l");
        leg5->Draw();
    }

    // Pad 6: 标准修正 (BiasCorr + TruthEff) vs Truth
    c1->cd(6);
    hCorrected->SetLineColor(kBlue+2);
    hCorrected->SetMarkerStyle(20);
    hCorrected->Draw("E");

    if (hDataGen) {
        hDataGen->Draw("HIST SAME");
        
        TLegend* leg6 = new TLegend(0.55, 0.7, 0.9, 0.9);
        leg6->AddEntry(hCorrected, "Corr (Bias+TruthEff)", "lp");
        leg6->AddEntry(hDataGen, "Truth", "l");
        leg6->Draw();
    }

    // Pad 7: Reco效率修正 (Raw + RecoEff) vs Truth
    c1->cd(7);
    hCorrectedRecoEff->SetLineColor(kOrange+1);
    hCorrectedRecoEff->SetMarkerStyle(25);
    hCorrectedRecoEff->Draw("E");

    if (hDataGen) {
        hDataGen->Draw("HIST SAME");
        TLegend* leg7 = new TLegend(0.55, 0.7, 0.9, 0.9);
        leg7->AddEntry(hCorrectedRecoEff, "Corr (Raw+RecoEff)", "lp");
        leg7->AddEntry(hDataGen, "Truth", "l");
        leg7->Draw();
    }

    // Pad 8: 三种修正结果对比
    c1->cd(8);
    hCorrected->Draw("E"); // Standard
    hCorrectedNoBias->Draw("E SAME"); // No Bias
    hCorrectedRecoEff->Draw("E SAME"); // Reco Eff
    if (hDataGen) hDataGen->Draw("HIST SAME");

    TLegend* leg8 = new TLegend(0.55, 0.6, 0.9, 0.9);
    leg8->AddEntry(hCorrected, "Standard", "lp");
    leg8->AddEntry(hCorrectedNoBias, "No Bias", "lp");
    leg8->AddEntry(hCorrectedRecoEff, "Reco Eff", "lp");
    if (hDataGen) leg8->AddEntry(hDataGen, "Truth", "l");
    leg8->Draw();

    // 保存结果
    c1->SaveAs("efficiency_correction_result.png");
    
    TFile* fOut = new TFile("corrected_data_output.root", "RECREATE");
    hDataBiasCorr->Write("hDataBiasCorr");
    hDataRaw->Write("hDataRaw");
    hMCRec->Write();
    hMCRecReco->Write();
    hMCGen->Write();
    hEff->Write();
    hEffReco->Write();
    hCorrected->Write("hCorrected_Standard");
    hCorrectedNoBias->Write("hCorrected_NoBias");
    hCorrectedRecoEff->Write("hCorrected_RecoEff");
    hDev->Write(); 
    pBias->Write(); // 保存Profile
    if (hDataGen) hDataGen->Write();
    fOut->Close();

    std::cout << "Analysis Complete." << std::endl;
    std::cout << "Plot saved to efficiency_correction_result.png" << std::endl;
    std::cout << "Root file saved to corrected_data_output.root" << std::endl;
}
