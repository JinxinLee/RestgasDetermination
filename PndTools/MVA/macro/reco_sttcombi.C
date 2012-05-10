void reco_sttcombi(char inDigiFile [] = "digi_sttcombi.root",   //Input file Digi
                   char inSimFile  [] = "points_sttcombi.root", // Input file Sim
                   char parFile    [] = "params_sttcombi.root",     // Parameter file
                   char outFile    [] = "reco_sttcombi.root" // Output file
                   )
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
    
  // Number of events to process
  Int_t nEvents = 0;
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
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
  fRun->AddFriend(inSimFile);
  fRun->SetOutputFile(outFile);

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  // ------------------------------------------------------------------------
  
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile);
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile,"in");
  
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------
  // -----   LHETRACK  ---------------------------------
  
  PndMvdRiemannTrackFinderTask* mvdTrackFinder = new PndMvdRiemannTrackFinderTask();
  mvdTrackFinder->SetVerbose(iVerbose);
  mvdTrackFinder->SetMaxDist(0.05);
  mvdTrackFinder->SetPersistence(kFALSE);
  fRun->AddTask(mvdTrackFinder);
  
  //  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", 
                                                         sttTrackFinder, iVerbose);

  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  //sttFindTracks->SetPersistence(kFALSE);
  fRun->AddTask(sttFindTracks);
  
  PndSttMvdTracking *  SttMvdTracking = new PndSttMvdTracking(0, false, false);
  //SttMvdTracking->Cleanup();
  SttMvdTracking->SetPersistence(kFALSE);
  fRun->AddTask(SttMvdTracking);
  
  PndSttMvdGemTracking * SttMvdGemTracking = new PndSttMvdGemTracking(0);
  //SttMvdGemTracking->SetPdgFromMC();
  fRun->AddTask(SttMvdGemTracking);
  
  PndMCTrackAssociator* trackMC = new PndMCTrackAssociator();
  trackMC->SetTrackInBranchName("SttMvdGemTrack");
  trackMC->SetTrackOutBranchName("SttMvdGemTrackID");
  fRun->AddTask(trackMC);

  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("SttMvdGemTrack");
  recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  // recoKalman->SetIdealHyp(kTRUE);
  // recoKalman->SetNumIterations(3);
  // recoKalman->SetParticleHypo("electron");
  // recoKalman->SetParticleHypo(11);
  fRun->AddTask(recoKalman);
  
  //////////////////
  /*
    PndRecoMultiKalmanTask* recoKalman = new PndRecoMultiKalmanTask();
    recoKalman->SetTrackInBranchName("LheTrack");
    recoKalman->SetTrackOutBranchName("LheGenTrack");
    fRun->AddTask(recoKalman);
  */
  //////////////////// 
  
  PndMCTrackAssociator* trackMC2 = new PndMCTrackAssociator();
  trackMC2->SetTrackInBranchName("SttMvdGemGenTrack"); 
  trackMC2->SetTrackOutBranchName("SttMvdGemGenTrackID");
  fRun->AddTask(trackMC2);

  // Correlator Task
  //PndPidCorrelator* corr = new PndPidCorrelator();
  //corr->SetVerbose();
  //corr->SetInputBranch("SttMvdGemGenTrack");
  //corr->SetInputIDBranch("SttMvdGemGenTrackID");
  //corr->SetInputBranch2("FTSTrkIdeal");
  //corr->SetInputIDBranch2("FTSTrkIdealID");
  //fRun->AddTask(corr);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  
  fRun->Init();
  fRun->Run(0, nEvents);
  
  rtdb->saveOutput();
  rtdb->print();
  
  // ------------------------------------------------------------------------
  
  // -----   Finish   -------------------------------------------------------
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "\n\nMacro finished succesfully.\n"
       << "Output file is "    << outFile
       << "\nParameter file is " << parFile
       << "Real time " << rtime << " s, CPU time " << ctime << " s.\n\n";
  // ------------------------------------------------------------------------
  exit(0);
}
