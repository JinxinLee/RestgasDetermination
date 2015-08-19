void emc_timebasedDigi(const char* inFile="emc_sim.root", const char* outFile="emc_timebasedDigi.root", Int_t nEvents=0, Double_t dt=50, const char* parFile="simparams.root") {

  // Loads a filewith hits and makes digitization for EMC
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  //
  // Number of events to process
  std::cout << "will process " << nEvents << " events" << std::endl;
  	
  // Digitisation file (ascii)
  TString digiFile = "emc.par";

  //Digitisation file (root)
  //TString parFile = "simparams.root";
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------
  
  // -----   Reconstruction run   ------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetUseFairLinks(kTRUE);

  // -----  Parameter database   --------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(digiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);

  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile);
  rtdb->setSecondInput(parInput1);
  
  // -----  Set tasks   ---------------------------------

  //wf simulation
  Bool_t storeWfDataClass = kTRUE;
  Bool_t storeWf = kTRUE;
  PndEmcFWEndcapTimebasedWaveforms* wfTask = new PndEmcFWEndcapTimebasedWaveforms(iVerbose);	
  wfTask->StoreDataClass(storeWfDataClass);
  wfTask->SetStorageOfData(storeWf);

  //digi 
  Bool_t storeUnsortedDigis = kTRUE;
  PndEmcFWEndcapDigi* digiTask = new PndEmcFWEndcapDigi(iVerbose, storeUnsortedDigis);
  PndEmcMakeDigi* idealDigiTask = new PndEmcMakeDigi();

  //ideal digi
  PndEmcMakeDigi* idealDigiTask = new PndEmcMakeDigi();

  //sorting
  PndEmcDigiSorterTask* sorterTask = new PndEmcDigiSorterTask(10000, 1, "EmcDigi", "EmcDigiSorted", "Emc");
  sorterTask->SetVerbose(iVerbose);

  //setup run..depending on dt parameter
  if(dt>0.) {
	  cout << "TIMEBASED DIGITAZATION" << endl;
	  FairRootManager::Instance()->SetEventMeanTime(dt);
	  wfTask->RunTimebased();
	  fRun->AddTask(wfTask);
	  fRun->AddTask(digiTask);
	  fRun->AddTask(sorterTask);
  } else if(dt==0.) {	
	  cout << "IDEAL DIGITAZATION (eventbased)" << endl;
	  fRun->AddTask(idealDigiTask);
  } else {
	  cout << "EVENTBASED SIMULATION" << endl;
	  fRun->AddTask(wfTask);
	  fRun->AddTask(digiTask);
  }

	
  // -----   Intialise and run   -----------------------
  gRandom->SetSeed();

  cout << "fRun->Init()" << endl;
  fRun->Init();

  fRun->Run(0,nEvents);

  // ----------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  
}

