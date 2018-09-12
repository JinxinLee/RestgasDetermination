// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int idealforwardtracking(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "evtcomplete";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "idealforwardtracking";
  TString  friend1        = "sim";
  TString  friend2        = "digi";
  TString  friend3        = "";
  TString  friend4        = "";
  TString  fOptions       = "";

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

  PndIdealTrackFinder *tracking = new PndIdealTrackFinder();
  fRun->AddTask(tracking);
	tracking->SetTrackSelector("FtsTrackFunctor");
	tracking->AddBranchName("FTSHit");
	tracking->AddBranchName("MVDHitsPixel");
	tracking->AddBranchName("MVDHitsStrip");
//	tracking->SetRelativeMomentumSmearing(0.05);
//	tracking->SetVertexSmearing(0.05, 0.05, 0.05);
	tracking->SetTrackingEfficiency(1.);
	tracking->SetOutputBranchName("FtsIdealTrack");
	tracking->SetPersistence(kTRUE);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
