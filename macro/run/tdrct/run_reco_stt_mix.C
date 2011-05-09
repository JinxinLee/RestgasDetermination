{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file
  TString inDigiFile = "evt_digi_stt.root";
  TString inSimFile = "evt_points_stt.root";

  // Parameter file
  TString parFile = "evt_params_stt.root";

  // Output file
  TString outFile = "mix_reco_stt.root";

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
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------
 
  //-------------------------- MixBackgroundEvents  task ----------------------------------
  // The following is the task that mixes physics evt and bkg events.
  PndMixBackgroundEvents *  mix = new PndMixBackgroundEvents(0);
  mix->SetInputBkgFilesName("dpm_digi_stt.root");//inBGFile.Data());// Mvd and Stt bkg file.
  fRun->AddTask(mix);
  // =========================================================================

  PndMvdRiemannTrackFinderTask* mvdTrackFinder = new PndMvdRiemannTrackFinderTask();
  mvdTrackFinder->AddHitBranch("MVDHitsPixelMix");
  mvdTrackFinder->AddHitBranch("MVDHitsStripMix");
  mvdTrackFinder->SetVerbose(iVerbose);
  mvdTrackFinder->SetMaxDist(0.05);
  fRun->AddTask(mvdTrackFinder);

  // =========================================================================

  //  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0, false, true);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHitMix", "STTPoint");
  fRun->AddTask(sttFindTracks);
  
  PndSttMvdTracking *  SttMvdTracking = new PndSttMvdTracking(0,false,true);
  SttMvdTracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  fRun->AddTask(SttMvdTracking);
 
  PndSttMvdGemTracking * SttMvdGemTracking = new PndSttMvdGemTracking(0);
  fRun->AddTask(SttMvdGemTracking);
  
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("SttMvdGemTrack");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetNumIterations(3);
  fRun->AddTask(recoKalman);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(6);
  fRun->Init();
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
