int idealcomplete(int level = 2){
  // ========================================================================
  // "Ideality" level: 1 = real  digi + ideal track finding + real fitting
  //                   2 = ideal digi + ideal track finding + real fitting
  if(level == 1)      cout << "real  digi + ideal track finding + real fitting" << endl;
  else if(level == 2) cout << "ideal  digi + ideal track finding + real fitting" << endl;
  else {
    cout << "this level is not implemented, choose 1 or 2:" << endl;
    cin >> level;
  }

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  
  // Input file (MC events)
  TString inFile = "testrun.root";
  
  // Number of events to process
  Int_t nEvents = 0;
  
  // Parameter file
  TString parFile = "testparams.root";
  
  // Output file
  TString outFile = "testideal.root";
  
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
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);


  // ------------------------------------------------------------------------
  // ideal digitization .... the drift radius is the real distance particle
  // trajectory - wire
  if(level == 1) PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  else if(level == 2) PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
  else cout << "this level is not implemented, choose 1 or 2" << endl;
  fRun->AddTask(sttHitProducer);
  // ------------------------------------------------------------------------

  // ------------------------------------------------------------------------ 
  // ideal trackfinding .... 
  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);

  // ideal trackmatching ....
  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttTrackMatcher);  

  // real trackfitting .... but on ideal radii -> the smearing
  // in the momentum distribution is only due to the algorithm
  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(iVerbose);
  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter); 
  sttFitTracks->AddHitCollectionName("STTHit");
  fRun->AddTask(sttFitTracks);

  // real helix hit production .... 
  PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
  fRun->AddTask(sttHHProducer);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  // fRun->Run(15, 20);
  // ------------------------------------------------------------------------

  //  sttHHProducer->WriteHistograms();

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
  return 0;
}
