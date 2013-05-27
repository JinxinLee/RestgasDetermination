{
  // ========================================================================
  //          Adjust this part according to your requirements
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug, 4=visualization on)
  Int_t iVerbose = 1;
  
  // Input file (MC events)
  TString inFile = "testrun.root";
  TString inFile2 = "testdigi.root";
  
  // Number of events to process
  Int_t nEvents = 0;
  
  // Parameter file
  TString parFile = "testparams.root";
  
  // Output file
  TString outFile = "testreco.root";
  
  //   // ----  Load libraries   ------------------------------------------------
  //   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  //   rootlogon();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(inFile2);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << "@@@@@@@@@@@@@@@@@@@@@@@ WARNING: @@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << "@@                                                      @@" << endl;
  cout << "@@ this is an old version of the reconstruction code.   @@" << endl;
  cout << "@@              You should not use this!                @@" << endl;
  cout << "@@                                                      @@" << endl;
  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;



  // TRACK FINDING =============================================================================
  // trackfinding ....
  PndTrkTracking* tracking = new PndTrkTracking(0,false,false,false);
  // tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
  tracking->NoMvdAloneTracking();
  fRun->AddTask(tracking);
  
  PndMCTrackAssociator* trackMC = new PndMCTrackAssociator();
  trackMC->SetTrackInBranchName("SttMvdTrack");
  trackMC->SetTrackOutBranchName("SttMvdTrackID");
  fRun->AddTask(trackMC);

 

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  // fRun->Run(15, 20);
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
