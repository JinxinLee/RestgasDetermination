{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents = 0;
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  // ------------------------------------------------------------------------
  // Output file
  TString parFile = "params_tpccombi.root";
  TString inSimuFile = "points_tpccombi.root";
  TString inPidFile = "pid_tpccombi.root";
  TString outFile = "algo_tpccombi.root";
   
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inPidFile);
  fRun->AddFriend(inSimuFile);
  fRun->SetOutputFile(outFile.Data());
  
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  // ------------------------------------------------------------------------
  
  PndPidIdealAssociatorTask *assMC= new PndPidIdealAssociatorTask();
  fRun->AddTask(assMC);
  
  PndPidMvdAssociatorTask *assMvd= new PndPidMvdAssociatorTask();
  fRun->AddTask(assMvd);
  
  PndPidDrcAssociatorTask *assDrc= new PndPidDrcAssociatorTask();
  fRun->AddTask(assDrc);
  
  // -----   Intialise and run   --------------------------------------------
  //  PndEmcMapper::Init(6);
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
  rtdb->print();
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
