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
 * Efficiency correction script
 * 
 * Features:
 * 1. Read real data (Data Rec)
 * 2. Read MC reconstructed data (MC Rec)
 * 3. Read MC generated data (MC Gen) - used to compute the denominator
 * 4. Compute the efficiency: Efficiency = MC Rec / MC Gen
 * 5. Correct the real data: Corrected Data = Data Rec / Efficiency
 * 
 * Usage:
 * root -l 'efficiency_correction_2.C("data_ana.root", "mc_ana.root", "mc_sim.root")'
 * root -b -q -l 'efficiency_correction_2.C("../data/test_poca/reco/test_poca_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_sim.root", "../data/test_poca/reco/test_poca_*_sim.root","10percent_fullgas")'
 * 
 * Specify file number range (New):
 * root -l 'efficiency_correction_2.C("data_%d.root,1,100", "mc_%d.root,1,500", "mc_sim_%d.root,1,500")'
 */
void efficiency_correction_2(
    TString realDataFile = "data_ana_final.root", // Real data file (after ana processing)
    TString mcRecFile = "mc_ana_final.root",      // MC reconstructed data file (after ana processing)
    TString mcGenFile = "mc_sim.root",            // MC generated data file (sim output, containing MCTrack)
    TString realDataGenFile = "",                 // Corresponding generated file for the real data (optional, for validation)
    TString outputName = "efficiency_correction_result", // Output filename prefix
    TString plotVar = "pvz",                      // Variable name to correct (in the ntpDp tree of the ana file)
    TString mcRecVar = "pvz_mc",                  // MC reconstructed variable (used as the efficiency numerator, usually the MC truth value)
    TString genVar = "MCTrack.fStartZ",           // Corresponding generated-level variable (in the pndsim tree of the sim file)
    TString recCut = "pvz != -999.0",             // Selection cut for reconstructed data
    TString genCut = "MCTrack.fMotherID==-1&&MCTrack.fPdgCode==2212", // Selection cut for generated particles
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
    // Basic setup
    // ---------------------------------------------------------
    // Force a pure white background
    //gStyle->SetCanvasColor(kWhite);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    // Remove the global legend border
    gStyle->SetLegendBorderSize(0);
    // (Optional) Make the global legend background transparent (in some ROOT versions, the global FillStyle support for Legend is unstable, so method 1 is recommended)
    gStyle->SetLegendFillColor(0);

    // ---------------------------------------------------------
    // Key: line width and marker size (optimized for 5000 px width)
    // ---------------------------------------------------------
    // The default line width is usually 1 px, which is nearly invisible on a 5000 px plot. A value of 3 to 5 is recommended.
    gStyle->SetLineWidth(6);      
    gStyle->SetFrameLineWidth(6); // Axis frame line width
    gStyle->SetHistLineWidth(6);  // Histogram line width
    gStyle->SetFuncWidth(6);      // Function/fitting curve width
    gStyle->SetGridWidth(2);      // Grid line width (if enabled)

    // Marker size (default 1.0; recommended to enlarge to 2.0–3.0)
    gStyle->SetMarkerSize(2.5);
    gStyle->SetMarkerStyle(20);   // Solid circles are recommended because they are clearest on large plots

    // ---------------------------------------------------------
    // Font settings (ROOT font sizes are percentages of pad height)
    // ---------------------------------------------------------
    // 5000x2500 is a wide 2:1 plot.
    // 0.05 means 5% of the height, i.e. 2500 × 0.05 = 125 pixels, which is very clear.
    
    // Axis tick labels
    gStyle->SetLabelSize(0.05, "XY"); 
    gStyle->SetLabelFont(42, "XY");   // Font 42 (Helvetica) is more standard than the default font 62

    // Axis titles
    gStyle->SetTitleSize(0.06, "XY"); 
    gStyle->SetTitleFont(42, "XY");

    // Top chart title
    gStyle->SetTitleSize(0.06, "t");  
    gStyle->SetTitleFont(42, "t");

    // ---------------------------------------------------------
    // Layout fine-tuning
    // ---------------------------------------------------------
    // Adjust the spacing between the title and axes. Since the canvas is very wide, the Y-axis title may be too far from the axis, so the offset should be reduced slightly
    gStyle->SetTitleOffset(0.95, "X");
    gStyle->SetTitleOffset(1.2, "Y"); // Wide canvas, Y axis title closer looks better

    // Tick length (slightly longer for a more refined look)
    gStyle->SetTickLength(0.02, "XY");
    
    // Statistics box (StatBox) – if shown, the font and position must be adjusted, otherwise it looks ugly
    gStyle->SetStatFont(42);
    gStyle->SetStatFontSize(0.04);
    gStyle->SetStatBorderSize(2); // Thicken the border
    // Set the plotting style
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(1);
    gStyle->SetPadBottomMargin(0.15); // Leave bottom padding for the X-axis title (default about 0.1)
    gStyle->SetPadLeftMargin(0.15);   // Leave left padding for the Y-axis title (default about 0.1)
    gStyle->SetPadRightMargin(0.05);  // Make the right side slightly tighter
    gStyle->SetPadTopMargin(0.08);
    // gStyle->SetTextSize(0.05);
    // gStyle->SetLabelSize(0.05, "XYZ");
    // gStyle->SetTitleSize(0.06, "XYZ");
    // gStyle->SetTitleOffset(1.4, "Y");
    // gStyle->SetPadLeftMargin(0.18);
    // gStyle->SetPadBottomMargin(0.15);

    // ---------------------------------------------------------
    // 1. Get the MC reconstructed distribution (numerator for efficiency) and compute the bias
    // ---------------------------------------------------------
    TChain* tMCRec = new TChain("ntpDp");
    AddToChain(tMCRec, mcRecFile);
    if (tMCRec->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in MC Rec file(s): " << mcRecFile << std::endl; 
        return; 
    }

    // High granularity histograms for Efficiency plot (using Variable Bins)
    
    // MC Rec (truth variable) – used for the standard efficiency calculation
    TH1F* hMCRec = new TH1F("hMCRec", "MC Rec (Truth Var);Z (cm);#Events", nBinsVar, xBinsVar);
    tMCRec->Draw(mcRecVar + ">>hMCRec", recCut);
    hMCRec->SetDirectory(0);

    // MC Rec (reco variable) – used for the reco-based efficiency calculation
    TH1F* hMCRecReco = new TH1F("hMCRecReco", "MC Rec (Reco Var);Z (cm);#Events", nBinsVar, xBinsVar);
    tMCRec->Draw(plotVar + ">>hMCRecReco", recCut);
    hMCRecReco->SetDirectory(0);

    // Compute the bias (reco - truth)
    // 1. Global bias distribution (1D)
    TH1F* hDev = new TH1F("hDev", "Global Resolution;Rec - Truth (cm);#Events", 100, devMin, devMax);
    tMCRec->Draw(Form("%s - %s >> hDev", plotVar.Data(), mcRecVar.Data()), recCut);
    hDev->SetDirectory(0);
    double globalBias = hDev->GetMean();

    // 2. Variation of the bias with Z (profile) – used for dynamic correction
    // Use nBinsVar for profile to match efficiency granularity
    TProfile* pBias = new TProfile("pBias", "Bias vs Z (Profile);Z_{reco} (cm);<Bias> (cm)", nBinsVar, xBinsVar);
    // Draw y:x >> profile
    tMCRec->Draw(Form("%s - %s : %s >> pBias", plotVar.Data(), mcRecVar.Data(), plotVar.Data()), recCut, "prof");
    pBias->SetDirectory(0);

    // 3. Fit the bias curve (using the linear function pol1)
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
    // 2. Get the real data distribution (numerator for data)
    // ---------------------------------------------------------
    TChain* tData = new TChain("ntpDp");
    AddToChain(tData, realDataFile);
    if (tData->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in Data file(s): " << realDataFile << std::endl; 
        return; 
    }

    // Data (raw) – keep the original data and do not directly correct the bias
    TH1F* hDataRaw = new TH1F("hDataRaw", "Real Data (Raw);Z (cm);#Events", nBinsVar, xBinsVar);
    tData->Draw(plotVar + ">>hDataRaw", recCut);
    hDataRaw->SetDirectory(0);

    // ---------------------------------------------------------
    // 3. Get the MC generated distribution (denominator for efficiency)
    // --------------------------------------------------------- 

    // ---------------------------------------------------------
    // 3. Get the MC generated distribution (denominator for efficiency)
    // ---------------------------------------------------------
    // Note: this is the key step for computing the absolute efficiency. The sim file must contain all generated particles.
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
    // Note: this assumes that genVar is directly accessible. If it is a TClonesArray, a more complex Draw syntax may be required.
    // For example, "MCTrack.fP" or "MCTrack[0].fP"
    tMCGen->Draw(genVar + ">>hMCGen", genCut); 
    hMCGen->SetDirectory(0);

    // ---------------------------------------------------------
    // 3.5 Get the generated distribution of the real data (optional truth for verification)
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
    // 4. Compute the efficiency
    // ---------------------------------------------------------
    // 4.1 Standard efficiency (truth-variable based)
    TH1F* hEff = (TH1F*)hMCRec->Clone("hEff");
    hEff->SetTitle("Efficiency (Truth Var);Z (cm);Efficiency");
    hEff->Divide(hMCRec, hMCGen, 1, 1, "B"); 

    // 4.2 Reco efficiency (reco-variable based)
    TH1F* hEffReco = (TH1F*)hMCRecReco->Clone("hEffReco");
    hEffReco->SetTitle("Efficiency (Reco Var);Z (cm);Efficiency");
    hEffReco->Divide(hMCRecReco, hMCGen, 1, 1, "B");

    // 4.3 Bias-corrected efficiency (shifted lookup)
    // Construct an efficiency curve that accounts for the bias: Eff_corr(z_rec) = Eff_std(z_rec - Bias(z_rec))
    // In this way, when correcting Data(z_rec), the efficiency at the corresponding true position z_true is used
    /*
    TH1F* hEffBiasCorr = (TH1F*)hEff->Clone("hEffBiasCorr");
    hEffBiasCorr->SetTitle("Efficiency (Bias Corrected);Z_{reco} (cm);Efficiency");
    hEffBiasCorr->Reset();
    
    for (int i = 1; i <= hEffBiasCorr->GetNbinsX(); ++i) {
        double z_rec = hEffBiasCorr->GetBinCenter(i);
        // Compute the corresponding true position: z_true = z_rec - Bias(z_rec)
        double bias_val = p0 + p1 * z_rec;
        double z_true = z_rec - bias_val;
        
        // Find the value of z_true in the standard efficiency curve (hEff)
        int bin_true = hEff->FindBin(z_true);
        double eff_val = 0.0;
        double eff_err = 0.0;
        
        // Ensure it is within range
        if (bin_true >= 1 && bin_true <= hEff->GetNbinsX()) {
            eff_val = hEff->GetBinContent(bin_true);
            eff_err = hEff->GetBinError(bin_true);
        }
        
        hEffBiasCorr->SetBinContent(i, eff_val);
        hEffBiasCorr->SetBinError(i, eff_err);
    }
    */

    // ---------------------------------------------------------
    // 5. Correct the real data
    // ---------------------------------------------------------
    // 5.1 Standard correction (new): raw data / bias-corrected efficiency
    // This preserves the reco coordinate of the data but normalizes it with the correct (shifted) efficiency value
    /*
    TH1F* hCorrected = (TH1F*)hDataRaw->Clone("hCorrected");
    hCorrected->SetTitle("Corrected (Raw / BiasCorrEff);Variable;#Events");
    hCorrected->Divide(hEffBiasCorr);
    */

    // 5.2 No-bias correction: raw data / standard truth efficiency (ignore the bias)
    TH1F* hCorrectedNoBias = (TH1F*)hDataRaw->Clone("hCorrectedNoBias");
    hCorrectedNoBias->SetTitle("Corrected (Raw / TruthEff);Z (cm);#Events");
    hCorrectedNoBias->Divide(hEff);

    // 5.3 Reco-efficiency correction: raw data / reco efficiency (automatically includes bias and smearing)
    TH1F* hCorrectedRecoEff = (TH1F*)hDataRaw->Clone("hCorrectedRecoEff");
    hCorrectedRecoEff->SetTitle("Corrected (Raw / RecoEff);Z (cm);#Events");
    hCorrectedRecoEff->Divide(hEffReco);

    // 5.4 Hybrid correction: (-2, 2) uses truth efficiency (pvz_mc), others use reco efficiency (pvz)
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
    // 6. Plotting and saving
    // ---------------------------------------------------------
    // Update: keep only the original Pad 6 (hybrid correction) and Pad 8 (relative difference)
    // Use a left-right layout (left: correction result, right: relative difference)
    TCanvas* c1 = new TCanvas("c1", "Efficiency Correction Analysis", 6000, 3000);
    // c1->Divide(2, 1); 

    // ------------------------------------------
    // New Pad 1 (Left): Hybrid Correction vs Truth
    // ------------------------------------------
    // ---------------------------------------------------------
    // ---------------------------------------------------------
    // 6. Draw (c1) - Physical Axis (Visual Width = Physical Width)
    // ---------------------------------------------------------
    c1->cd(1);
    gPad->SetLeftMargin(0.08);
    gPad->SetRightMargin(0.04);
    gPad->SetBottomMargin(0.12); 
    gPad->SetTopMargin(0.08); 
    gPad->SetTicks(1, 1);
    gPad->SetLogy();

    // Unify the vertical-axis range
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
        line->SetLineWidth(4); // Thicken
        line->Draw();
    }
    */

    c1->SaveAs(outputName + ".png");

    // ---------------------------------------------------------
    // 7. Draw an efficiency comparison on a new canvas (truth vs reco) - Physical Axis
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
    TLegend* legEff = new TLegend(0.7, 0.75, 0.9, 0.9); // Put in bottom right corner
    legEff->SetFillStyle(0);
    legEff->SetBorderSize(0);
    legEff->SetTextFont(132);
    legEff->SetTextSize(0.045); 
    legEff->AddEntry(hEff, "Uniform Input", "lp");
    legEff->AddEntry(hEffReco, "Realistic Input", "lp");
    legEff->Draw();

    c2->SaveAs(outputName + "_eff.png");

    // ---------------------------------------------------------
    // 8. Print statistics
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

