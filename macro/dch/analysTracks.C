analysTracks() {
	// Macro loads files with simulated (PndMCTrack) and reconstructed tracks (PndDchTracks)
	// and plots various tracks quality spectra

	gStyle->SetOptFit(1);
	gStyle->SetOptStat(111111);
	gStyle->SetPalette(1,0);

	// --- Input files ---------------------------------------------
	//TString inTrackFile = "tst_trackIdeal.root";
	//TString inTrackFile = "tst_trackIdealDigi.root";
	TString inTrackFile = "tst_DigisCylhits.root";
	TFile* file = new TFile(inTrackFile); //Track-file you want to analyse
	
	// --- Go to histogram directory -------------------------------
	TDirectory* current = gDirectory;
	current->cd("DchFinderQa");
	
	// --- Momentum distribution of tracks -------------------------
	TCanvas *c1 = new TCanvas("c1", "", 50, 50, 800, 800);
	c1->Divide(3,3);
	c1->cd(1);
		hMomAccAll->Draw();
	c1->cd(2);
		hMomRecAll->Draw();
	c1->cd(3);
		hMomEffAll->Draw();
	c1->cd(4);
		hMomAccPrim->Draw();
	c1->cd(5);
		hMomRecPrim->Draw();
	c1->cd(6);
		hMomEffPrim->Draw();
	c1->cd(7);
		hMomAccSec->Draw();
	c1->cd(8);
		hMomRecSec->Draw();
	c1->cd(9);
		hMomEffSec->Draw();

	// --- Number of hords ()points||hits) per track -------------
	TCanvas *c2 = new TCanvas("c2", "", 100, 100, 800, 800);
	c2->Divide(3,3);
	c2->cd(1);
		hNpAccAll->Draw();
	c2->cd(2);
		hNpRecAll->Draw();
	c2->cd(3);
		hNpEffAll->Draw();
	c2->cd(4);
		hNpAccPrim->Draw();
	c2->cd(5);
		hNpRecPrim->Draw();
	c2->cd(6);
		hNpEffPrim->Draw();
	c2->cd(7);
		hNpAccSec->Draw();
	c2->cd(8);
		hNpRecSec->Draw();
	c2->cd(9);
		hNpEffSec->Draw();

}