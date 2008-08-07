{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "points_sttmvd.root";

  // Parameter file
  TString parFile = "testparams.root";

  // Output file
  TString outFile = "tracks_sttmvd.root";

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
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
  // 			     orient, skew,
  // 			     topAnchor,botAnchor,
  // 			     topFE, botFE, nrFEChannels,
  // 			     threshold, noise);
  PndMvdStripHitProducer* mvdHitProd = new PndMvdStripHitProducer();
  mvdHitProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdHitProd);

  Double_t  lx=0.01, ly=0.01, threshold=600, noise=200;
  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer(lx,ly,threshold,noise);
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);
 
  // CLUST
  // Cluster finding for strip detectors
  Double_t noise = 1000.; // put such stuff inside the task
  Double_t chargecut = 3. * noise;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut, inFile);
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
  
  // Cluster finder for pixel detectors
  PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(1.8,76,84, inFile);
  mvdClusterizer->SetVerbose(iVerbose);
  fRun->AddTask(mvdClusterizer);
 
  // TRACKFINDER
  //PndMvdIdealTrackingTask* mvdmctrk = new PndMvdIdealTrackingTask();
  //mvdmctrk->SetVerbose(iVerbose);
  //fRun->AddTask(mvdmctrk);

  // -----   LHETRACK  ---------------------------------

  PndTpcLheHitsMaker* trackMS = new PndTpcLheHitsMaker("Tracking routine");
  trackMS->SetTpcMode(0);  // 0 OFF, 1 TpcPoint, 2 TpcCluster // TpcPoint smearing [cm], if negative no smearing
  trackMS->SetSttMode(3);  // 0 OFF, 1 SttPoint, 2 SttHit, (3) SttHelixHit // TpcPoint smearing [cm], if negative no smearing
  trackMS->SetMvdMode(2, -1);  // 0 OFF, 1 MVDPoint, 2 MVDHit     // MVDPoint smearing [cm], if negative no smearing
  fRun->AddTask(trackMS);

  PndTpcLheTrackFinder* trackFinder    = new PndTpcLheTrackFinder();
  fRun->AddTask(trackFinder);

  PndTpcLheTrackFitter* trackFitter    = new PndTpcLheTrackFitter("fitting");
  fRun->AddTask(trackFitter);

  PndLhePidMaker* pidMaker    = new PndLhePidMaker("pid");
  fRun->AddTask(pidMaker);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
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
