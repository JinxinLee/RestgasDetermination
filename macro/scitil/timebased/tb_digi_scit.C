void tb_digi_scit()
{
  // -----------   User Settings: -------------------------------

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  Bool_t timebased = kTRUE;
  //Bool_t timebased = kFALSE;
  Double_t eventrate = 20.0; // in MHz

  // ---------- SciTHit parameters: ------------------
  Double_t SciTDeadtime = 1000;
  Double_t SciTdt = 0.1;
  // ------------------------

  TString inFile = "sim_scit.root";
  TString parFile = "sim_scit_params.root"; // at the moment you do not need it
  TString digiFile = "all.par";
  TString outFile = "tb_digi_scit.root";
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetUseFairLinks(kTRUE);
  //fRun->SetEventMeanTime(50); //in ns
  if (timebased) fRun->SetEventMeanTime(1/(eventrate * 10e6)*10e9); //in n
 
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  /*
  // -----   STT digi producers   ---------------------------------
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);
  
  // -----   MDV digi producers   ---------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);
 
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  // -----   EMC hit producers   ---------------------------------
  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
  emcHitsToWaveform->SetStorageOfData(kFALSE);
  //emcWaveformToDigi->SetStorageOfData(kFALSE);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization

  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  fRun->AddTask(emcMakeCluster);

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  fRun->AddTask(emcMakeBump);

  //PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  //fRun->AddTask(emcHdrFiller); // ECM header
  */
  // -----   SciT hit producers   ---------------------------
  PndSciTDigiTask* SciTDigi = new PndSciTDigiTask();
  SciTDigi->SetVerbose(iVerbose);
  SciTDigi->SetDeadTime(SciTDeadtime);
  SciTDigi->SetTimeResolution(SciTdt);
  if(timebased) SciTDigi->RunTimeBased();
  fRun->AddTask(SciTDigi);
  /*
  // -----   MDT hit producers   ---------------------------------
  PndMdtHitProducerIdeal* mdtHitProd = new PndMdtHitProducerIdeal();
  mdtHitProd->SetPositionSmearing(.3); // position smearing [cm]
  fRun->AddTask(mdtHitProd);

  PndMdtTrkProducer* mdtTrkProd = new PndMdtTrkProducer();
  fRun->AddTask(mdtTrkProd);

  // -----   DRC hit producers   ---------------------------------
  PndDrcHitProducerReal* drchit = new PndDrcHitProducerReal();
  drchit->SetVerbose(iVerbose);
  fRun->AddTask(drchit);

  // -----   GEM hit producers   ---------------------------------
  Int_t verboseLevel = 0;
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", verboseLevel);
  fRun->AddTask(gemDigitize);

  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", verboseLevel);
  fRun->AddTask(gemFindHits);

  // -----   FTS hit producers   ---------------------------------
  PndFtsHitProducerRealFast* ftsHitProducer = new PndFtsHitProducerRealFast();
  fRun->AddTask(ftsHitProducer);

  // -----   Ftof hit producers   ---------------------------
  PndFtofHitProducerIdeal* ftofhit = new PndFtofHitProducerIdeal();
  ftofhit->SetVerbose(iVerbose);
  fRun->AddTask(ftofhit);
  */
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  timer.Start();
  fRun->Run();

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;

  exit(0);
}
