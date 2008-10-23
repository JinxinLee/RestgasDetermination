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
  TString parFile = "params_sttcombi.root";
  TString inSimuFile = "points_sttcombi.root";
  TString inDigiFile = "digi_sttcombi.root";
  TString inRecoFile = "reco_sttcombi.root";

  TString outFile = "fit_sttcombi.root";
   
  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inSimuFile);
  fRun->AddFriend(inDigiFile);
  fRun->AddFriend(inRecoFile);
  
  
  fRun->SetOutputFile(outFile.Data());
  // ------------------------------------------------------------------------

  // THIS IS STRONGLY NEEDED
  CbmGeane *Geane = new CbmGeane(inSimuFile);
  PndEmcMapper *emcMap = PndEmcMapper::Instance(2,inSimuFile);

  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();

  // ------------------------------------------------------------------------
  
  PndLheKalmanTask* lheKalman = new PndLheKalmanTask();
  lheKalman->SetVerbose(iVerbose);
  lheKalman->SetGeane(kTRUE);
  lheKalman->SetSmooth(kTRUE);
  lheKalman->SetNumIterations(3);
  fRun->AddTask(lheKalman);
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  Geane->SetField(fRun->GetField());
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
