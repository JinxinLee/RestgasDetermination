{
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  gSystem->Load("libjobdb");
  // ------------------------------------------------------------------------

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  
  TString inFile = "tutorial.mc.root";
  TString outFile = inFile; 
  outFile.ReplaceAll(".mc.root",".raw.root");

  TString param = inFile;
  param.ReplaceAll(".mc.root",".param.root");
  TString paramOut = inFile;
  paramOut.ReplaceAll(".mc.root",".param2.root");

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
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(param.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/tpc.par";
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);

  PndTpcDigiPar* par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  par->setInputVersion(fRun->GetRunId(),1);
  par->setChanged(kTRUE);

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  fRun->LoadGeometry();

  // ------------------------------------------------------------------------
  // ----- Tpc Standard Digi Sequence  --------------------------------------
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


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

  fRun->Run(0,0); // process all events from input file
  // ------------------------------------------------------------------------


  tpcDrifter->WriteHistograms();
  tpcPadResponse->WriteHistograms();

  // -----   Finish   -------------------------------------------------------


  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
