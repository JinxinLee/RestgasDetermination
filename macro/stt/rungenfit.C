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
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libTrkBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
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
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->AddFriend(inFile2);
  fRun->AddFriend(inFile3);

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  FairGeane *Geane = new FairGeane(inFile);

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
    PndSttPatternRecoTask* STTPR = new PndSttPatternRecoTask();
  }
  else if(whichhit == 2){
    // use helixhit
    PndSttPatternRecoTask2* STTPR = new PndSttPatternRecoTask2();
  }

  fRun->AddTask(STTPR);
  
  if(whichhit == 1){
    // use hit
    PndSttKalmanTask* RecoHitPro = new PndSttKalmanTask();
    RecoHitPro->AddHitBranch(3,"STTHit");
   }
  else if(whichhit == 2){
    // use helixhit
    PndSttKalmanTask2*RecoHitPro = new PndSttKalmanTask2();
    RecoHitPro->AddHitBranch(3,"SttHelixHit");
   }
  
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
