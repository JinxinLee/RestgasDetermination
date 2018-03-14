#include "../auxi.C"
int recoideal_complete(Int_t nEvents = 0)
{
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
	  fRun->SetUseFairLinks(kTRUE);
	  fRun->Setup();

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);


  PndIdealTrackFinder* track = new PndIdealTrackFinder();
  track->SetTrackSelector("NoFtsTrackFunctor");
  track->SetRelativeMomentumSmearing(0.05);
  track->SetVertexSmearing(0.05, 0.05, 0.05);
  track->SetTrackingEfficiency(1.);
  track->SetOutputBranchName("SttMvdGemIdealTrack");
  track->SetPersistency(kFALSE);
  fRun->AddTask(track);


  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("SttMvdGemIdealTrack");
  //recoKalman->SetTrackInIDBranchName("SttMvdGemIdealTrackID");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetIdealHyp(kTRUE);
  //recoKalman->SetNumIterations(3);
  fRun->AddTask(recoKalman);


  PndIdealTrackFinder* trackFts = new PndIdealTrackFinder();
  trackFts->SetTrackSelector("FtsTrackFunctor");
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetOutputBranchName("FtsIdealTrack");
  trackFts->SetPersistency(kFALSE);
  fRun->AddTask(trackFts);


  PndRecoKalmanTask* recoKalmanFwd = new PndRecoKalmanTask();
  recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
  //recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  //recoKalmanFwd->SetIdealHyp(kTRUE);
  //recoKalmanFwd->SetNumIterations(3);
  fRun->AddTask(recoKalmanFwd);


  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  CloseGeoManager();
  return 0;
}
