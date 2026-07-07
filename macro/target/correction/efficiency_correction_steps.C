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
#include "TLine.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TTreeFormula.h"
#include "TLatex.h"

// -------------------------------------------------------------------
// 辅助函数部分 (保持不变)
// -------------------------------------------------------------------

void AddToChain(TChain* chain, TString input) {
    if (input.Contains(",")) {
        TObjArray* tokens = input.Tokenize(",");
        if (tokens->GetEntries() >= 3) {
            TString format = ((TObjString*)tokens->At(0))->GetString();
            int start = ((TObjString*)tokens->At(1))->GetString().Atoi();
            int end = ((TObjString*)tokens->At(2))->GetString().Atoi();
            for (int i = start; i <= end; ++i) chain->Add(Form(format.Data(), i));
        } else { chain->Add(input); }
        delete tokens;
    } else { chain->Add(input); }
}

void FillWeightedHisto(TChain* tree, TString varToFill, TString varForWeight, TString cut, TH1F* hToFill, TH1F* hWeightMap) {
    TTreeFormula* fVarFill = new TTreeFormula("fVarFill", varToFill, tree);
    TTreeFormula* fVarWeight = new TTreeFormula("fVarWeight", varForWeight, tree);
    TTreeFormula* fCut = new TTreeFormula("fCut", cut, tree);
    tree->SetNotify(fVarFill); tree->SetNotify(fVarWeight); tree->SetNotify(fCut);
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->LoadTree(i);
        if (fCut->EvalInstance() > 0) {
            double valFill = fVarFill->EvalInstance();
            double valWeight = fVarWeight->EvalInstance(); 
            int bin = hWeightMap->FindBin(valWeight);
            if (bin >= 1 && bin <= hWeightMap->GetNbinsX()) {
                double weight = hWeightMap->GetBinContent(bin);
                if (weight > 0) hToFill->Fill(valFill, weight);
            }
        }
    }
    delete fVarFill; delete fVarWeight; delete fCut;
}

TH1F* ApplyHybridCorrection(TH1F* hDataRaw, TH1F* hEffTruth, TH1F* hEffReco, double biasScaleFactor) {
    TH1F* hCorrected = (TH1F*)hDataRaw->Clone();
    hCorrected->Reset();
    double scale = 1.0 / biasScaleFactor; // 如果 biasScaleFactor=1.0，则不缩放

    for (int i = 1; i <= hDataRaw->GetNbinsX(); ++i) {
        double content = hDataRaw->GetBinContent(i);
        double error = hDataRaw->GetBinError(i);
        double center = hDataRaw->GetBinCenter(i);
        double eff = 0;
        double eff_err = 0;

        if (center >= -2.0 && center <= 2.0) { // Target Region: Truth Eff
             int binEff = hEffTruth->FindBin(center);
             eff = hEffTruth->GetBinContent(binEff);
             eff_err = hEffTruth->GetBinError(binEff);
        } else { // Gas Region: Reco Eff
             int binEff = hEffReco->FindBin(center);
             eff = hEffReco->GetBinContent(binEff);
             eff_err = hEffReco->GetBinError(binEff);
        }

        if (eff > 0) {
            double val_eff_corr = content / eff;
            double rel_err_content = (content > 0) ? error / content : 0;
            double rel_err_eff = eff_err / eff;
            double err_eff_corr = val_eff_corr * sqrt(rel_err_content*rel_err_content + rel_err_eff*rel_err_eff);
            
            hCorrected->SetBinContent(i, val_eff_corr * scale);
            hCorrected->SetBinError(i, err_eff_corr * scale);
        }
    }
    return hCorrected;
}

