void run_rec_alldet()
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inFile = "pi_sttcombi.root";

  // Parameter file
  TString parFile = "params_sttcombi.root";

  // Output file
  TString outFile = "pi_reco.root";

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
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
  TString allDigiFile = sysFile+"/macro/params/all.par";

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

  //PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
  //fRun->AddTask(emcMakeDigi); // fast digitization

  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization

  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  fRun->AddTask(emcMakeCluster);

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  fRun->AddTask(emcMakeBump);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header
  
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


 // ------------------------------------------------------------------------
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
  pidMaker->SetGeanePro(kFALSE);  // Switch ON Geane propagation
  pidMaker->SetDebugMode(kTRUE);  // Debug ntuples
  fRun->AddTask(pidMaker);

  PndGpidTaskLhe* pid = new PndGpidTaskLhe();


  pid->SetAPPNAME("testp_1_2");
  pid->SetDIR("/home/vanni/work/pid_ana/weights/");
  pid->SetMVA(PndGpidTask::KNN);

//  fRun->AddTask(pid);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
//  fRun->Run(2500, 2700);

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

