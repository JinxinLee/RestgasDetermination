{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 1000; // process all events in input file

  // Output file
  TString outFile = "testgenfit.root";
  
  TString inFile  = "testrun.root";
  TString inFile2 = "testdigi.root";
  TString inFile3 = "testreco.root";
  TString parFile = "testparams.root"; 
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  gSystem->Load("libStt");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libSttReco");
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reco run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->AddFriend(inFile2);
  fRun->AddFriend(inFile3);

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  CbmGeane *Geane = new CbmGeane(inFile);

  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  // ------------------------------------------------------------------------
  // -----  Reco Sequence   --------------------------------------------

  SttPatternRecoTask* STTPR = new SttPatternRecoTask();
  fRun->AddTask(STTPR);
  
  SttKalmanTask* RecoHitPro = new SttKalmanTask();
  RecoHitPro->AddHitBranch(3,"STTHit");
  fRun->AddTask(RecoHitPro); 

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  rtdb->print();
  Geane->SetField(fRun->GetField());

  fRun->Run(0,nEvents);
  
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
