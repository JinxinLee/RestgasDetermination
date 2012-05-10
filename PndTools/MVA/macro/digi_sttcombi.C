void digi_sttcombi(char inFile  [] = "points_sttcombi.root", //Input file (MC events)
                   char parFile [] = "params_sttcombi.root",//Parameter file
                   char outFile [] = "digi_sttcombi.root"  //Output file
                   )

{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

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
  FairRunAna* fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile);
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile,"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------

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

  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  emcHitProd->SetStorageOfData(kTRUE);
  fRun->AddTask(emcHitProd);  
  
  //PndEmcFullDigiTask* fdig = new PndEmcFullDigiTask(0,kTRUE);
  //fdig->SetStorageOfData(kTRUE);
  //fRun->AddTask(fdig);

  //PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
  //fRun->AddTask(emcMakeDigi); // fast digitization

  PndEmcHitsToWaveform* emcHitsToWaveform = new PndEmcHitsToWaveform(iVerbose);
  PndEmcWaveformToDigi* emcWaveformToDigi = new PndEmcWaveformToDigi(iVerbose);

  emcHitsToWaveform->SetStorageOfData(kTRUE);
  emcWaveformToDigi->SetStorageOfData(kTRUE);
    
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization

  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  emcMakeCluster->SetStorageOfData(kTRUE); 
  fRun->AddTask(emcMakeCluster);

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  emcMakeBump->SetStorageOfData(kTRUE);
  fRun->AddTask(emcMakeBump);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header

  PndEmcMakeRecoHit* emcMakeRecoHit= new PndEmcMakeRecoHit();
  emcMakeRecoHit->SetStorageOfData(kTRUE);
  fRun->AddTask(emcMakeRecoHit);
  
  PndEmcExpClusterSplitter* clspl = new PndEmcExpClusterSplitter();
  clspl->SetStorageOfData(kTRUE);
  fRun->AddTask(clspl);
  
  PndEmc2DLocMaxFinder* localMaxF = new PndEmc2DLocMaxFinder();
  localMaxF->SetStorageOfData(kTRUE);
  fRun->AddTask(localMaxF);
  
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
  Int_t verboseLevel = 0;
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", verboseLevel);
  fRun->AddTask(gemDigitize);

  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", verboseLevel);
  fRun->AddTask(gemFindHits);

  // -----   FTS hit producers   ---------------------------------
  PndFtsHitProducerRealFast* ftsHitProducer = new PndFtsHitProducerRealFast();
  //PndFtsHitProducerIdeal* ftsHitProducer = new PndFtsHitProducerIdeal();
  //PndFtsHitProducerRealFull* ftsHitProducer = new PndFtsHitProducerRealFull();
  fRun->AddTask(ftsHitProducer);
  
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
  cout << "\n\nMacro finished succesfully.\n"
       << "Output file is "    << outFile
       << "\nParameter file is " << parFile
       << "\nReal time " << rtime << " s, CPU time " << ctime << " s.\n\n";
  // ------------------------------------------------------------------------

  exit(0);
}
