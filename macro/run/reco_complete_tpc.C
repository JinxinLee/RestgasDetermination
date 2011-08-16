void reco_complete_tpc()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits for EMC

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");  
  rootlogon();
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
	// Number of events to process
  Int_t nEvents = 100;  // if 0 all the vents will be processed
  
  // Parameter file
  TString parFile = "simparams.root"; // at the moment you do not need it
  
  // Digitisation file (ascii)
  TString digiFile = "all.par";
  
  // Output file
  TString outFile = "reco_complete_tpc.root";
  // MC file
  TString mcFile = "sim_complete_tpc.root";
  
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("sim_complete_tpc.root");
  fRun->AddFriend("digi_complete_tpc.root");
  fRun->SetOutputFile(outFile);


  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);


  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  

  // -----   EMC hit producers   ---------------------------------
  // The file name should be the same of the geometry file which was used for the simulation
  
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  emcMakeCluster->SetStorageOfData(kFALSE);
  //fRun->AddTask(emcMakeCluster);
  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  //fRun->AddTask(emcHdrFiller); // ECM header
  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  //fRun->AddTask(emcMakeBump);

  //------ Ideal DCH track finder --------------------
  PndDchFindTracks* finderTask = new PndDchFindTracks("dchFindTracks");
  finderTask->SetUseHitOrDigi("chit");
  //fRun->AddTask(finderTask);
  // ------------------------------------------------- 
  PndDchTrackFinderIdealCylHit* mcTrackFinder = new  PndDchTrackFinderIdealCylHit();
  mcTrackFinder->SetPrimary(1);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------
  PndDchMatchTracks *matchTask = new PndDchMatchTracks();//match PndDchTracks and MCTracks
  matchTask->SetUseHitOrDigi("chit");
  //fRun->AddTask(matchTask);

  //----- MVD Hit Reco -----
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  //fRun->AddTask(mvdmccls);


	
  //------ GEM Realistic Track finder --------------------
  //Create and add finder task
  PndGemFindTracks* gemFinderTask = new  
  PndGemFindTracks("PndGemFindTracks");
  gemFinderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  // fRun->AddTask(gemFinderTask);
	
  PndGemTrackFinderOnHits* gemTrackFinder = new   PndGemTrackFinderOnHits();
  gemTrackFinder->SetVerbose(0);  // verbosity level
  gemTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are  processed, 0 = all (default)
  gemFinderTask->UseFinder(gemTrackFinder);
	
  PndGemTrackFinderQA* gemTrackFinderQA = new PndGemTrackFinderQA();
  gemTrackFinderQA->SetVerbose(0);
  // fRun->AddTask(gemTrackFinderQA);
	


    // -----   TPC Reco Sequence  --------------------------------------------
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


  //find PndTpcRiemannTracks in the TPC alone
  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  //tpcSPR->SetPersistence();
  //tpcSPR->SetVerbose(1);
  fRun->AddTask(tpcSPR);

  //build GFTracks from PndTpcRiemannTracks
  PndTpcTrackInitTask* trackInit=new PndTpcTrackInitTask();
  trackInit->SetPersistence();
  //trackInit->SetVerbose(1);
  //trackInit->SetMCPid(); // use ideal particle identification
  trackInit->SetPDG(211);
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
  //Res->SetNumberOfTrackReps(2);
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  fRun->AddTask(Res);
  

	
  // -----   Intialise and run   --------------------------------------------
  cout << "fRun->Init()" << endl;
  fRun->Init();

  timer.Start();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

  tpcSPR->WriteHistograms("Riemann");

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
  cout << " Test passed" << endl;
  cout << " All ok " << endl;

//  exit(0);  
}
