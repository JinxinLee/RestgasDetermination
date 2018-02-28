int printFairLinks(Int_t nEvents = 0)
{

	// Number of events to process
//  Int_t nEvents = 0;  // if 0 all the vents will be processed

  TString inputName = "evtcomplete";
  Bool_t useDigiFile = kTRUE;
  Bool_t useRecoFile = kTRUE;
  Bool_t usePidFile = kTRUE;

  PndFileNameCreator creator(inputName.Data());

  TString MCFile = creator.GetSimFileName();
  TString DigiFile = creator.GetDigiFileName();
  TString RecoFile = creator.GetRecoFileName();
  TString PidFile = creator.GetPidFileName();
  TString ParFile = creator.GetParFileName();

  // Digitisation file (ascii)
  TString digiFile = "all.par";
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  FairFileSource* fileSource = new FairFileSource(MCFile.Data());
  fRun->SetSource(fileSource);

  TFile* testFile;
  if (useDigiFile == kTRUE){
	testFile = new TFile(DigiFile.Data());
	if (!testFile->IsZombie()){
	 fileSource->AddFriend(DigiFile.Data());
	}
	testFile->Close();
  }

  if (useRecoFile == kTRUE){
	  testFile = new TFile(RecoFile.Data());
	  if (!testFile->IsZombie()){
		 fileSource->AddFriend(RecoFile.Data());
	  }
	  testFile->Close();
  }

  if (usePidFile == kTRUE){
	  testFile = new TFile(PidFile.Data());
	  if (!testFile->IsZombie()){
		 fileSource->AddFriend(PidFile.Data());
	  }
	  testFile->Close();
  }

  fRun->SetOutputFile("TST.root");
  fRun->SetGenerateRunInfo(kTRUE);
  fRun->SetUseFairLinks(kTRUE);

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----  Parameter database   --------------------------------------------
  TString digiParFile = gSystem->Getenv("VMCWORKDIR");
  digiParFile += "/macro/params/";
  digiParFile += digiFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(digiParFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  // ------------------------------------------------------------------------

  PndPrintFairLinks* printlinksTask = new PndPrintFairLinks();
  fRun->AddTask(printlinksTask);

  // -----   Intialise and run   --------------------------------------------

  cout << "fRun->Init()" << endl;

  fRun->Init();

  timer.Start();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;

  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;

  return 0;
}
