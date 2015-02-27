void reco()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a digi file and performs tracking

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
	// Number of events to process
  Int_t nEvents = 0;  // if 0 all the vents will be processed
  
  TString simFile = "sim.root";
  PndFileNameCreator creator(simFile.Data());
  TString digiFile = creator.GetDigiFileName();
  TString recoFile = creator.GetRecoFileName();
  TString parFile = creator.GetParFileName();
  
  // Digitisation file (ascii)
  TString digiParFile = "all.par";
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(digiFile);
  fRun->SetOutputFile(recoFile);
  fRun->SetWriteRunInfoFile(kFALSE);
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiParFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  // ------------------------------------------------------------------------

  // -----   MDV hit producers   ---------------------------------
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
  // -----   STT digi producers   ---------------------------------
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);
  // -----   EMC hit producers   ---------------------------------
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  fRun->AddTask(emcMakeCluster);
  
  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  fRun->AddTask(emcMakeBump);
  // -----   SciT hit producers   ---------------------------
  PndSciTHitProducerIdeal* tofhit = new PndSciTHitProducerIdeal();
  tofhit->SetVerbose(iVerbose);
  fRun->AddTask(tofhit);

  // -----   MDT hit producers   ---------------------------------
  PndMdtHitProducerIdeal* mdtHitProd = new PndMdtHitProducerIdeal();
  mdtHitProd->SetPositionSmearing(.3); // position smearing [cm]
  fRun->AddTask(mdtHitProd);
  
  PndMdtTrkProducer* mdtTrkProd = new PndMdtTrkProducer();
  fRun->AddTask(mdtTrkProd);

  // -----   DRC hit producers   ---------------------------------
  PndDrcHitProducerReal* drchit = new PndDrcHitProducerReal();
  drchit->SetVerbose(iVerbose);
  fRun->AddTask(drchit);

  // -----   GEM hit producers   ---------------------------------
  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", iVerbose);
  fRun->AddTask(gemFindHits);
  // -----   FTS hit producers   ---------------------------------
  PndFtsHitProducerRealFast* ftsHitProducer = new PndFtsHitProducerRealFast();
  fRun->AddTask(ftsHitProducer);

  // -----   Ftof hit producers   ---------------------------
  PndFtofHitProducerIdeal* ftofhit = new PndFtofHitProducerIdeal();
  ftofhit->SetVerbose(iVerbose);
  fRun->AddTask(ftofhit);
  
  // -----   Central Tracking   ---------------------------------
  //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
  PndTrkTracking2* tracking = new PndTrkTracking2(0,false,false,true);
  tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
  //  don't do the Pattern Recognition second part, starting from the Mvd;
  tracking->NoMvdAloneTracking();
  // do Cleanup only when there is Mixing;
  // tracking->Cleanup();
  tracking->SetPersistence(kFALSE);
  fRun->AddTask(tracking);
  
  PndSttMvdGemTracking * SttMvdGemTracking = new PndSttMvdGemTracking(0);
  SttMvdGemTracking->SetPersistence(kFALSE);
  fRun->AddTask(SttMvdGemTracking);
  
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("SttMvdGemTrack");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetNumIterations(3);
  recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalman->SetPropagateToIP(kFALSE);
  fRun->AddTask(recoKalman);
  
  PndMCTrackAssociator* trackMC1 = new PndMCTrackAssociator();
  trackMC1->SetTrackInBranchName("SttMvdGemGenTrack"); 
  trackMC1->SetTrackOutBranchName("SttMvdGemGenTrackID");
  fRun->AddTask(trackMC1);
  
  // -----   CentralForward Tracking   ---------------------------------
  PndFtsTrackerIdeal* trackFts = new PndFtsTrackerIdeal();
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetTrackOutput("FtsIdealTrack");
  trackFts->SetPersistence(kFALSE);
  fRun->AddTask(trackFts);

  PndRecoKalmanTask* recoKalmanFwd = new PndRecoKalmanTask();
  recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  //recoKalmanFwd->SetNumIterations(3);
  recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
  recoKalmanFwd->SetPropagateToIP(kFALSE);
  fRun->AddTask(recoKalmanFwd);

  PndMCTrackAssociator* trackMC2 = new PndMCTrackAssociator();
  trackMC2->SetTrackInBranchName("FtsIdealGenTrack");
  trackMC2->SetTrackOutBranchName("FtsIdealGenTrackID");
  fRun->AddTask(trackMC2);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
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
  exit(0);
}
