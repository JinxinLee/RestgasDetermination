{
  gROOT->Reset();
  TStopwatch timer;
  timer.Start();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  //gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  //  gSystem->Load("libPGen");
  gSystem->Load("libStt");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libSttReco");

  TFile filekalman("testgenfit.root");
  TTree *treekal = (TTree*) filekalman.Get("cbmsim");
  TClonesArray *kal = new TClonesArray("Track");
  treekal->SetBranchAddress("Track",&kal);
 
  Track *trk;
  TCanvas *TestGenfit = new TCanvas("TestGenfit", "STT genfit momentum distribution", 0, 0, 600, 600);
  TH1F *h = new TH1F("h","GENFIT",100,0.5,1.5);

  // Loop on events
  for (Int_t evt = 0; evt < 1000; evt++) {
    treekal->GetEntry(evt);
   
    for(int trki = 0; trki < kal->GetEntries(); trki++)
      {
	trk = (Track*) kal->At(trki);
	// trk->getCardinalRep().Print();

	if(trk->getTrackRep(0)->getStatusFlag()==0) {
	  h->Fill(trk->getCardinalRep()->getMom().Mag());
	}
      } 
  }


  TestGenfit->cd();
  h->Draw();





}
