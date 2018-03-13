{
  gROOT->Reset();
  TStopwatch timer;
  timer.Start();
//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//   rootlogon();

  TFile filekalman("testgenfit.root");
  TTree *treekal = (TTree*) filekalman.Get("pndsim");
  TClonesArray *kal = new TClonesArray("GFTrack");
  treekal->SetBranchAddress("Track",&kal);

  GFTrack *trk;
  TCanvas *TestGenfit = new TCanvas("TestGenfit", "STT genfit momentum distribution", 0, 0, 600, 600);
  TH1F *h = new TH1F("h","GENFIT",100,0.5,1.5);

  // Loop on events
  for (Int_t evt = 0; evt < 1000; evt++) {
    treekal->GetEntry(evt);

    for(int trki = 0; trki < kal->GetEntries(); trki++)
      {
	trk = (GFTrack*) kal->At(trki);
	// trk->getCardinalRep().Print();

	if(trk->getTrackRep(0)->getStatusFlag()==0) {
	  h->Fill(trk->getCardinalRep()->getMom().Mag());
	}
      }
  }


  TestGenfit->cd();
  h->Draw();





}
