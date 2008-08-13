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
  TString parFile = "testparams.root";
  TString outFile = "fit_tpcmvd.root";
   
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
  fRun->SetInputFile("points_tpcmvd.root");
  fRun->AddFriend("tracks_tpcmvd.root");
  
  
  fRun->SetOutputFile(outFile.Data());
  // ------------------------------------------------------------------------

  // THIS IS STRONGLY NEEDED
  CbmGeane *Geane = new CbmGeane("points_tpcmvd.root");
  Geane->SetField(fRun->GetField());


  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  //  CbmParAsciiFileIo* parInput1 = new CbmParAsciiFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  /*Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open(parOutFile);
  rtdb->setOutput(output);
*/  fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  PndLheKalmanTask* lheKalman = new PndLheKalmanTask();
  lheKalman->SetVerbose(iVerbose);
  //lheKalman->SetGeane(kTRUE);
  lheKalman->SetSmooth(kTRUE);
  lheKalman->SetNumIterations(3);
  fRun->AddTask(lheKalman);

  // =====                 End of HitProducers                           =====
  // =========================================================================
     PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
  // -----   Intialise and run   --------------------------------------------
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
