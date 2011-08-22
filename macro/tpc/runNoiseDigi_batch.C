void runNoiseDigi_batch(TString filename, double occ, double mean,
			Int_t nEvents = 0) { //mean: mean noise cl amplitude (keV)


// ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
 
  TString mcMode = "TGeant3";
  // ----  Load libraries   -------------------------------------------------
  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  TString inFile = filename;
  TString outFile = filename;
  outFile.ReplaceAll(".mc.root", "");
  outFile.Append("_occ");
  char bufferocc[5];
  sprintf(bufferocc, "%.2f", occ);
  outFile.Append(bufferocc);
  outFile.Append("_mean");
  char buffermean[5];
  sprintf(buffermean, "%.2f", mean);
  outFile.Append(buffermean);
  outFile.Append(".raw.root");
  TString parFile = inFile;
  parFile.ReplaceAll("mc.root", "param.root");
  
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

   // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------

  // -----   TPC digi producers   ---------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
  if(mcMode=="TGeant3") tpcClusterizer->SetMereChargeConversion();
  //tpcClusterizer->SetPersistence();
  fRun->AddTask(tpcClusterizer);
  
  PndTpcNoiseTask* tpcNoise = new PndTpcNoiseTask();
  tpcNoise->setOccupancy(occ);
  tpcNoise->setMeanEnergy(mean);
  //tpcNoise->SetPersistence();
  fRun->AddTask(tpcNoise);
  
  PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
  tpcDrifter->SetPersistence();
  tpcDrifter->SetDistort(false);
  fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
  //tpcGem->SetPersistence();
  //fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  //tpcPadResponse->SetPersistence();
  fRun->AddTask(tpcPadResponse);

  PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  tpcElec->SetPersistence();
  tpcElec->SetSamplePersistence();
  fRun->AddTask(tpcElec);

  PndTpcEvtTimeGenTask* evttimegen = new PndTpcEvtTimeGenTask();
  evttimegen->SetPersistence();
  evttimegen->SetEvtRate(1E7);   
  evttimegen->SetT0(0);
  fRun->AddTask(evttimegen);
  
  //PndTpcDigiAmpCorrectionTask* digiCorr = new PndTpcDigiAmpCorrectionTask();
  //digiCorr->SetExternalInput("gain_corrected.csv");
  //fRun->AddTask(digiCorr);

    
  // -----   MDV digi producers   --------------------------------- 
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  //fRun->AddTask(mvddigi);

  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  //fRun->AddTask(mvdmccls); 
  
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
