// Macro for running Panda reconstruction tasks
// to run the macro:
// root  recoideal_complete.C  or in root session root>.x  recoideal_complete.C
void recoideal_complete(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "recoideal";
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
  
  PndSttMvdGemTrackingIdeal* trackStt = new PndSttMvdGemTrackingIdeal();
  trackStt->SetRelativeMomentumSmearing(0.05);
  trackStt->SetVertexSmearing(0.05, 0.05, 0.05);
  trackStt->SetTrackingEfficiency(1.);
  trackStt->SetTrackOutput("SttMvdGemIdealTrack");
  trackStt->SetPersistence(kFALSE);
  fRun->AddTask(trackStt);
   
  PndMCTrackAssociator* trackMC = new PndMCTrackAssociator();
  trackMC->SetTrackInBranchName("SttMvdGemIdealTrack");
  trackMC->SetTrackOutBranchName("SttMvdGemIdealTrackID");
  fRun->AddTask(trackMC);

  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("SttMvdGemIdealTrack");
  recoKalman->SetTrackInIDBranchName("SttMvdGemIdealTrackID");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  recoKalman->SetIdealHyp(kTRUE);
  //recoKalman->SetNumIterations(3);
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
  recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  recoKalmanFwd->SetIdealHyp(kTRUE);
  //recoKalmanFwd->SetNumIterations(3);
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
