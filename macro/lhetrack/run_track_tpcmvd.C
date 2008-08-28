{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "points_tpcmvd.root";

  // Parameter file
  TString parFile = "testparams.root";

  // Output file
  TString outFile = "tracks_tpcmvd.root";

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

  // -----   TPC digi producers   ---------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
  //tpcClusterizer->SetPersistence();
  fRun->AddTask(tpcClusterizer);
 
  PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
  // tpcDrifter->SetPersistence();
  tpcDrifter->SetDistort(false);
  fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
  //tpcGem->SetPersistence();
  fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  tpcPadResponse->SetPersistence();
  fRun->AddTask(tpcPadResponse);

  PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  tpcElec->SetPersistence();
  fRun->AddTask(tpcElec);

  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetPersistence();
  tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
  fRun->AddTask(tpcCF);

  //PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  //tpcSPR->SetTrkFinderParameters(2.,// proxcut
  //	 		       0.02, // proxcut on rieman sphere
  //		 	       2.E-3, // planecut
  //			       4.0, // szcut
  //			       4); // minnumhits for fit
  //tpcSPR->SetPersistence();
  //  fRun->AddTask(tpcSPR);

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

  // -----   LHETRACK  ---------------------------------

  PndTpcLheHitsMaker* trackMS = new PndTpcLheHitsMaker("Tracking routine");
  trackMS->SetTpcMode(2, -1);  // 0 OFF, 1 TpcPoint, 2 TpcCluster // TpcPoint smearing [cm], if negative no smearing
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
