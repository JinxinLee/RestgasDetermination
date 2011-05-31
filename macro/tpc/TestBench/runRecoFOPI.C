void runRecoFOPI(TString digifile) {
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
  TString allDigiFile = sysFile+"/tpc/TestBench/tpc.TBtestChamber.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  PndGeoHandling* geoH = PndGeoHandling::Instance();


  // ------- RECO procedure ------------------------------------------------

  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetDigiPersistence(); // keep reference to digis in clusters
  tpcCF->SetPersistence(); // keep Clusters
  tpcCF->timeslice(5); //in samples
  tpcCF->SetThreshold(1);
  tpcCF->SetSingleDigiClusterAmpCut(0.);
  tpcCF->SetClusterAmpCut(0.); // cut on mean digi amplitude
  tpcCF->SetErrorPars(600.,400.);
  tpcCF->SetSimpleClustering(); // use PndTpcClusterFinderSimple
  fRun->AddTask(tpcCF);

  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetSortingParameters(
                   true, // false: sort only according to _sorting (see next argument); true: use internal sorting when adding hits to trackcands
                   3,    // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to origin
                   0.); // z-position of interaction point (for sorting 4)
  tpcSPR->SetTrkFinderParameters(
                   1.9,  // proximity cut in 3D
                   0.1, // proximity cut on rieman sphere
                   0.04, // distance to plane cut
                   0.2,  // szcut
                   4);   // minimum hits for plane & sz-fit
  tpcSPR->SetMergeTracks();
  tpcSPR->SetTrkMergerParameters(
                   2.2,  // proximity cut
                   0.33,  // sz cut
                   0.025);// plane cut (RMS)
  tpcSPR->SetRiemannScale(); // sets riemannscale for the prototype;
  tpcSPR->SetPersistence();
  //tpcSPR->SetStoreHistograms(PROutFile); //
  //tpcSPR->WriteHistograms(PROutFile);
  fRun->AddTask(tpcSPR);

  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);

  PndTpcResidualTask* Res = new PndTpcResidualTask();
  Res->SetPersistence();
  Res->SetNumberOfTrackReps(2);
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
