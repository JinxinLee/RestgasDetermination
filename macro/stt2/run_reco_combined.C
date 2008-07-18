{
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
//  TString inFile = "auaumbias.root";
  TString inFile = "test.root";
//  TString inFile = "fourparticle_fieldon.root";
  //  TString inFile = "correct_geom_4proton_1gev_isotropic.root";
  //  TString inFile = "correct_geom_2piplus_2pimin_1gev_isotropic.root";
  // Number of events to process
  Int_t nEvents = 50;

  // Parameter file
  TString parFile = "parfiles/params.root";

  // Output file
  TString outFile = "auaumbias.reco.root";





  // In general, the following parts need not be touched
  // ========================================================================

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libStt2");
  gSystem->Load("libTst");
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun = new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----   Magnetic field   -------------------------------------------
//  CbmConstField *fMagField = new CbmConstField();
//  fMagField->SetFieldXYZ(0, 20, 0); // values are in kG
//  fMagField->SetFieldRegions(-100., -100., -200., 100., 100., 200); // values in cm
//  fRun->SetField(fMagField);
  // ------------------------------------------------------------------------

  // -----   STT analysis tasks   --------------------------------------------
  // digitize ....

  CbmSttHitProducerIdeal* sttHitProducer = new CbmSttHitProducerIdeal();
  fRun->AddTask(sttHitProducer);

  CbmTstHybridHitProducer* tstHitProducer = new CbmTstHybridHitProducer(0.05,0.4, 0);
  fRun->AddTask(tstHitProducer);

  // trackfinding ....
  //CbmSttTrackFinderIdeal* sttTrackFinder = new CbmSttTrackFinderIdeal(iVerbose);
  CbmSttTrackFinderHough* sttTrackFinder = new CbmSttTrackFinderHough(iVerbose);
  CbmSttFindTracks* sttFindTracks = new CbmSttFindTracks("Track Finder", "CbmTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  sttFindTracks->AddHitCollectionName("TSTHit", "TSTPoint");
  // fRun->AddTask(sttFindTracks);

  // trackmatching ....
  CbmSttMatchTracks* sttTrackMatcher = new CbmSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  sttTrackMatcher->AddHitCollectionName("TSTHit", "TSTPoint");
//  fRun->AddTask(sttTrackMatcher);  

  // trackfitting ....
  //CbmSttTrackFitter* sttTrackFitter = new CbmSttMinuitTrackFitter();
  //CbmSttFitTracks* sttFitTracks = new CbmSttFitTracks("STT Track Fitter", "CbmTask", sttTrackFitter);
  //fRun->AddTask(sttFitTracks);

  // quality check ....
  CbmSttFindTracksQa* sttFindQa = new CbmSttFindTracksQa(6, 0.7, kTRUE);
  sttFindQa->SetVerbose(iVerbose);
  //fRun->AddTask(sttFindQa);

  // plot resolutions, efficiencies ....fitterView1.C
  //CbmSttFitTracksQa* sttFitQa = new CbmSttFitTracksQa(1);
  //fRun->AddTask(sttFitQa);

  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(16, nEvents);
  // ------------------------------------------------------------------------

  //sttFitQa->WriteHistograms();
  sttFindQa->WriteHistograms();

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
