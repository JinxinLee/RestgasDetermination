void runMomresReco_batch(TString digifile) {
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file
  TString inDigiFile = digifile;
  TString inSimFile = inDigiFile;
  inSimFile.ReplaceAll("raw.root", "mc.root");
  // Parameter file
  TString parFile = inDigiFile;
  parFile.ReplaceAll("raw.root", "param.root");

  // Output file
  TString outFile = inDigiFile;
  outFile.ReplaceAll("raw.root", "reco.root");
  

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inDigiFile);
  cout<<"Set Input File: "<<inDigiFile<<endl;
  fRun->AddFriend(inSimFile);
  cout<<"Set Friend MC File: "<<inSimFile<<endl;
  fRun->SetOutputFile(outFile);
  cout<<"Set Output File: "<<outFile<<endl;
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  PndGeoHandling* geoH = PndGeoHandling::Instance();

  bool SimpleClustering=true;

  // ------- RECO procedure ------------------------------------------------
  
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  //tpcCF->SetDigiPersistence(); // keep reference to digis in clusters
  tpcCF->SetPersistence(); // keep Clusters
  tpcCF->timeslice(9); //in samples
  tpcCF->SetThreshold(1);
  tpcCF->SetSingleDigiClusterAmpCut(0.);
  tpcCF->SetClusterAmpCut(0.); // cut on mean digi amplitude
  tpcCF->SetErrorPars(600.,400.);
  tpcCF->SetSimpleClustering(); // use PndTpcClusterFinderSimple
  fRun->AddTask(tpcCF);

  //find track candidates in the TPC alone
  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetPersistence();
  //tpcSPR->useGeane(); // use RKTrackrep and GeaneTrackrep
  //tpcSPR->SetMCPid(); // use ideal particle identification
  tpcSPR->SetPDG(211);
  fRun->AddTask(tpcSPR);

  PndTpcIdealTrackingTask* tpcIPR = new PndTpcIdealTrackingTask();
  //tpcIPR->useGeane(true);
  tpcIPR->useDistSorting(true);
  tpcIPR->SetPersistence();
  tpcIPR->SetSmoothing(true);
  //fRun->AddTask(tpcIPR);
  

  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);

  PndTpcResidualTask* Res = new PndTpcResidualTask();
  Res->SetPersistence();
  //Res->SetNumberOfTrackReps(2);
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  fRun->AddTask(Res);
 
  // -----   Intialise and run   --------------------------------------------
  
  fRun->Init();
  std::cout<<"post init"<<std::endl;
  fRun->Run(0, nEvents);

  rtdb->saveOutput();
  rtdb->print();

  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
