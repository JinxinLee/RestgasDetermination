{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 0; // process all events in input file

  // Output file
  TString outFile = "testgenfit.root";
  
  TString inFile  = "testrun.root";
  TString inFile2 = "testdigi.root";
  TString inFile3 = "testreco.root";
  TString parFile = "testparams.root"; 
  
//   // ----  Load libraries   -------------------------------------------------
//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//   rootlogon();
//   // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reco run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->AddFriend(inFile2);
  fRun->AddFriend(inFile3);

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
   // ------------------------------------------------------------------------

  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  // ------------------------------------------------------------------------
  // -----  Reco Sequence   --------------------------------------------
  int whichhit = 2; // 1 = use hit; 2 = use helixhit 

  if(whichhit == 1){
    // use hit
    PndSttPatternRecoTask* STTPR = new PndSttPatternRecoTask(iVerbose);
  }
  else if(whichhit == 2){
    // use helixhit
    PndSttPatternRecoTask2* STTPR = new PndSttPatternRecoTask2(iVerbose);
  }

  fRun->AddTask(STTPR);
  
  if(whichhit == 1){
    // use hit
    PndSttKalmanTask* RecoHitPro = new PndSttKalmanTask();
   }
  else if(whichhit == 2){
    // use helixhit
    PndSttKalmanTask2*RecoHitPro = new PndSttKalmanTask2();
   }
  
  fRun->AddTask(RecoHitPro); 

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  rtdb->print();

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
