// Macro for running Panda reconstruction tasks
// to run the macro:
// root  reco_complete.C  or in root session root>.x  reco_complete.C
int reco_complete(TString prefix, Int_t nEvents=0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "";
  TString  output         = "reco";
  TString  friend1        = "digi";
  TString  friend2        = "";
  TString  friend3        = "";
  TString  friend4        = "";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->SetFriend1(friend1);
  fRun->SetFriend2(friend2);
  fRun->SetFriend3(friend3);
  fRun->SetFriend4(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  
  // -----   Add tasks   ----------------------------------------------------
  // here comes your tracking code
  
    FairGeane *Geane = new FairGeane();
    fRun->AddTask(Geane);

    PndTrkTracking2* tracking = new PndTrkTracking2(0,false,false,true);
    tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
    // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
    //  don't do the Pattern Recognition second part, starting from the Mvd;
    tracking->NoMvdAloneTracking();
    // do Cleanup only when there is Mixing;
    // tracking->Cleanup();
    tracking->SetPersistence(kTRUE);
    fRun->AddTask(tracking);

    PndSttMvdGemTracking * SttMvdGemTracking = new PndSttMvdGemTracking(0);
    //SttMvdGemTracking->SetPdgFromMC();
    SttMvdGemTracking->SetPersistence(kTRUE);
    fRun->AddTask(SttMvdGemTracking);


    PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
    recoKalman->SetTrackInBranchName("SttMvdGemTrack");
  //  recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
    recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
    recoKalman->SetBusyCut(50); // CHECK to be tuned
  //  recoKalman->SetIdealHyp(kTRUE);
    //recoKalman->SetNumIterations(3);
    recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
    //recoKalman->SetPropagateToIP(kFALSE);
    fRun->AddTask(recoKalman);


    // ---- PndIdealTrackFinder has to stay for QA -----

    PndIdealTrackFinder* idealTracking = new PndIdealTrackFinder();
    idealTracking->SetOutputBranchName("IdealTrack");
//    idealTracking->SetTrackSelector("StandardTrackFunctor");
    fRun->AddTask(idealTracking);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
