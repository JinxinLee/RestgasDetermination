void runMomresRecoMVD_batch(TString digifile, Int_t nEvents = 0) {
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
  outFile.ReplaceAll("raw.root", "MVD.reco.root");
  

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

  PndTpcRoughAlignmentTask* align = new PndTpcRoughAlignmentTask();
  align->SetShift(TVector3(0.,0.,-3.71357e-01));
  fRun->AddTask(align);

  //find PndTpcRiemannTracks in the TPC alone
  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  //tpcSPR->SetPersistence();
  //tpcSPR->SetVerbose(1);
  fRun->AddTask(tpcSPR);

  //build GFTracks from PndTpcRiemannTracks
  PndTpcTrackInitTask* trackInit=new PndTpcTrackInitTask();
  trackInit->SetPersistence();
  //trackInit->SetVerbose(1);
  trackInit->SetMCPid(); // use ideal particle identification
  //trackInit->SetPDG(211);
  //trackInit->useGeane(); // uses RKTrackrep and GeaneTrackrep
  trackInit->SetSmoothing(true);
  fRun->AddTask(trackInit);

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
  Res->SetNumberOfTrackReps(2);
  //fRun->AddTask(Res);
  
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  PndTpcMVDCorrelatorTask* corr = new PndTpcMVDCorrelatorTask();
  corr->SetMatchDistance(100.);   //mutliple of MVD hit sigma (which is roughly 20 mu)
  corr->SetMinMVDHits(3);
  corr->SetPersistence(true);
  fRun->AddTask(corr);
  
  //fit once more
  KalmanTask* kalman2 =new KalmanTask();
  kalman2->SetPersistence();
  kalman2->SetNumIterations(3); // number of fitting iterations (back and forth)
  kalman2->SetTrackBranchName("TrackPreFitComplete");
  kalman2->SetOutBranchName("TrackPostFitComplete");
  fRun->AddTask(kalman2);
  
  PndTpcResidualTask* Res2 = new PndTpcResidualTask();
  Res2->SetPersistence();
  Res2->SetTrackBranchName("TrackPostFitComplete");
  Res2->SetOutBranchName("TrackFitStatMVD");
  Res2->SetNumberOfTrackReps(2);
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  //fRun->AddTask(Res2);
    
  // -----   Intialise and run   --------------------------------------------
  
  fRun->Init();
  std::cout<<"post init"<<std::endl;
  fRun->Run(0, nEvents);

  rtdb->saveOutput();
  rtdb->print();
  
  // ------------------------------------------------------------------------
  
  TString resName = inDigiFile;
  resName.ReplaceAll("raw.root", "MVDres.root");
  corr->WriteHistograms(resName);
  
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
