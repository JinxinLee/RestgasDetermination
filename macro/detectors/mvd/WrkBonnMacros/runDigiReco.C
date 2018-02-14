int runDigiReco(TString runfiles, Int_t nEvents = 0)
{
  // leave nEvents at 0 to run on the whole data
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
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/macro/params/all.par";
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->SetOutputFile(recoFile);
  
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
    
  
  // Digitization and detector Reconstruction
  // ----------------------------------------

  // -----   MVD digi producers   --------------------------------- 
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);
  
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls); 
  
  // -----   STT digi producers   --------------------------------- 
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);

  // STT alone pattern recognition
  ////PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(iVerbose);
  //PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  //PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  //sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  //sttFindTracks->SetHelixHitProduction();
  //fRun->AddTask(sttFindTracks);
  //PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  //sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  //fRun->AddTask(sttTrackMatcher);
  //PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(0);
  //PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter);
  //sttFitTracks->AddHitCollectionName("STTHit");
  //fRun->AddTask(sttFitTracks);
  //PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
  //fRun->AddTask(sttHHProducer);
  
  // -----   EMC hit producers   ---------------------------------
  // done in sim macro to save disk space
  //PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  //fRun->AddTask(emcHitProd); // hit production, already done in simulation run
  // done in sim macro to save disk space
  //PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
  //fRun->AddTask(emcMakeDigi); // fast digitization
  
  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
  emcHitsToWaveform->SetStorageOfData(kFALSE);
  emcWaveformToDigi->SetStorageOfData(kFALSE);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization
  
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  fRun->AddTask(emcMakeCluster);
  
  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  fRun->AddTask(emcMakeBump);
  
  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header
    
  // -----   MDT hit producers   ---------------------------------
  PndMdtHitProducerIdeal* mdtHitProd = new PndMdtHitProducerIdeal();
  mdtHitProd->SetPositionSmearing(.3); // position smearing [cm]
  fRun->AddTask(mdtHitProd);
  
  PndMdtTrkProducer* mdtTrkProd = new PndMdtTrkProducer();
  fRun->AddTask(mdtTrkProd);
  
  // -----   DRC hit producers   ---------------------------------
  PndDrcHitProducerIdeal* drchit = new PndDrcHitProducerIdeal();
  drchit->SetVerbose(iVerbose);
  fRun->AddTask(drchit);
  
  // -----   GEM hit producers   ---------------------------------
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", iVerbose);
  fRun->AddTask(gemDigitize);
  
  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", iVerbose);
  fRun->AddTask(gemFindHits);
  
  // -----   Intialise and run   --------------------------------------------
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

