{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "points_sttcombi.root";

  // Parameter file
  TString parFile = "params_sttcombi.root";

  // Output file
  TString outFile = "tracks_sttcombi.root";

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  // ------------------------------------------------------------------------

  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------

  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = "../params/all.par";

  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
	
  CbmParAsciiFileIo* parIo1 = new CbmParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----   STT analysis tasks   --------------------------------------------
  // digitize ....

  //PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);

  // trackfinding ....
  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "CbmTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);

  // trackmatching ....
  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttTrackMatcher);

  // trackfitting ....
  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(0);
  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "CbmTask", sttTrackFitter);
  sttFitTracks->AddHitCollectionName("STTHit");
  fRun->AddTask(sttFitTracks);
  
  // helix hit production ....
  PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
  fRun->AddTask(sttHHProducer);
  
  // -----   MDV digi producers   ---------------------------------
  // DIGI
  // double   topPitch=0.015,//cm
  //   botPitch=0.015,//cm
  //   orient=TMath::Pi()*(0.5),
  //   skew=TMath::Pi()*(0.5);
  // TVector2 topAnchor(0.,0.);
  // TVector2 botAnchor(0.,0.);
  // int      topFE = 10,
  //   botFE = 4,
  //   nrFEChannels = 128;
  // double   threshold=3000., noise=1000.;

  // PndMvdStripHitProducer* mvdHitProd
  // = new PndMvdStripHitProducer(topPitch, botPitch,
  //                         orient, skew,
  //                         topAnchor,botAnchor,
  //                         topFE, botFE, nrFEChannels,
  //                         threshold, noise);
  PndMvdStripHitProducer* mvdHitProd = new PndMvdStripHitProducer();
  mvdHitProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdHitProd);

  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);

  // CLUST
  // Cluster finding for strip detectors
  Double_t chargecut = 5000.;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut, inFile);
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  // Cluster finder for pixel detectors
  PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(1.8, inFile);
  mvdClusterizer->SetVerbose(iVerbose);
  fRun->AddTask(mvdClusterizer);

  // -----   EMC hit producers   ---------------------------------
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd); // hit production 

  PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
  //fRun->AddTask(emcMakeDigi); // fast digitization

  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization

  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  fRun->AddTask(emcMakeCluster);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  fRun->AddTask(emcMakeBump);

  PndEmcMakeRecoHit* emcMakeRecoHit= new PndEmcMakeRecoHit();
  fRun->AddTask(emcMakeRecoHit);

  // -----   TOF hit producers   ---------------------------------

  PndTofHitProducerIdeal* tofhit = new PndTofHitProducerIdeal();
  tofhit->SetVerbose(iVerbose);
  fRun->AddTask(tofhit);
 
  // -----   MDT hit producers   ---------------------------------
  PndMdtHitProducerIdeal* mdtHitProd = new PndMdtHitProducerIdeal();
  mdtHitProd->SetPositionSmearing(0.2); // position smearing [cm]
  fRun->AddTask(mdtHitProd);
 
  PndMdtTrkProducerIdeal* mdtTrkProd = new PndMdtTrkProducerIdeal();
  fRun->AddTask(mdtTrkProd);

  // -----   DRC hit producers   ---------------------------------
  PndDrcHitProducerIdeal* drchit = new PndDrcHitProducerIdeal();
  drchit->SetVerbose(iVerbose);
  fRun->AddTask(drchit);

// -----   LHETRACK  ---------------------------------

  PndTpcLheHitsMaker* trackMS = new PndTpcLheHitsMaker("Tracking routine");
  trackMS->SetSttMode(3);  // 0 OFF, 1 SttPoint, 2 SttHit, (3) SttHelixHit // SttPoint smearing [cm], if negative no smearing
  trackMS->SetMvdMode(2);  // 0 OFF, 1 MVDPoint, 2 MVDHit     // MVDPoint smearing [cm], if negative no smearing
  fRun->AddTask(trackMS);

  PndTpcLheTrackFinder* trackFinder    = new PndTpcLheTrackFinder();
  fRun->AddTask(trackFinder);

  PndTpcLheTrackFitter* trackFitter    = new PndTpcLheTrackFitter("fitting");
  fRun->AddTask(trackFitter);

  PndLhePidMaker* pidMaker    = new PndLhePidMaker("pid");
  pidMaker->SetDebugMode(kTRUE);
  fRun->AddTask(pidMaker);

  // -----   Intialise and run   --------------------------------------------
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
