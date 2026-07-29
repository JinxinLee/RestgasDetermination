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
 * Features:
 * 1. Read real data (Data Rec)
 * 2. Read MC reconstructed data (MC Rec)
 * 3. Read MC generated data (MC Gen) - used to compute the denominator
 * 4. Compute the efficiency: Efficiency = MC Rec / MC Gen
 * 5. Correct the real data: Corrected Data = Data Rec / Efficiency
 * 
 * Usage:
 * root -l 'efficiency_correction.C("data_ana.root", "mc_ana.root", "mc_sim.root")'
 * root -b -q -l 'efficiency_correction.C("../data/test_poca/reco/test_poca_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_ana_final.root", "../data/acc_fullgas/reco/acc_fullgas_*_sim.root", "../data/test_poca/reco/test_poca_*_sim.root","10percent_fullgas")'
 */
void efficiency_correction_1(
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
    double xMin = -600.0,
    double xMax = 1100.0,
    double devMin = -1.0,
    double devMax = 1.0
)
{
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
    gStyle->SetLineWidth(4);      
    gStyle->SetFrameLineWidth(4); // Axis frame line width
    gStyle->SetHistLineWidth(4);  // Histogram line width
    gStyle->SetFuncWidth(4);      // Function/fitting curve width
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
    tMCRec->Add(mcRecFile);
    if (tMCRec->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in MC Rec file(s): " << mcRecFile << std::endl; 
        return; 
    }

    // High granularity histograms for Efficiency plot (1000 bins)
    int nBinsEff = 300;

    // MC Rec (truth variable) – used for the standard efficiency calculation
    // Standard bins (kept if needed for consistency checks, though mostly using high res for Eff now)
    TH1F* hMCRec = new TH1F("hMCRec", "MC Rec (Truth Var);Z (cm);#Events", nBinsEff, xMin, xMax);
    tMCRec->Draw(mcRecVar + ">>hMCRec", recCut);
    hMCRec->SetDirectory(0);

    // MC Rec (reco variable) – used for the reco-based efficiency calculation
    TH1F* hMCRecReco = new TH1F("hMCRecReco", "MC Rec (Reco Var);Z (cm);#Events", nBinsEff, xMin, xMax);
    tMCRec->Draw(plotVar + ">>hMCRecReco", recCut);
    hMCRecReco->SetDirectory(0);

    // Compute the bias (reco - truth)
    // 1. Global bias distribution (1D)
    TH1F* hDev = new TH1F("hDev", "Global Resolution;Rec - Truth (cm);#Events", 100, devMin, devMax);
    tMCRec->Draw(Form("%s - %s >> hDev", plotVar.Data(), mcRecVar.Data()), recCut);
    hDev->SetDirectory(0);
    double globalBias = hDev->GetMean();

    // 2. Variation of the bias with Z (profile) – used for dynamic correction
    // Use nBinsEff for profile to match efficiency granularity
    TProfile* pBias = new TProfile("pBias", "Bias vs Z (Profile);Z_{reco} (cm);<Bias> (cm)", nBinsEff, xMin, xMax);
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
    tData->Add(realDataFile);
    if (tData->GetEntries() == 0) { 
        std::cout << "Error: Cannot find tree 'ntpDp' in Data file(s): " << realDataFile << std::endl; 
        return; 
    }

    // Data (raw) – keep the original data and do not directly correct the bias
    TH1F* hDataRaw = new TH1F("hDataRaw", "Real Data (Raw);Z (cm);#Events", nBins, xMin, xMax);
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

    // Use nBinsEff (1000) for Efficiency calculation
    TH1F* hMCGen = new TH1F("hMCGen", "MC Generated;Z (cm);#Events", nBinsEff, xMin, xMax);
    
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
        tDataGen->Add(realDataGenFile);
        if (tDataGen->GetEntries() == 0) {
            delete tDataGen;
            tDataGen = new TChain("cbmsim");
            tDataGen->Add(realDataGenFile);
        }

        if (tDataGen->GetEntries() > 0) {
            // Keep DataGen at standard nBins (200) for comparison with DataRaw
            hDataGen = new TH1F("hDataGen", "Real Data Generated (Truth);Z (cm);#Events", nBins, xMin, xMax);
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
    // 6. Plotting and saving
    // ---------------------------------------------------------
    // Update: keep only the original Pad 6 (hybrid correction) and Pad 8 (relative difference)
    // Use a left-right layout (left: correction result, right: relative difference)
    TCanvas* c1 = new TCanvas("c1", "Efficiency Correction Analysis", 6000, 3000);
    c1->Divide(2, 1); 

    // ------------------------------------------
    // New Pad 1 (Left): Hybrid Correction vs Truth
    // ------------------------------------------
    c1->cd(1);
    gPad->SetLeftMargin(0.12);
    gPad->SetRightMargin(0.04);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.08); 
    gPad->SetTicks(1, 1);
    gPad->SetLogy();

    // Unify the vertical-axis range
    double yMax = 0;
    if (hDataGen) {
        yMax = hDataGen->GetMaximum() * 5.0; // Increased for Log scale
    } else {
        yMax = hDataRaw->GetMaximum() * 5.0; // fallback
        if (hCorrectedHybrid && hCorrectedHybrid->GetMaximum() > yMax) yMax = hCorrectedHybrid->GetMaximum() * 5.0;
    }

    // Draw the frame
    if (hCorrectedHybrid) {
        hCorrectedHybrid->SetTitle(""); // Remove the title
        hCorrectedHybrid->SetLineColor(kOrange+1);
        hCorrectedHybrid->SetLineWidth(5);     // Thicken the line
        hCorrectedHybrid->SetMarkerColor(kOrange+1);
        hCorrectedHybrid->SetMarkerStyle(21);
        hCorrectedHybrid->SetMarkerSize(2.5);  // Increase marker size
        hCorrectedHybrid->SetMinimum(0.5);     // Log scale minimum
        hCorrectedHybrid->SetMaximum(yMax);
        
        // Restore axis labels – reduce the font size
        hCorrectedHybrid->GetXaxis()->SetLabelSize(0.035); 
        hCorrectedHybrid->GetXaxis()->SetTitle("#font[132]{#it{z}} (cm)"); // change title
        hCorrectedHybrid->GetXaxis()->SetTitleSize(0.045);
        hCorrectedHybrid->GetXaxis()->SetTitleOffset(1.1);
        hCorrectedHybrid->GetXaxis()->CenterTitle(); // Center
        
        hCorrectedHybrid->GetYaxis()->SetLabelSize(0.035);
        hCorrectedHybrid->GetYaxis()->SetTitle("#font[132]{#Events}"); // Switch to a mathematical font
        hCorrectedHybrid->GetYaxis()->SetTitleSize(0.045);
        hCorrectedHybrid->GetYaxis()->SetTitleOffset(1.3);
        hCorrectedHybrid->GetYaxis()->CenterTitle(); // Center
        
        hCorrectedHybrid->Draw("E");
    }

    TLegend* leg6 = new TLegend(0.6, 0.75, 0.9, 0.9);
    leg6->SetFillStyle(0);
    leg6->SetBorderSize(0);
    leg6->SetTextSize(0.035); 
    if (hCorrectedHybrid) leg6->AddEntry(hCorrectedHybrid, "Corrected", "lp");

    if (hDataGen) {
        hDataGen->SetLineColor(kAzure+2);
        hDataGen->SetLineWidth(5); // Thicken the line
        hDataGen->Draw("HIST SAME");
        leg6->AddEntry(hDataGen, "Truth", "l");

        // Compute Delta N / N
        double errHybrid, errGen;
        double nHybrid = hCorrectedHybrid->IntegralAndError(1, hCorrectedHybrid->GetNbinsX(), errHybrid);
        double nGen = hDataGen->IntegralAndError(1, hDataGen->GetNbinsX(), errGen);
        
        if (nGen > 0) {
            double ratioHybrid = nHybrid / nGen;
            double relDevHybrid = (ratioHybrid - 1.0) * 100.0;
            // Compute the relative error: dR = (N_h/N_g) * sqrt( (dN_h/N_h)^2 + (dN_g/N_g)^2 )
            double errRelDevHybrid = 100.0 * ratioHybrid * sqrt(pow(errHybrid/nHybrid, 2) + pow(errGen/nGen, 2));
            
            TLatex* latex = new TLatex();
            latex->SetNDC();
            latex->SetTextSize(0.04); 
            latex->SetTextColor(kBlack);
            latex->SetTextAlign(12);
            // Add text to the plot (with errors)
            latex->DrawLatex(0.18, 0.85, Form("#font[132]{#Delta#it{N}/#it{N} = %.2f #pm %.2f %%}", relDevHybrid, errRelDevHybrid));
        }
    }
    leg6->Draw();

    // ------------------------------------------
    // New Pad 2 (Right): Relative Difference
    // ------------------------------------------
    c1->cd(2);
    gPad->SetLeftMargin(0.18); // Increased to avoid overlap
    gPad->SetRightMargin(0.04);
    gPad->SetBottomMargin(0.12);
    gPad->SetTopMargin(0.08); 
    gPad->SetTicks(1, 1);
    gPad->SetGridy();

    if (hDataGen && hCorrectedHybrid) {
        TH1F* hRelDiffHybrid = (TH1F*)hCorrectedHybrid->Clone("hRelDiffHybrid");
        hRelDiffHybrid->SetTitle(""); 
        hRelDiffHybrid->Add(hDataGen, -1.0); // Hybrid - Truth
        //hRelDiffHybrid->Divide(hDataGen);    // (Hybrid - Truth) / Truth
        
        hRelDiffHybrid->SetLineColor(kBlack);
        hRelDiffHybrid->SetMarkerStyle(20);
        hRelDiffHybrid->SetMarkerColor(kBlack);
        hRelDiffHybrid->SetMarkerSize(2.3); // Increase marker size
        
        hRelDiffHybrid->SetMinimum(-1000); // Adjust range as needed
        hRelDiffHybrid->SetMaximum(1000);

        // Restore the normal axis properties – reduce the font size
        hRelDiffHybrid->GetXaxis()->SetLabelSize(0.035);
        hRelDiffHybrid->GetXaxis()->SetTitle("#font[132]{#it{z}} (cm)"); // change title
        hRelDiffHybrid->GetXaxis()->SetTitleSize(0.045);
        hRelDiffHybrid->GetXaxis()->SetTitleOffset(1.1);
        hRelDiffHybrid->GetXaxis()->CenterTitle(); // Center
        hRelDiffHybrid->GetXaxis()->SetTickLength(0.03);

        hRelDiffHybrid->GetYaxis()->SetLabelSize(0.035);
        hRelDiffHybrid->GetYaxis()->SetTitle("#font[132]{#frac{#Delta#it{N}}{#it{N}}}"); // Change to math formula font
        hRelDiffHybrid->GetYaxis()->SetTitleSize(0.045);
        hRelDiffHybrid->GetYaxis()->SetTitleOffset(1.3);
        hRelDiffHybrid->GetYaxis()->CenterTitle(); // Center
        hRelDiffHybrid->GetYaxis()->SetNdivisions(505);

        // Draw: show only points, not error bars
        // Method: clone a histogram and set its errors to zero, then draw it with "P"
        TH1F* hRelDiffDraw = (TH1F*)hRelDiffHybrid->Clone("hRelDiffDraw");
        for (int i=0; i<=hRelDiffDraw->GetNbinsX()+1; ++i) hRelDiffDraw->SetBinError(i, 0.0);
        
        // Separate positive and negative values using different colors
        // Positive values use the orange color from plot 1 (kOrange+1), negative values use the blue color from plot 1 (kAzure+2)
        TH1F* hRelDiffPos = (TH1F*)hRelDiffDraw->Clone("hRelDiffPos");
        TH1F* hRelDiffNeg = (TH1F*)hRelDiffDraw->Clone("hRelDiffNeg");
        
        hRelDiffPos->SetMarkerColor(kOrange+1);
        hRelDiffNeg->SetMarkerColor(kAzure+2);
        
        // Loop over all bins and set the bins that should not be displayed to a very small value (outside the plotting range)
        for (int i=1; i<=hRelDiffDraw->GetNbinsX(); ++i) {
             double val = hRelDiffDraw->GetBinContent(i);
             if (val >= 0) {
                 hRelDiffNeg->SetBinContent(i, -9999999.0); // Hide positive values in Neg plot
             } else {
                 hRelDiffPos->SetBinContent(i, -9999999.0); // Hide negative values in Pos plot
             }
        }

        hRelDiffPos->Draw("P");      // Draw axes and positive values
        hRelDiffNeg->Draw("P SAME"); // Overlay negative values
        
        TLine *line = new TLine(xMin, 0.0, xMax, 0.0);
        line->SetLineStyle(2);
        line->SetLineColor(kRed);
        line->SetLineWidth(4); // Thicken
        line->Draw();
    }

    c1->SaveAs(outputName + ".png");

    // ---------------------------------------------------------
    // 7. Draw an efficiency comparison on a new canvas (truth vs reco)
    // ---------------------------------------------------------
    TCanvas* c2 = new TCanvas("c2", "Efficiency Comparison", 3000, 3000);
    c2->SetLeftMargin(0.12);
    c2->SetRightMargin(0.04);
    c2->SetBottomMargin(0.12);
    c2->SetTopMargin(0.08); 
    c2->SetTicks(1, 1);
    c2->SetGrid();
    
    // Set the style
    hEff->SetLineColor(kGreen+2);
    hEff->SetMarkerColor(kGreen+2);
    hEff->SetMarkerStyle(20);
    hEff->SetMarkerSize(2.5); // Increase from 2.0 to 2.5
    hEff->SetLineWidth(4);
    hEff->SetTitle(""); // Remove title
    
    // Axis styles similar to c1
    hEff->GetXaxis()->SetLabelSize(0.035);
    hEff->GetXaxis()->SetTitle("#font[132]{#it{z}} (cm)");
    hEff->GetXaxis()->SetTitleSize(0.045);
    hEff->GetXaxis()->SetTitleOffset(1.1);
    hEff->GetXaxis()->CenterTitle();

    hEff->GetYaxis()->SetLabelSize(0.035);
    hEff->GetYaxis()->SetTitle("#font[132]{Efficiency}"); // Styled Y title
    hEff->GetYaxis()->SetTitleSize(0.045);
    hEff->GetYaxis()->SetTitleOffset(1.3);
    hEff->GetYaxis()->CenterTitle();
    
    hEff->SetMinimum(0.0);
    hEff->SetMaximum(1.2);

    hEffReco->SetLineColor(kOrange+7);
    hEffReco->SetMarkerColor(kOrange+7);
    hEffReco->SetMarkerStyle(21);
    hEffReco->SetMarkerSize(2.5); // Increase from 2.0 to 2.5
    hEffReco->SetLineWidth(4);

    hEff->Draw("E");
    hEffReco->Draw("E SAME");

    TLegend* legEff = new TLegend(0.7, 0.75, 0.9, 0.9); // Put in bottom right corner
    legEff->SetFillStyle(0);
    legEff->SetBorderSize(0);
    legEff->SetTextSize(0.035); // Adjust text size
    legEff->AddEntry(hEff, "Truth Eff", "lp");
    legEff->AddEntry(hEffReco, "Reco Eff", "lp");
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
