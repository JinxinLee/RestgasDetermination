void digi()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits 

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "data/sim.root";
  
  // Parameter file
  TString parFile = "data/simparams.root"; // at the moment you do not need it
  
  // Digitisation file (ascii)
  TString digiFile = "all.par";
  
  // Output file
  TString outFile = "data/digi.root";
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);  
  
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
  
  // -----   STT digi producers   ---------------------------------
  //PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  //fRun->AddTask(sttHitProducer);
  
  // -----   MDV digi producers   ---------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);

  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  // -----   EMC hit producers   ---------------------------------
  //PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
  //PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
  //emcHitsToWaveform->SetStorageOfData(kFALSE);
  ////emcWaveformToDigi->SetStorageOfData(kFALSE);
  //fRun->AddTask(emcHitsToWaveform);  // full digitization
  //fRun->AddTask(emcWaveformToDigi);  // full digitization

  //PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  //fRun->AddTask(emcMakeCluster);

  //PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  //fRun->AddTask(emcMakeBump);

  //PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  //fRun->AddTask(emcHdrFiller); // ECM header

  // -----   SciT hit producers   ---------------------------
  //PndSciTHitProducerIdeal* tofhit = new PndSciTHitProducerIdeal();
  //tofhit->SetVerbose(iVerbose);
  //fRun->AddTask(tofhit);

  // -----   MDT hit producers   ---------------------------------
  //PndMdtHitProducerIdeal* mdtHitProd = new PndMdtHitProducerIdeal();
  //mdtHitProd->SetPositionSmearing(.3); // position smearing [cm]
  //fRun->AddTask(mdtHitProd);

  //PndMdtTrkProducer* mdtTrkProd = new PndMdtTrkProducer();
  //fRun->AddTask(mdtTrkProd);

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
  PndFtsHitProducerRealFull* ftsHitProducer = new PndFtsHitProducerRealFull(); // ??? is it "full" ?
//  PndFtsHitProducerRealFast* ftsHitProducer = new PndFtsHitProducerRealFast(); // hits have center of straws as positions
//  PndFtsHitProducerMcPointCoordinates* ftsHitProducer = new PndFtsHitProducerMcPointCoordinates(); // hits have MC truth positions
  ftsHitProducer->SetVerbose(4);
  //ftsHitProducer->SwitchOnOverlap();
  fRun->AddTask(ftsHitProducer);

  // -----   Ftof hit producers   ---------------------------
  PndFtofHitProducerIdeal* ftofhit = new PndFtofHitProducerIdeal();
  ftofhit->SetVerbose(iVerbose);
  fRun->AddTask(ftofhit);

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
  
  // Send cool info to the Dashboard
  cout << "<DartMeasurement name=\"Time\" type=\"numeric/double\">";
  cout << rtime;
  cout << "</DartMeasurement>" << endl;
  Float_t cpuUsage=ctime/rtime;
  cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
  cout << cpuUsage;
  cout << "</DartMeasurement>" << endl;
  FairSystemInfo sysInfo;
  Float_t maxMemory=sysInfo.GetMaxMemory();
  cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
  cout << maxMemory;
  cout << "</DartMeasurement>" << endl;
  // done sending Info
  
  // ------------------------------------------------------------------------
  cout << "Macro finished successfully." << endl;

  exit(0);
}
