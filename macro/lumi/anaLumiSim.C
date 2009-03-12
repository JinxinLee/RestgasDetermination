/*
 * anaLumiSim.C
 *
 *  Created on: Feb 15, 2008
 *      Author: tsito
 */

{
	//Load basic libraries

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	Double_t pi=TMath::Pi();

	// Set a pointer to the file (tree) contains the output
	TFile *f_mc = new TFile("lumisim.root");
	TTree *tr_mc = f_mc->Get("cbmsim") ;

	// Collect data in an array
	TClonesArray* point = new TClonesArray("PndLumiPoint");
	tr_mc->SetBranchAddress("LumiPoint",&point);

	TClonesArray* track = new TClonesArray("CbmMCTrack");
	tr_mc->SetBranchAddress("MCTrack",&track);

	//Histogram definition

	TH1F *hX_mc = new TH1F("","",100, -10., 10.);
	TH1F *hY_mc = new TH1F("","",100, -10., 10.);
	TH1F *hZ_mc = new TH1F("","",100, 1000., 1200.);

	TH2F *hXY_Plan = new TH2F("","",100, -10., 10., 100, -10., 10.);
	//Variable declaration
	Double_t energy;
	TVector3 pos_reco, pos_mc;
	Int_t nhits = 0;

	Double_t x, y, r, z;

	for ( Int_t i = 0 ; i < tr_mc->GetEntriesFast(); i++){
		tr_mc->GetEntry(i);
		for (Int_t k = 0; k < point->GetEntriesFast(); k++){
			PndLumiPoint *MyPoint = (PndLumiPoint*)point->At(k);

			x = MyPoint->GetEntryX(); y = MyPoint->GetEntryY();
			hXY_Plan->Fill(x,y);

		}
	}

	TCanvas *c_sim = new TCanvas("c_sim","c_sim",0, 0, 900, 900);
	hXY_Plan->Draw();



}