// -------------------------------------------------------------------
// 主程序
// -------------------------------------------------------------------
void efficiency_correction_steps(
    TString realDataFile = "data_ana_final.root", 
    TString mcRecFile = "mc_ana_final.root",      
    TString mcGenFile = "mc_sim.root",            
    TString realDataGenFile = "",                 
    TString outputName = "eff_steps_result", 
    TString plotVar = "pvz",                      
    TString mcRecVar = "pvz_mc",                  
    TString genVar = "MCTrack.fStartZ",           
    TString recCut = "pvz != -999.0",             
    TString genCut = "MCTrack.fMotherID==-1&&MCTrack.fPdgCode==2212", 
    int nBins = 300,
    double xMin = -600.0,
    double xMax = 1100.0
)
{
    // [MODIFICATION 1] 强制关闭 Bias 修正
    double globalBiasFactor = 1.0; 
    std::cout << ">>> WARNING: Systematic Bias Correction is set to " << globalBiasFactor << " (OFF) <<<" << std::endl;

    // Binning Setup (Variable Bins)
    std::vector<double> edges;
    double centerMin = -50.0; double centerMax = 50.0;
    int nCenter = 200; int nOuterTotal = 200;
    double rMin = xMin; double rMax = xMax;
    double widthLeft = (centerMin > rMin) ? (centerMin - rMin) : 0;
    double widthRight = (rMax > centerMax) ? (rMax - centerMax) : 0;
    double widthOuter = widthLeft + widthRight;
    int nLeft = 0; int nRight = 0;
    if (widthOuter > 0) { nLeft = (int)(nOuterTotal * widthLeft / widthOuter); nRight = nOuterTotal - nLeft; }
    if (nLeft > 0) { double step = widthLeft/nLeft; for(int i=0; i<nLeft; ++i) edges.push_back(rMin + i*step); }
    if (centerMax > centerMin) { double step = (centerMax-centerMin)/nCenter; for(int i=0; i<nCenter; ++i) edges.push_back(centerMin + i*step); }
    if (nRight > 0) { double step = widthRight/nRight; for(int i=0; i<=nRight; ++i) edges.push_back(centerMax + i*step); }
    else { edges.push_back(centerMax); }
    int nBinsVar = edges.size() - 1;
    double* xBinsVar = edges.data();
    
    // Style
    gStyle->SetOptStat(0); gStyle->SetOptTitle(1);
    gStyle->SetLineWidth(2);

    // Load Chains
    TChain* tMCRec = new TChain("ntpDp"); AddToChain(tMCRec, mcRecFile);
    TChain* tData = new TChain("ntpDp"); AddToChain(tData, realDataFile);
    TChain* tMCGen = new TChain("pndsim"); AddToChain(tMCGen, mcGenFile);
    if(tMCGen->GetEntries()==0) { delete tMCGen; tMCGen = new TChain("cbmsim"); AddToChain(tMCGen, mcGenFile); }

    if (!tMCRec->GetEntries() || !tData->GetEntries() || !tMCGen->GetEntries()) return;

    // ---------------------------------------------------------
    // [ITER 0] 初始猜测 (No Weighting)
    // ---------------------------------------------------------
    std::cout << "\n=== Iteration 0 ===\n";
    TH1F* hMCRec_Truth_0 = new TH1F("hMCRec_Truth_0", "MC Rec Truth Iter0", nBinsVar, xBinsVar);
    tMCRec->Draw(mcRecVar + ">>hMCRec_Truth_0", recCut); 
    TH1F* hMCRec_Reco_0 = new TH1F("hMCRec_Reco_0", "MC Rec Reco Iter0", nBinsVar, xBinsVar);
    tMCRec->Draw(plotVar + ">>hMCRec_Reco_0", recCut);
    TH1F* hMCGen_0 = new TH1F("hMCGen_0", "MC Gen Iter0", nBinsVar, xBinsVar);
    tMCGen->Draw(genVar + ">>hMCGen_0", genCut);

    TH1F* hEff_Truth_0 = (TH1F*)hMCRec_Truth_0->Clone("hEff_Truth_0"); hEff_Truth_0->Divide(hMCRec_Truth_0, hMCGen_0, 1, 1, "B");
    TH1F* hEff_Reco_0 = (TH1F*)hMCRec_Reco_0->Clone("hEff_Reco_0"); hEff_Reco_0->Divide(hMCRec_Reco_0, hMCGen_0, 1, 1, "B");

    TH1F* hDataRaw = new TH1F("hDataRaw", "Data Raw", nBinsVar, xBinsVar);
    tData->Draw(plotVar + ">>hDataRaw", recCut);

    // Apply Hybrid with factor 1.0
    TH1F* hResult_0 = ApplyHybridCorrection(hDataRaw, hEff_Truth_0, hEff_Reco_0, globalBiasFactor);
    hResult_0->SetName("hResult_0");

    // ---------------------------------------------------------
    // [ITER 1] Reweighting
    // ---------------------------------------------------------
    std::cout << "\n=== Iteration 1 ===\n";
    TH1F* hWeights = (TH1F*)hResult_0->Clone("hWeights");
    hWeights->Divide(hMCGen_0); 
    for(int i=1; i<=hWeights->GetNbinsX(); ++i) if(hMCGen_0->GetBinContent(i) == 0) hWeights->SetBinContent(i, 0); 

    TH1F* hMCGen_1 = new TH1F("hMCGen_1", "MC Gen Reweighted", nBinsVar, xBinsVar);
    TH1F* hMCRec_Truth_1 = new TH1F("hMCRec_Truth_1", "MC Rec Truth Reweighted", nBinsVar, xBinsVar);
    TH1F* hMCRec_Reco_1 = new TH1F("hMCRec_Reco_1", "MC Rec Reco Reweighted", nBinsVar, xBinsVar);

    FillWeightedHisto(tMCGen, genVar, genVar, genCut, hMCGen_1, hWeights);
    FillWeightedHisto(tMCRec, mcRecVar, mcRecVar, recCut, hMCRec_Truth_1, hWeights);
    FillWeightedHisto(tMCRec, plotVar, mcRecVar, recCut, hMCRec_Reco_1, hWeights);

    TH1F* hEff_Truth_1 = (TH1F*)hMCRec_Truth_1->Clone("hEff_Truth_1"); hEff_Truth_1->Divide(hMCRec_Truth_1, hMCGen_1, 1, 1, "B");
    TH1F* hEff_Reco_1 = (TH1F*)hMCRec_Reco_1->Clone("hEff_Reco_1"); hEff_Reco_1->Divide(hMCRec_Reco_1, hMCGen_1, 1, 1, "B");

    // Apply Hybrid with factor 1.0
    TH1F* hResult_1 = ApplyHybridCorrection(hDataRaw, hEff_Truth_1, hEff_Reco_1, globalBiasFactor);
    hResult_1->SetName("hResult_1");

    // ---------------------------------------------------------
    // [PLOTTING] 可视化每一步的结果
    // ---------------------------------------------------------
    TCanvas* c1 = new TCanvas("c1", "Step by Step Results", 2400, 1200);
    c1->Divide(2, 1);

    // --- Pad 1: IP Distribution Evolution ---
    c1->cd(1); gPad->SetLogy(); gPad->SetGrid(); gPad->SetLeftMargin(0.12);
    
    // 准备 Truth (如果存在)
    TH1F* hTruth = nullptr;
    if (realDataGenFile != "") {
        TChain* tDataGen = new TChain("pndsim"); AddToChain(tDataGen, realDataGenFile);
        if(tDataGen->GetEntries()==0) { delete tDataGen; tDataGen = new TChain("cbmsim"); AddToChain(tDataGen, realDataGenFile); }
        if(tDataGen->GetEntries() > 0) {
            hTruth = new TH1F("hTruth", "Truth", nBinsVar, xBinsVar);
            tDataGen->Draw(genVar + ">>hTruth", genCut);
        }
    }

    // 设置最大值
    double yMax = hResult_1->GetMaximum();
    if(hTruth && hTruth->GetMaximum() > yMax) yMax = hTruth->GetMaximum();
    hResult_0->SetMaximum(yMax * 20.0); // Log scale space
    hResult_0->SetMinimum(0.5);

    // 样式设置
    hResult_0->SetTitle("IP Reconstruction Evolution;z (cm);Events");
    hResult_0->SetLineColor(kAzure+1);
    hResult_0->SetLineStyle(2); // Dash
    hResult_0->SetLineWidth(3);
    
    hResult_1->SetLineColor(kOrange+1);
    hResult_1->SetLineStyle(1); // Solid
    hResult_1->SetLineWidth(4);
    hResult_1->SetMarkerStyle(20);
    hResult_1->SetMarkerColor(kOrange+1);
    hResult_1->SetMarkerSize(1.0);

    hDataRaw->SetLineColor(kGray+1);
    hDataRaw->SetLineWidth(2);
    
    // 绘制顺序
    hResult_0->Draw("HIST");       // Iter 0 (Blue Dash)
    hDataRaw->Draw("HIST SAME");   // Raw Data (Gray)
    hResult_1->Draw("E SAME");     // Iter 1 (Orange Points)
    
    if (hTruth) {
        hTruth->SetLineColor(kGreen+2);
        hTruth->SetLineWidth(3);
        hTruth->Draw("HIST SAME"); // Truth (Green)
    }

    TLegend* leg1 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg1->SetHeader("Steps (Bias Correction = OFF)");
    leg1->AddEntry(hDataRaw, "Raw Data", "l");
    leg1->AddEntry(hResult_0, "Iter 0 (Initial MC)", "l");
    leg1->AddEntry(hResult_1, "Iter 1 (Reweighted)", "lp");
    if(hTruth) leg1->AddEntry(hTruth, "Truth (Sim)", "l");
    leg1->Draw();

    // --- Pad 2: Efficiency Evolution ---
    c1->cd(2); gPad->SetGrid();
    
    hEff_Reco_0->SetTitle("Efficiency Evolution (Reco Eff);z (cm);Efficiency");
    hEff_Reco_0->SetLineColor(kAzure+1);
    hEff_Reco_0->SetLineStyle(2);
    hEff_Reco_0->SetLineWidth(3);
    hEff_Reco_0->SetMinimum(0);
    hEff_Reco_0->SetMaximum(1.1);
    
    hEff_Reco_1->SetLineColor(kOrange+1);
    hEff_Reco_1->SetLineWidth(3);
    
    hEff_Reco_0->Draw("HIST");
    hEff_Reco_1->Draw("HIST SAME");

    TLegend* leg2 = new TLegend(0.6, 0.2, 0.9, 0.4);
    leg2->AddEntry(hEff_Reco_0, "Eff Iter 0 (Initial)", "l");
    leg2->AddEntry(hEff_Reco_1, "Eff Iter 1 (Reweighted)", "l");
    leg2->Draw();

    // 绘制 Hybrid 区域线
    TLine* l1 = new TLine(-2, 0, -2, 1.1); l1->SetLineStyle(2); l1->Draw();
    TLine* l2 = new TLine(2, 0, 2, 1.1); l2->SetLineStyle(2); l2->Draw();
    TLatex* tex = new TLatex(); tex->SetTextSize(0.04);
    tex->DrawLatex(-1.5, 0.5, "Truth Eff Used");
    tex->DrawLatex(3, 0.5, "Reco Eff Used");

    c1->SaveAs(outputName + ".png");

    // 保存文件
    TFile* fOut = new TFile(outputName + ".root", "RECREATE");
    hResult_0->Write("hResult_Iter0");
    hResult_1->Write("hResult_Final");
    if(hTruth) hTruth->Write("hTruth");
    hEff_Reco_0->Write("hEff_Iter0");
    hEff_Reco_1->Write("hEff_Final");
    fOut->Close();
}
