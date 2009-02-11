{
  // ========================================================================
  //          Adjust this part according to your requirements
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  
  // Input file (MC events)
  TString inFile = "testrun.root";
  TString inFile2 = "testdigi.root";
  
  // Number of events to process
  Int_t nEvents = 0;
  
  // Parameter file
  TString parFile = "testparams.root";
  
  // Output file
  TString outFile = "testreco.root";
  
  // ----  Load libraries   ------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libStt");

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

  // trackfinding ....
  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);

  // trackmatching ....
  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttTrackMatcher);  

  // trackfitting ....
  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(0);
  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter); 
  sttFitTracks->AddHitCollectionName("STTHit");
  fRun->AddTask(sttFitTracks);

  // helix hit production ....
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
}
