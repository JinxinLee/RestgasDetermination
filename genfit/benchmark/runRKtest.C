// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  gROOT->Reset();

  TString inFile  = "tpcMC.root";
  TString outFile = "geane.root";
  TString parFile = "tpcMC.param.root"; 

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libbenchmark");
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile); 
  fRun->SetOutputFile(outFile);

  RKtestTask *RKT = new RKtestTask();

  fRun->AddTask(RKT);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  // -----  Parameter database   --------------------------------------------

  fRun->Init();
  rtdb->print();
  RKT->setField(new PndFieldAdaptor(fRun->GetField()));


  // Transport nEvents
  // -----------------
  TStopwatch timer;
  timer.Start();
    
  fRun->Run(0,1);
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}  
  
