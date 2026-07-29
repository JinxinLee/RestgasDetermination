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
 * Efficiency correction script
 * 
 * Function:
 * 1. Read real data (Data Rec)
 * 2. Read MC reco data (MC Rec)
 * 3. Read MC gen data (MC Gen) - for denominator calculation
 * 4. Calculate efficiency: Efficiency = MC Rec / MC Gen
 * 5. Correct real data: Corrected Data = Data Rec / Efficiency
 * 
 * Usage:
 * root -l 'efficiency_correction.C("data_ana.root", "mc_ana.root", "mc_sim.root")'
 * root -b -q -l 'efficiency_correction.C("../data/test_poca/reco/test_poca_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_sim.root", "../data/test_poca/reco/test_poca_*_sim.root","10percent_fullgas")'
 */
void efficiency_correction(
    TString realDataFile = "data_ana_final.root", // Real data file (after ana)
    TString mcRecFile = "mc_ana_final.root",      // MC reco data file (after ana)
    TString mcGenFile = "mc_sim.root",            // MC gen data file (sim output, contains MCTrack)
    TString realDataGenFile = "",                 // Gen file for real data (optional, for verification)
    TString outputName = "efficiency_correction_result", // Output file name prefix
    TString plotVar = "pvz",                      // Variable to correct (in ntpDp tree of ana file)
    TString mcRecVar = "pvz_mc",                  // MC reco variable (for efficiency numerator, usually MC truth)
    TString genVar = "MCTrack.fStartZ",           // Corresponding gen variable (in pndsim tree of sim file)
    TString recCut = "pvz != -999.0",             // Reco data selection condition
    TString genCut = "MCTrack.fMotherID==-1&&MCTrack.fPdgCode==2212", // Gen particle selection condition
    int nBins = 200,
    double xMin = -30.0,
    double xMax = 30.0,
    double devMin = -1.0,
    double devMax = 1.0
)
{
    // ---------------------------------------------------------
    // Basic settings
    // ---------------------------------------------------------
    // Force pure white background
    //gStyle->SetCanvasColor(kWhite);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    // Set global legend borderless
    gStyle->SetLegendBorderSize(0);
    // (Optional) Set global legend background transparent (Some ROOT versions are unstable, manual transparency is recommended)
    gStyle->SetLegendFillColor(0);

    // ---------------------------------------------------------
    // Key: Line width and point size (optimized for 5000px width)
    // ---------------------------------------------------------
    // Default line width is usually 1px, almost invisible on a 5000px plot. Recommend 3 to 5.
    gStyle->SetLineWidth(4);      
    gStyle->SetFrameLineWidth(4); // Axis frame line width
    gStyle->SetHistLineWidth(4);  // Histogram line width
    gStyle->SetFuncWidth(4);      // Function/fitting curve width
    gStyle->SetGridWidth(2);      // Grid line width (if enabled)

    // Marker size, default 1.0, recommend enlarge to 2.0 - 3.0
    gStyle->SetMarkerSize(2.5);
    gStyle->SetMarkerStyle(20);   // Recommend solid circle, clearest on large plot

    // ---------------------------------------------------------
    // Font settings (ROOT font size is percentage of Pad height)
    // ---------------------------------------------------------
    // 5000x2500 is 2:1 wide plot.
    // 0.05 means 5% of height, i.e., 2500 * 0.05 = 125 pixels high (very clear).
    
    // Axis scale numerical labels
    gStyle->SetLabelSize(0.05, "XY"); 
    gStyle->SetLabelFont(42, "XY");   // Font 42 (Helvetica) is more standard than default 62

    // Axis title
    gStyle->SetTitleSize(0.06, "XY"); 
    gStyle->SetTitleFont(42, "XY");

    // Top chart title
    gStyle->SetTitleSize(0.06, "t");  
    gStyle->SetTitleFont(42, "t");

    // ---------------------------------------------------------
    // Layout fine-tuning
    // ---------------------------------------------------------
    // Adjust distance between title and axis. Decrease Offset since canvas is very wide
    gStyle->SetTitleOffset(0.95, "X");
    gStyle->SetTitleOffset(1.2, "Y"); // Wide canvas, Y axis title closer looks better

    // Tick length (slightly longer for better texture)
    gStyle->SetTickLength(0.02, "XY");
    
    // StatBox - Adjust font and position if display is needed
    gStyle->SetStatFont(42);
    gStyle->SetStatFontSize(0.04);
    gStyle->SetStatBorderSize(2); // Thicken border
    // Set drawing style
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
    gStyle->SetPadBottomMargin(0.15); // Bottom margin for X axis title (default approx 0.1)
    gStyle->SetPadLeftMargin(0.15);   // Left margin for Y axis title (default approx 0.1)
    gStyle->SetPadRightMargin(0.05);  // Right side slightly more compact
    gStyle->SetPadTopMargin(0.08);
    // gStyle->SetTextSize(0.05);
    // gStyle->SetLabelSize(0.05, "XYZ");
    // gStyle->SetTitleSize(0.06, "XYZ");
    // gStyle->SetTitleOffset(1.4, "Y");
    // gStyle->SetPadLeftMargin(0.18);
    // gStyle->SetPadBottomMargin(0.15);

    // ---------------------------------------------------------
    // 1. Get MC reco distribution (Numerator for Efficiency) & calculate bias
    // ---------------------------------------------------------
    TChain* tMCRec = new TChain("ntpDp");
    tMCRec->Add(mcRecFile);
    if (tMCRec->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in MC Rec file(s): " << mcRecFile << std::endl; 
        return; 
    }

    // MC Rec (Truth Variable) - for standard efficiency calculation
    TH1F* hMCRec = new TH1F("hMCRec", "MC Reconstructed (Truth Var);Z (cm);#Events", nBins, xMin, xMax);
    tMCRec->Draw(mcRecVar + ">>hMCRec", recCut);
    hMCRec->SetDirectory(0);

    // MC Rec (Reco Variable) - for Reco-based efficiency calculation
    TH1F* hMCRecReco = new TH1F("hMCRecReco", "MC Reconstructed (Reco Var);Z (cm);#Events", nBins, xMin, xMax);
    tMCRec->Draw(plotVar + ">>hMCRecReco", recCut);
    hMCRecReco->SetDirectory(0);

    // Calculate bias (Reco - Truth)
    // 1. Global bias distribution (1D)
    TH1F* hDev = new TH1F("hDev", "Global Resolution;Rec - Truth (cm);#Events", 100, devMin, devMax);
    tMCRec->Draw(Form("%s - %s >> hDev", plotVar.Data(), mcRecVar.Data()), recCut);
    hDev->SetDirectory(0);
    double globalBias = hDev->GetMean();

    // 2. Bias variation with Z (Profile) - for dynamic correction
    TProfile* pBias = new TProfile("pBias", "Bias vs Z (Profile);Z_{reco} (cm);<Bias> (cm)", nBins, xMin, xMax);
    // Draw y:x >> profile
    tMCRec->Draw(Form("%s - %s : %s >> pBias", plotVar.Data(), mcRecVar.Data(), plotVar.Data()), recCut, "prof");
    pBias->SetDirectory(0);

    // 3. Fit bias curve (using linear function pol1)
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
    // 2. Get real data distribution (Numerator for Data)
    // ---------------------------------------------------------
    TChain* tData = new TChain("ntpDp");
    tData->Add(realDataFile);
    if (tData->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in Data file(s): " << realDataFile << std::endl; 
        return; 
    }

    // Data (Raw) - Keep raw data, no direct bias correction
    TH1F* hDataRaw = new TH1F("hDataRaw", "Real Data (Raw);Z (cm);#Events", nBins, xMin, xMax);
    tData->Draw(plotVar + ">>hDataRaw", recCut);
    hDataRaw->SetDirectory(0);

    // ---------------------------------------------------------
    // 3. Get MC generation distribution (Denominator for Efficiency)
    // --------------------------------------------------------- 

    // ---------------------------------------------------------
    // 3. Get MC generation distribution (Denominator for Efficiency)
    // ---------------------------------------------------------
    // Note: Key to absolute efficiency. Must use sim file with all generated particles.
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
    // Note: Assume genVar accessible. If TClonesArray, might need complex Draw syntax
    // e.g. "MCTrack.fP" or "MCTrack[0].fP"
    tMCGen->Draw(genVar + ">>hMCGen", genCut); 
    hMCGen->SetDirectory(0);

    // ---------------------------------------------------------
    // 3.5 Get real data generation distribution (Optional Truth for Verification)
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
    // 4. Calculate efficiency (Efficiency)
    // ---------------------------------------------------------
    // 4.1 Standard efficiency (Truth Variable based)
    TH1F* hEff = (TH1F*)hMCRec->Clone("hEff");
    hEff->SetTitle("Efficiency (Truth Var);Z (cm);Efficiency");
    hEff->Divide(hMCRec, hMCGen, 1, 1, "B"); 

    // 4.2 Reco efficiency (Reco Variable based)
    TH1F* hEffReco = (TH1F*)hMCRecReco->Clone("hEffReco");
    hEffReco->SetTitle("Efficiency (Reco Var);Z (cm);Efficiency");
    hEffReco->Divide(hMCRecReco, hMCGen, 1, 1, "B");

    // 4.3 Bias-Corrected Efficiency (Shifted Lookup)
    // Construct efficiency curve considering bias: Eff_corr(z_rec) = Eff_std(z_rec - Bias(z_rec))
    // When correcting Data(z_rec), use efficiency at corresponding true position z_true
    /*
    TH1F* hEffBiasCorr = (TH1F*)hEff->Clone("hEffBiasCorr");
    hEffBiasCorr->SetTitle("Efficiency (Bias Corrected);Z_{reco} (cm);Efficiency");
    hEffBiasCorr->Reset();
    
    for (int i = 1; i <= hEffBiasCorr->GetNbinsX(); ++i) {
        double z_rec = hEffBiasCorr->GetBinCenter(i);
        // Calculate corresponding true position: z_true = z_rec - Bias(z_rec)
        double bias_val = p0 + p1 * z_rec;
        double z_true = z_rec - bias_val;
        
        // Find z_true value in standard efficiency curve (hEff)
        int bin_true = hEff->FindBin(z_true);
        double eff_val = 0.0;
        double eff_err = 0.0;
        
        // Ensure within range
        if (bin_true >= 1 && bin_true <= hEff->GetNbinsX()) {
            eff_val = hEff->GetBinContent(bin_true);
            eff_err = hEff->GetBinError(bin_true);
        }
        
        hEffBiasCorr->SetBinContent(i, eff_val);
        hEffBiasCorr->SetBinError(i, eff_err);
    }
    */

    // ---------------------------------------------------------
    // 5. Correct real data (Correction)
    // ---------------------------------------------------------
    // 5.1 Standard correction (New): Raw Data / Bias-Corrected Eff
    // This preserves Reco coordinates of Data, but uses correct (shifted) efficiency for normalization
    /*
    TH1F* hCorrected = (TH1F*)hDataRaw->Clone("hCorrected");
    hCorrected->SetTitle("Corrected (Raw / BiasCorrEff);Variable;#Events");
    hCorrected->Divide(hEffBiasCorr);
    */

    // 5.2 No Bias correction: Raw Data / Standard Truth Eff (ignore Bias)
    TH1F* hCorrectedNoBias = (TH1F*)hDataRaw->Clone("hCorrectedNoBias");
    hCorrectedNoBias->SetTitle("Corrected (Raw / TruthEff);Z (cm);#Events");
    hCorrectedNoBias->Divide(hEff);

    // 5.3 Reco efficiency correction: Raw Data / Reco Eff (auto-includes Bias and Smearing)
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

        if (center >= -1.50 && center <= 1.50) {
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
    // 6. Draw and save
    // ---------------------------------------------------------
    TCanvas* c1 = new TCanvas("c1", "Efficiency Correction Analysis", 5000, 5000);
    c1->Divide(3, 3); // 3 columns, 3 rows

    // Pad 1: Raw data vs MC reco (Reco Var) - shape comparison
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

    // Pad 2: MC Gen vs MC Reco (Truth Var)
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

    // Pad 3: Bias histogram (Global Resolution)
    c1->cd(3);
    hDev->SetLineColor(kBlue);
    //hDev->SetLineWidth(3);
    hDev->Draw();
    
    TLegend* leg3 = new TLegend(0.6, 0.78, 0.9, 0.9);
    leg3->SetFillStyle(0);
    leg3->SetBorderSize(0);
    leg3->AddEntry(hDev, "Bias Distribution", "l");
    leg3->Draw();

    // Pad 4: Efficiency curves comparison
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

    // Unify Y-axis range (Truth as standard, ignore corrected abnormal highs)
    double yMax = 0;
    if (hDataGen) {
        yMax = hDataGen->GetMaximum() * 1.2;
    } else {
        // If no Truth, use max value of corrected data
        if (hCorrectedNoBias->GetMaximum() > yMax) yMax = hCorrectedNoBias->GetMaximum();
        if (hCorrectedRecoEff->GetMaximum() > yMax) yMax = hCorrectedRecoEff->GetMaximum();
        if (hCorrectedHybrid->GetMaximum() > yMax) yMax = hCorrectedHybrid->GetMaximum();
        yMax *= 1.2;
    }

    // Pad 5: Reco Efficiency Correction (Raw / RecoEff) vs Truth
    c1->cd(5);
    hCorrectedRecoEff->SetLineColor(kAzure+7);
    hCorrectedRecoEff->SetLineWidth(3);
    hCorrectedRecoEff->SetMarkerColor(kAzure+7);
    hCorrectedRecoEff->SetMarkerStyle(20);
    hCorrectedRecoEff->SetMinimum(0.0);
    hCorrectedRecoEff->SetMaximum(yMax);
    hCorrectedRecoEff->Draw("E");

    if (hDataGen) {
        hDataGen->Draw("HIST SAME");
        TLegend* leg5 = new TLegend(0.6, 0.78, 0.9, 0.9);
        leg5->SetFillStyle(0);
        leg5->SetBorderSize(0);
        leg5->AddEntry(hCorrectedRecoEff, "Corr (RecoEff)", "lp");
        leg5->AddEntry(hDataGen, "Truth", "l");
        leg5->Draw();
    }

    // Pad 6: Hybrid Correction (Raw / HybridEff) vs Truth
    c1->cd(6);
    hCorrectedHybrid->SetLineColor(kOrange+1);
    hCorrectedHybrid->SetLineWidth(3);
    hCorrectedHybrid->SetMarkerColor(kOrange+1);
    hCorrectedHybrid->SetMarkerStyle(21);
    hCorrectedHybrid->SetMinimum(0.0);
    hCorrectedHybrid->SetMaximum(yMax);
    hCorrectedHybrid->Draw("E");

    if (hDataGen) {
        hDataGen->Draw("HIST SAME");
        TLegend* leg6 = new TLegend(0.6, 0.78, 0.9, 0.9);
        leg6->SetFillStyle(0);
        leg6->SetBorderSize(0);
        leg6->AddEntry(hCorrectedHybrid, "Corr (Hybrid)", "lp");
        leg6->AddEntry(hDataGen, "Truth", "l");
        leg6->Draw();
    }

    // Pad 7: Relative Difference ((RecoEff Corrected - Truth) / Truth)
    c1->cd(7);
    if (hDataGen) {
        TH1F* hRelDiffReco = (TH1F*)hCorrectedRecoEff->Clone("hRelDiffReco");
        hRelDiffReco->SetTitle("Rel. Diff ((RecoEff - Truth) / Truth);Z (cm);Rel. Diff");
        hRelDiffReco->Add(hDataGen, -1.0); // RecoEff - Truth
        hRelDiffReco->Divide(hDataGen);    // (RecoEff - Truth) / Truth
        
        hRelDiffReco->SetLineColor(kAzure+7);
        hRelDiffReco->SetLineWidth(2);
        hRelDiffReco->SetMarkerStyle(20);
        hRelDiffReco->SetMarkerColor(kAzure+7);
        hRelDiffReco->SetMinimum(-1.0);
        hRelDiffReco->SetMaximum(1.0);
        hRelDiffReco->Draw("E");
        
        TLine *line = new TLine(xMin, 0.0, xMax, 0.0);
        line->SetLineStyle(2);
        line->SetLineColor(kBlack);
        line->Draw();
        
        TLegend* leg7 = new TLegend(0.6, 0.78, 0.9, 0.9);
        leg7->SetFillStyle(0);
        leg7->SetBorderSize(0);
        leg7->AddEntry(hRelDiffReco, "(Reco - Truth)/Truth", "lp");
        leg7->Draw();
    }

    // Pad 8: Relative Difference ((Hybrid Corrected - Truth) / Truth)
    c1->cd(8);
    if (hDataGen) {
        TH1F* hRelDiffHybrid = (TH1F*)hCorrectedHybrid->Clone("hRelDiffHybrid");
        hRelDiffHybrid->SetTitle("Rel. Diff ((Hybrid - Truth) / Truth);Z (cm);Rel. Diff");
        hRelDiffHybrid->Add(hDataGen, -1.0); // Hybrid - Truth
        hRelDiffHybrid->Divide(hDataGen);    // (Hybrid - Truth) / Truth
        
        hRelDiffHybrid->SetLineColor(kOrange+1);
        hRelDiffHybrid->SetLineWidth(2);
        hRelDiffHybrid->SetMarkerStyle(21);
        hRelDiffHybrid->SetMarkerColor(kOrange+1);
        hRelDiffHybrid->SetMinimum(-1.0);
        hRelDiffHybrid->SetMaximum(1.0);
        hRelDiffHybrid->Draw("E");
        
        TLine *line = new TLine(xMin, 0.0, xMax, 0.0);
        line->SetLineStyle(2);
        line->SetLineColor(kBlack);
        line->Draw();
        
        TLegend* leg8 = new TLegend(0.6, 0.78, 0.9, 0.9);
        leg8->SetFillStyle(0);
        leg8->SetBorderSize(0);
        leg8->AddEntry(hRelDiffHybrid, "(Hybrid - Truth)/Truth", "lp");
        leg8->Draw();
    }

    // Pad 9: Statistical info output
    c1->cd(9);
    TPaveText *pt = new TPaveText(0.1, 0.3, 0.9, 0.7, "NDC");
    pt->SetFillColor(kWhite);
    pt->SetBorderSize(1);
    pt->SetTextAlign(12); // Left-Center
    pt->SetTextSize(0.05);
    
    int binStatsMin = hDataRaw->FindBin(-30.0);
    int binStatsMax = hDataRaw->FindBin(30.0);

    pt->AddText("Event Statistics (-30 < Z < 30 cm):");
    pt->AddText("--------------------------------");
    
    double errRaw, errGen, errHybrid, errRecoEff;
    double nRaw = hDataRaw->IntegralAndError(binStatsMin, binStatsMax, errRaw);
    pt->AddText(Form("N_{RealData} (Raw): %.1f #pm %.1f", nRaw, errRaw));
    
    double nGen = 0;
    if (hDataGen) {
        nGen = hDataGen->IntegralAndError(binStatsMin, binStatsMax, errGen);
        pt->AddText(Form("N_{Truth} (Gen): %.1f #pm %.1f", nGen, errGen));
    } else {
        pt->AddText("N_{Truth}: N/A");
    }
    
    pt->AddText("--------------------------------");
    double nRecoEff = hCorrectedRecoEff->IntegralAndError(binStatsMin, binStatsMax, errRecoEff);
    pt->AddText(Form("N_{Corr} (RecoEff): %.1f #pm %.1f", nRecoEff, errRecoEff));
    if (nGen > 0) {
        double ratioReco = nRecoEff / nGen;
        double relDevReco = (ratioReco - 1.0) * 100.0;
        double errRelDevReco = 100.0 * (1.0/nGen) * sqrt(errRecoEff*errRecoEff + ratioReco*ratioReco*errGen*errGen);
        pt->AddText(Form("  Rel. Dev: %.2f #pm %.2f %%", relDevReco, errRelDevReco));
    }

    double nHybrid = hCorrectedHybrid->IntegralAndError(binStatsMin, binStatsMax, errHybrid);
    pt->AddText(Form("N_{Corr} (Hybrid): %.1f #pm %.1f", nHybrid, errHybrid));
    if (nGen > 0) {
        double ratioHybrid = nHybrid / nGen;
        double relDevHybrid = (ratioHybrid - 1.0) * 100.0;
        double errRelDevHybrid = 100.0 * (1.0/nGen) * sqrt(errHybrid*errHybrid + ratioHybrid*ratioHybrid*errGen*errGen);
        pt->AddText(Form("  Rel. Dev: %.2f #pm %.2f %%", relDevHybrid, errRelDevHybrid));
    }
    
    pt->Draw();

    gPad->RedrawAxis();

    // Save result
    c1->SaveAs(outputName + ".png");
    
    TFile* fOut = new TFile(outputName + ".root", "RECREATE");
    hDataRaw->Write("hDataRaw");
    hMCRec->Write();
    hMCRecReco->Write();
    hMCGen->Write();
    hEff->Write();
    hEffReco->Write();
    // hEffBiasCorr->Write(); // Save efficiency after bias correction
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
