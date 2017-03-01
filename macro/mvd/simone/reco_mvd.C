int reco_mvd()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits for EMC

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");  
  rootlogon();
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
	// Number of events to process
  Int_t nEvents = 0;  // if 0 all the vents will be processed
  
  // Parameter file
  TString parFile = "simparams.root"; // at the moment you do not need it
  
  // Digitisation file (ascii)
  TString digiFile = "all.par";
  
  // Output file
  TString outFile = "reco_mvd.root";
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("sim_mvd.root");
  fRun->AddFriend("digi_mvd.root");
  fRun->SetOutputFile(outFile);

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
  
//  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
//  emcMakeCluster->SetStorageOfData(kFALSE);
//  fRun->AddTask(emcMakeCluster);
//  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
//  fRun->AddTask(emcHdrFiller); // ECM header
//  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
//  fRun->AddTask(emcMakeBump);
//  PndEmcMakeRecoHit* emcMakeRecoHit= new PndEmcMakeRecoHit();
//  fRun->AddTask(emcMakeRecoHit);

//  // trackfinding ....
//  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
//  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
//  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
//  fRun->AddTask(sttFindTracks);
//  // trackmatching ....
//  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
//  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
//  fRun->AddTask(sttTrackMatcher);
//  // trackfitting ....
//  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(0);
//  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter);
//  sttFitTracks->AddHitCollectionName("STTHit");
//  fRun->AddTask(sttFitTracks);
//  // helix hit production ....
//  PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
//  fRun->AddTask(sttHHProducer);


//  //------ Ideal DCH track finder --------------------
//  PndDchFindTracks* finderTask = new PndDchFindTracks("dchFindTracks");
//  finderTask->SetUseHitOrDigi("chit");
//  fRun->AddTask(finderTask);
//  // -------------------------------------------------
//  PndDchTrackFinderIdealCylHit* mcTrackFinder = new  PndDchTrackFinderIdealCylHit();
//  mcTrackFinder->SetPrimary(1);  // 1 = Only primary tracks are processed, 0 = all (default)
//  finderTask->UseFinder(mcTrackFinder);
//  //--------------------------------------------------
//  PndDchMatchTracks *matchTask = new PndDchMatchTracks();//match PndDchTracks and MCTracks
//  matchTask->SetUseHitOrDigi("chit");
//  fRun->AddTask(matchTask);

  //----- Mvd Hit Reco -----
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  fRun->AddTask(mvdmccls);

  PndMvdIdealTrackFinderTask* mvdTrackFinder = new PndMvdIdealTrackFinderTask();
   mvdTrackFinder->SetVerbose(iVerbose);
   fRun->AddTask(mvdTrackFinder);





	
//  //------ GEM Realistic Track finder --------------------
//  //Create and add finder task
//  PndGemFindTracks* gemFinderTask = new
//  PndGemFindTracks("PndGemFindTracks");
//  gemFinderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
//  fRun->AddTask(gemFinderTask);
//
//  PndGemTrackFinderOnHits* gemTrackFinder = new   PndGemTrackFinderOnHits();
//  gemTrackFinder->SetVerbose(0);  // verbosity level
//  gemTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are  processed, 0 = all (default)
//  gemFinderTask->UseFinder(gemTrackFinder);
//
//  PndGemTrackFinderQA* gemTrackFinderQA = new PndGemTrackFinderQA();
//  gemTrackFinderQA->SetVerbose(0);
//  fRun->AddTask(gemTrackFinderQA);
//
//  KalmanTask* kalman =new KalmanTask();
//   kalman->SetPersistence();
//   kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
//   fRun->AddTask(kalman);
	
	
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
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  return 0;
}
