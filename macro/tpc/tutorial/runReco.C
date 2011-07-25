{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");
  
  

  // Input file (RAW events)
  TString inFile="tutorial.raw.root";
  TString outFile = inFile; 
  outFile.ReplaceAll(".raw.root",".reco.root");
  TString mcFile = inFile; 
  mcFile.ReplaceAll(".raw.root",".mc.root");

  TString param = outFile;
  param.ReplaceAll(".reco.root",".param2.root");

// -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  

  // -----   Run - setup   ------------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(param.Data());
  rtdb->setFirstInput(parInput1);
  
  rtdb->print();

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(param.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  


   // -----    Reco Sequence  --------------------------------------------
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetPersistence();
  tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
  fRun->AddTask(tpcCF);

  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetPersistence();
  tpcSPR->useGeane(); // use RKTrackrep and GeaneTrackrep
  tpcSPR->SetMCPid(); // use ideal particle identification
  //tpcSPR->SetPDG(211);
  fRun->AddTask(tpcSPR);

  KalmanTask* kalman =new KalmanTask();
  fRun->AddTask(kalman);

  TrackFitStatTask* fitstat=new TrackFitStatTask();
  fitstat->SetPersistence();
  fitstat->SetMCPCut(3); // in sigma dp/p
  fitstat->SetMCCuts(0.05, // pmin
	             10., // pmax
		     -TMath::Pi(),   // thetamin 5deg
		     TMath::Pi(),  // thetamax
		     20); // nPndTpcPoints
  //fitstat->SetPdgSelection(321);
  //fitstat->DoResiduals();
  fRun->AddTask(fitstat);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,0);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------

  DebugLogger::Instance()->WriteFiles();

  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << param << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
