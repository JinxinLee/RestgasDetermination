{
// Analysis for PndMvdAdvancedPidAlgo: run runMvdSim.C and runMvdPidIdeal.C 
// before running this. An energyloss over momentum plot is drawn as well as
// a histo of the generated likelihoods, and a histo of 'fake' likelihoods 
// that are created with the algo's own parameterisation. 
// This macro has been tested with rev 2503 so far.

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  //opening tree
  TFile* inFile = new TFile("MvdPidIdeal.root","READ");
  TTree* tree = (TTree *)inFile->Get("cbmsim");
  TClonesArray* pointlist=new TClonesArray("PndMvdPidCand");
  tree->SetBranchAddress("PndMvdPidCand", &pointlist);


  TH2F* h1=new TH2F("h1", "likelihood count", 100, 0.0, 2.5, 100, 0.0, 1.0);
  TH2F* h2=new TH2F("h2", "energy loss", 100, 0.0, 2.5, 100, 0.0, 0.05);
  TH2F* h3=new TH2F("h3", "ideal likelihood count", 100, 0.0, 2.5, 100, 0.0, 1.0);

  //variable 

  Int_t nEvents = 10000;

  for(int j=0;j<nEvents && j<tree->GetEntriesFast();j++) {
    tree->GetEntry(j);
    for(int i=0;i<pointlist->GetEntriesFast();i++) {
      PndMvdPidCand* cand=(PndMvdPidCand*)pointlist->At(i);
      PndMvdPidCand* idealcand=new PndMvdPidCand();


      double dE=0;
      double dx=0;
      double momentum=0;

      for (int k=0;k<cand->GetMvdHits(); k++) {
        dE+=cand->GetMvdHitdE(k);
        dx+=cand->GetMvdHitdx(k);
        momentum+=cand->GetMvdHitMomentum(k);
      }

      if (dx>0) 
        momentum/=cand->GetMvdHits();

      PndMvdAdvancedPidAlgo::CalcLikelihood(PndMvdAdvancedPidAlgo::proton, momentum, idealcand);
      h2->Fill(momentum, dE/dx);
      h1->Fill(momentum, cand->GetLikelihood(2212));
      h3->Fill(momentum, idealcand->GetLikelihood(2212));
    }
  }
  TCanvas* c;
  h1->Draw();
  c=new TCanvas("c2");
  h2->Draw();
  c=new TCanvas("c3");
  h3->Draw();

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
