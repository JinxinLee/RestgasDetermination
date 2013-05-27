{
  // ========================================================================
  //          Adjust this part according to your requirements
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug, 4=visualization on)
  Int_t iVerbose = 1;
  
  // Input file (MC events)
  TString inFile = "testrun.root";
  TString inFile2 = "testdigi.root";
  
  // Number of events to process
  Int_t nEvents = 0;
  
  // Parameter file
  TString parFile = "testparams.root";
  
  // Output file
  TString outFile = "testreco.root";
  
  //   // ----  Load libraries   ------------------------------------------------
  //   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  //   rootlogon();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(inFile2);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << "@@@@@@@@@@@@@@@@@@@@@@@ WARNING: @@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << "@@                                                      @@" << endl;
  cout << "@@ this is an old version of the reconstruction code.   @@" << endl;
  cout << "@@              You should not use this!                @@" << endl;
  cout << "@@                                                      @@" << endl;
  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;



  // TRACK FINDING =============================================================================
  // OUTPUT: PndTrackCand                    -> STTTrackCand
  //         PndSttHelixHit (if switched on) -> STTPRHelixHit
  // trackfinding ....
  //  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  //  sttFindTracks->SetHelixHitProduction(); // if you want helix hit to be produced by PR uncomment this (STTPRHelixHit)
  fRun->AddTask(sttFindTracks);

  // trackmatching ....
  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttTrackMatcher);  

  // TRACK FITTING =============================================================================
  // OUTPUT: PndSttTrack                    -> STTTrack
  //         PndSttHelixHit                 -> SttHelixHit
  // trackfitting ....
  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(iVerbose);
  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter); 
  sttFitTracks->AddHitCollectionName("STTHit");
  fRun->AddTask(sttFitTracks);

  // helix hit production (this task only works if called after PndSttFitTracks!) ....
  PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
  fRun->AddTask(sttHHProducer);
 
  // QA plots if you want them
  //  PndSttTrackFitterQATask* qaFit = new PndSttTrackFitterQATask();
  //  fRun->AddTask(qaFit);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  // fRun->Run(15, 20);
  // ------------------------------------------------------------------------

  // sttHHProducer->WriteHistograms();
  // qaFit->WriteHistograms();

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
