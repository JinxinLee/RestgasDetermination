{

  gROOT->Reset();
  TStopwatch timer;
  timer.Start();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libSttMvdTracking");

  TFile filereco("MvdStt_Test_reco.root");
  TTree *treereco = (TTree*) filereco.Get("pndsim");
  TClonesArray *prefit = new TClonesArray("PndTrack");
  treereco->SetBranchAddress("SttMvdTrack",&prefit);
  TClonesArray *kalfit = new TClonesArray("PndTrack");
  treereco->SetBranchAddress("SttMvdKalmanTrack",&kalfit);

  TH1F *hkalmom = new TH1F("hkalmom", "genfit", 100, 0.5, 1.5);
  TH1F *hpremom = new TH1F("hpremom", "prefit", 100, 0.5, 1.5);

  for(int evt = 0; evt < treereco->GetEntriesFast(); evt++)
    {
      treereco->GetEntry(evt);
      if(evt%100 == 0) cout << "EVT " << evt << endl;

      for(int ikal = 0; ikal < kalfit->GetEntriesFast(); ikal++)
	{
	  PndTrack* kaltrk = (PndTrack*) kalfit->At(ikal);
	  if(!kaltrk) continue;

	  Int_t refIndex = kaltrk->GetRefIndex();
	  if(refIndex == -1) continue;
	  PndTrack *pretrk = (PndTrack*) prefit->At(refIndex);
	  if(!pretrk) continue;

	  if(kaltrk->GetFlag() < 0) continue;

	  // FILL histos only when both kalman track
	  // and prefit track are present and kalman
	  // flag is not negative
	  TVector3 kalmom = kaltrk->GetParamFirst().GetMomentum();
	  hkalmom->Fill(kalmom.Mag());
	  TVector3 premom = pretrk->GetParamFirst().GetMomentum();
	  hpremom->Fill(premom.Mag());
	}
    }

      hkalmom->SetLineColor(4); // blue
      hpremom->SetLineColor(2); // red

      hkalmom->Draw();
      hpremom->Draw("SAME");

}
