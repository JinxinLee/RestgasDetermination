int runTrackingPid(TString runfiles, Int_t nEvents = 0)
{
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  // Number of events to process
  // Parameter file
  PndFileNameCreator namecreator(runfiles.Data());
  TString simFile = namecreator.GetSimFileName();
  TString parFile = namecreator.GetParFileName();
  TString recoFile = namecreator.GetRecoFileName();
  TString tracksFile = namecreator.GetCustomFileName("pidtracks");
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/macro/params/all.par";
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(recoFile);
  fRun->SetOutputFile(tracksFile);
  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kParameterMerged);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);
  rtdb->setOutput(parInput1);
  
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);
  
  // -----   Geane   -----
  // ---------------------
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  
  PndMvdRiemannTrackFinderTask* mvdTrackFinder = new PndMvdRiemannTrackFinderTask();
  mvdTrackFinder->SetVerbose(iVerbose);
  mvdTrackFinder->SetMaxDist(0.05);
  fRun->AddTask(mvdTrackFinder);
  
  //  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);
  
  PndSttMvdTracking *  SttMvdTracking = new PndSttMvdTracking(0);
  fRun->AddTask(SttMvdTracking);
  
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("SttMvdTrack");
  recoKalman->SetTrackOutBranchName("SttMvdGenTrack");
  //recoKalman->SetNumIterations(3);
  fRun->AddTask(recoKalman);
  
  // -----   PID   -----
  // -------------------
  
  PndPidCorrelator* corr = new PndPidCorrelator(); //Propagates by default to pca of (0,0,0)
  //corr->SetVerbose();
  corr->SetInputBranch("SttMvdGenTrack");
  //corr->SetInputIDBranch("LheTrackID");
  corr->SetPidHyp(211);
  corr->SetDebugMode(kFALSE);
  fRun->AddTask(corr);
  
  PndPidIdealAssociatorTask* pidprobs = new PndPidIdealAssociatorTask();
  fRun->AddTask(pidprobs);
  
  //  PndMCMatchCreatorTask* mcmatcher = new PndMCMatchCreatorTask();
  //  fRun->AddTask(mcmatcher);
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(6);
  fRun->Init();
  fRun->Run(0,nEvents);
  rtdb->saveOutput();
  
  rtdb->print();
  
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << recoFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
  return 0;
}

