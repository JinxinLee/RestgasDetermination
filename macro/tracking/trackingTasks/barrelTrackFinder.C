// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int barrelTrackFinder(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "evtcomplete";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "barreltracking";
  TString  friend1        = "sim";
  TString  friend2        = "digi";
  TString  friend3        = "";
  TString  friend4        = "";
  TString  fOptions       = "gf2";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->AddFriend(friend3);
  fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  PndBarrelTrackFinder *tracking = new PndBarrelTrackFinder();
  tracking->UseMvdSttGem(kTRUE, kTRUE, kTRUE);
  tracking->SetPersistency(kTRUE);
  fRun->AddTask(tracking);

  if (fOptions.Contains("gf1")){
      FairGeane *Geane = new FairGeane();
      fRun->AddTask(Geane);

      PndRecoKalmanTask* recoKalman = NULL;
      fRun->AddTask(recoKalman = new PndRecoKalmanTask());
      recoKalman->SetTrackInBranchName("BarrelTrack");
      recoKalman->SetTrackOutBranchName("BarrelGenTrack");

      recoKalman->SetBusyCut(50); // CHECK to be tuned
      //recoKalman->SetIdealHyp(kTRUE);
      //recoKalman->SetNumIterations(3);
      recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
      //recoKalman->SetPropagateToIP(kFALSE);
  } else if (fOptions.Contains("gf2")){
      FairGeane *Geane = new FairGeane();
      fRun->AddTask(Geane);

      PndRecoKalmanTask2* recoKalman = NULL;
      fRun->AddTask(recoKalman = new PndRecoKalmanTask2());
      recoKalman->SetTrackInBranchName("BarrelTrack");
      recoKalman->SetTrackOutBranchName("BarrelGenTrack");

      recoKalman->SetBusyCut(50); // CHECK to be tuned
      //recoKalman->SetIdealHyp(kTRUE);
      //recoKalman->SetNumIterations(3);
      //recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
      //recoKalman->SetPropagateToIP(kFALSE);
  }

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
