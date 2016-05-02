// Macro for running Panda reconstruction tasks
// to run the macro:
// root  reco_complete_sec.C  or in root session root>.x  reco_complete_sec.C
void reco_complete_sec(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "recosec";
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
  fRun->Setup();
  
  // -----   Add tasks   ----------------------------------------------------
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

   //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
  PndTrkTracking2* tracking = new PndTrkTracking2(0,false,false,true);
  tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
  // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  //  don't do the Pattern Recognition second part, starting from the Mvd;
  tracking->NoMvdAloneTracking();
  // do Cleanup only when there is Mixing;
  // tracking->Cleanup();
  tracking->SetPersistence(kFALSE);
  fRun->AddTask(tracking);
  
  PndSttMvdGemTracking * SttMvdGemTracking = new PndSttMvdGemTracking(0);
  //SttMvdGemTracking->SetPdgFromMC();
  SttMvdGemTracking->SetPersistence(kFALSE);
  fRun->AddTask(SttMvdGemTracking);
  
  // ------------------------------------------------------------------------
  // secondary track finder -------------------------------------------------
  PndTrkTrackFinder *ltracking = new PndTrkTrackFinder(0);
  //   ltracking->SwitchOnDisplay();
  ltracking->SearchSecondaryTracks();
  // ltracking->DeletePrimaryHits();
  fRun->AddTask(ltracking);
  
  // ------------------------------------------------------------------------
  // sum I and II track finder ----------------------------------------------
  PndTrkAddTCA *add = new PndTrkAddTCA();
  fRun->AddTask(add);
  
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("CombiTrack");
  //recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetIdealHyp(kTRUE);
  //recoKalman->SetNumIterations(3);
  recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalman->SetPropagateToIP(kFALSE);
  fRun->AddTask(recoKalman);

  PndMCTrackAssociator* trackMC2 = new PndMCTrackAssociator();
  trackMC2->SetTrackInBranchName("SttMvdGemGenTrack"); 
  trackMC2->SetTrackOutBranchName("SttMvdGemGenTrackID");
  fRun->AddTask(trackMC2);
 
  PndFtsTrackerIdeal* trackFts = new PndFtsTrackerIdeal();
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetTrackOutput("FtsIdealTrack");
  trackFts->SetPersistence(kFALSE);
  fRun->AddTask(trackFts);

  PndMCTrackAssociator* trackMCfwd = new PndMCTrackAssociator();
  trackMCfwd->SetTrackInBranchName("FtsIdealTrack");
  trackMCfwd->SetTrackOutBranchName("FtsIdealTrackID");
  fRun->AddTask(trackMCfwd);

  PndRecoKalmanTask* recoKalmanFwd = new PndRecoKalmanTask();
  recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
  //recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  //recoKalmanFwd->SetIdealHyp(kTRUE);
  //recoKalmanFwd->SetNumIterations(3);
  recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalmanFwd->SetPropagateToIP(kFALSE);
  fRun->AddTask(recoKalmanFwd);

  PndMCTrackAssociator* trackMC3 = new PndMCTrackAssociator();
  trackMC3->SetTrackInBranchName("FtsIdealGenTrack");
  trackMC3->SetTrackOutBranchName("FtsIdealGenTrackID");
  fRun->AddTask(trackMC3);
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  exit(0);
}
