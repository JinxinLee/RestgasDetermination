int printFairLinks(Int_t nEvents = 0, TString prefix="../data/evtcomplete")
{

	// Number of events to process
//  Int_t nEvents = 0;  // if 0 all the vents will be processed

	 TString  parAsciiFile   = "all.par";
	  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
	  TString  output         = "digionly";
	  TString  friend1        = "digi";
	  TString  friend2        = "reco";
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

  PndPrintFairLinks* printlinksTask = new PndPrintFairLinks();
  fRun->AddTask(printlinksTask);

  // -----   Intialise and run   --------------------------------------------

  cout << "fRun->Init()" << endl;

  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

  return 0;
}
