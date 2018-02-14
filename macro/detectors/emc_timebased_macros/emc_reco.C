//PhiSplitter cannot handle clusters with digis having identical energy depositions inside
//As a workaround, we remove it from the bumpSplitting routines
//Call this function after Init...
void deactivatePhiSplitter(FairTask* bumpSplitter) {
	FairTask* phi_splitter= dynamic_cast<FairTask*>(bumpSplitter->GetListOfTasks()->FindObject("PndEmcPhiBumpSplitter"));
	if(phi_splitter) {
		cout << "Deactivating PhiBumpSplitter!" << endl;
		phi_splitter->SetActive(kFALSE);
	} else {
		cout << "No PndEmcPhiBumpSplitter available" << endl;
	}
}

int emc_reco(const char* digiFile, const char* simFile, const char* outFile, Int_t events=0, const char* parFile="simparams.root") {
  // Loads a filewith hits and makes digitization for EMC
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1; // just forget about it, for the moment
  
  // Input file (MC events)
  //
  // Number of events to process
  Int_t nEvents=events;  // if 0 all the events will be processed
  std::cout << "will process " << nEvents << " events" << std::endl;
  	
  // Digitisation file (ascii)
  TString parFile_Ascii = "emc.par";
  
  // Loading libraries
  // If the macro gives error messages in loading libraries, please check the path of the libs and put it by hands
  
//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
 // gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //rootlogon();
  //basiclibs();

  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------
  
  // -----   Reconstruction run   ------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(digiFile);
  fRun->AddFriend(simFile);
  fRun->SetOutputFile(outFile);
  
  fRun->SetUseFairLinks(kTRUE);

  if(TString(digiFile).Contains(TPRegexp("(?<!tim)ebased")) || TString(digiFile).Contains("ideal")) {
	cout << "WILL RUN EVENTBASED" << endl;
  } else {
	  fRun->RunWithTimeStamps();
	  cout << "WILL RUN TIMEBASED" << endl;
  }

  // -----  Parameter database   --------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(parFile_Ascii.Data(),"in");
  rtdb->setFirstInput(parIo1);

  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile);
  rtdb->setSecondInput(parInput1);

  // ----------------------------------------------------
  
  Int_t verbose = 0;
  Bool_t storeclusters = kTRUE;
  Bool_t storebumps = kTRUE;

  PndEmcMakeCluster* clusterTask = new PndEmcMakeCluster(verbose, storeclusters);
  clusterTask->StoreClusterBaseDigis(kTRUE);
  fRun->AddTask(clusterTask);

  //TODO: tasks needed due to limitiations in EmcCluster data class
  //Removes multiple entries of same crystal in one cluster --> PndEmcCluster cannot handle this
  PndEmcClusterRemoveDuplCrys* corrTask = new PndEmcClusterRemoveDuplCrys(0);
  clusterTask->Add(corrTask);
  corrTask->SetActive(kTRUE);

  PndEmcMakeBump* bumperTask = new PndEmcMakeBump(verbose, storebumps);
  fRun->AddTask(bumperTask);
	
  // -----   Intialise and run   -----------------------
  
  gRandom->SetSeed();
  cout << "fRun->Init()" << endl;
  fRun->Init();

  deactivatePhiSplitter(bumperTask); 

  cout << "fRun->Run()" << endl;
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
  
  return 0;
}

