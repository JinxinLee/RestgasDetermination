{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Number of events to process
  Int_t nEvents = 20; // process all events in input file
  // Base File name
  TString base="demo";

  // Input file (MC events)
  TString inFile = base+".mc.root";
  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".mcreco.root";

  

// ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libgeant321");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");
  
  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");

  
  gSystem->Load("libMvd");

gSystem->ListLibraries();



// ------------------------------------------------------------------------

// -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reco run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  CbmGeane *Geane = new CbmGeane(inFile);

  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  rtdb->Print();
  
  // Set the field(if any) to Geane
    Geane->SetField(fRun->GetField());
       
  // ------------------------------------------------------------------------
  // -----  Reco Sequence   --------------------------------------------

  DemoPatternRecoTask* DemoPR = new DemoPatternRecoTask();
  DemoPR->AddHitBranch(2,"PndTpcPoint");
//  DemoPR->AddHitBranch(3,"MVDPoint");
  DemoPR->SetPersistence();
DemoPR->UseGeane();
  fRun->AddTask(DemoPR);

  DemoKalmanTask* DemoKalman = new DemoKalmanTask();
  DemoKalman->AddHitBranch(2,"PndTpcPoint");
//  DemoKalman->AddHitBranch(3,"MVDPoint");
  DemoKalman->SetPersistence();
//  DemoKalman->SetSmooth(true);
  fRun->AddTask(DemoKalman);

 

  DemoToolsTask* DemoTools = new DemoToolsTask();
//fRun->AddTask(DemoTools);
 

// ----------------------------

//  V0Selector* V0Sel2 = new V0Selector();
//  V0Sel2->SetTrackBranchName("Track");
//  V0Sel2->SetV0BranchName("Lambda");
//  V0Sel2->SetPositivePartMass(0.938272);
//  V0Sel2->SetNegativePartMass(0.13957);
//  V0Sel2->SetPersistence();
//fRun->AddTask(V0Sel2);


//  LambdaStatTask* lambdaStat = new LambdaStatTask();
//  lambdaStat->SetPersistence();
//fRun->AddTask(lambdaStat);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  rtdb->print();
  Geane->SetField(fRun->GetField());

  fRun->Run(0,nEvents);

  DemoKalman->WriteHistograms();
//DemoTools->WriteHistograms();

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
