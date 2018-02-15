{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents = 0;
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------
  // Output file
  TString parFile    = "params_sttcombi.root";
  TString inSimuFile = "points_sttcombi.root";
  TString inDigiFile = "digi_sttcombi.root";
  TString inRecoFile = "reco_sttcombi.root";

  TString outFile = "pid_sttcombi.root";
   
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inSimuFile);
  fRun->AddFriend(inDigiFile);
  fRun->AddFriend(inRecoFile);
  fRun->SetOutputFile(outFile.Data());
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------
  
  PndPidCorrelator* corr = new PndPidCorrelator();
  corr->SetVerbose(10);
  corr->SetInputBranch("SttMvdGemGenTrack");
  corr->SetInputIDBranch("SttMvdGemGenTrackID");
  //corr->SetInputBranch2("FTSTrkIdeal");
  //corr->SetInputIDBranch2("FTSTrkIdealID");
  //corr->SetInputBranch("SttMvdGenTrack");
  corr->SetDebugMode(kTRUE);
  //corr->SetFast(kTRUE);
  fRun->AddTask(corr);
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  
  //////////////
  std::vector<std::string> labels;
  std::vector<std::string> vars;

  labels.push_back("electron");
  labels.push_back("pion");
  
  vars.push_back("emc");
  vars.push_back("lat");  
  vars.push_back("z20");
  vars.push_back("z53");
  vars.push_back("E9E25");

  PndPidMvaAssociatorTask* ts = new PndPidMvaAssociatorTask();
  // Set the path to the weightFiles; otherwise the standard
  // path is used.
  ts->SetWeightFileName("/tmp/test.root");
  
  // Set variable- and class-names.
  ts->SetClassNames(labels);
  ts->SetVarNames(vars);

  // Set classifiers type.
  /*
   * KNN, KNN methode.
   * LVQ, LVQ methode.
   * TMVA_MLP, Multi label TMVA ANN.
   * TMVA_BDT, Multi label TMVA BDT.
   * See PndPidMvaAssociatorTask.cxx
   */
  ts->SetClassifier(LVQ);
  
  // If KNN is selected, then set the number of neighbors.
  ts->SetNumNeigh(250);
  
  //Add task
  fRun->AddTask(ts);
  
  //////////////
  fRun->Init();
  fRun->Run(0,nEvents);
  
  // ------------------------------------------------------------------------
  rtdb->print();
  
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "\n\nMacro finished succesfully.\n"
       << "Output file is "    << outFile
       << "\nParameter file is " << parFile
       << "\nReal time " << rtime << " s, CPU time " 
       << ctime << " s.\n";
  // ------------------------------------------------------------------------
  exit(0);
}
