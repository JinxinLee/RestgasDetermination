int MCTrackInfo(Int_t nEvents = 0, TString prefix="../data/evtcomplete")
{

	// Number of events to process
//  Int_t nEvents = 0;  // if 0 all the vents will be processed

	 TString  parAsciiFile   = "all.par";
	  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
	  TString  output         = "mctrackinfo";
	  TString  friend1        = "sim";
	  TString  friend2        = "";
	  TString  friend3        = "";
	  TString  friend4        = "";

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

	  fRun->UseFairLinks(kTRUE);

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // ------------------------------------------------------------------------

  PndMCTrackInfoTask* trackInfo = new PndMCTrackInfoTask();
  trackInfo->SetMaxStage(2);
  fRun->AddTask(trackInfo);

  // -----   Intialise and run   --------------------------------------------

  cout << "fRun->Init()" << endl;

  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

  return 0;
}
