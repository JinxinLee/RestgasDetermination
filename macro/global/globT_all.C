void globT_all() {
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file
  TString inLocTFile = "locT_global.root";
  TString inDigiFile = "digi_global.root";
  TString inSimFile = "points_global.root";

  // Parameter file
  TString parFile = "params_global.root";

  // Output file
  TString outFile = "globT_global.root";

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inLocTFile);
  fRun->AddFriend(inDigiFile);
  fRun->AddFriend(inSimFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ##################################################################   MERGE
  PndGlobalIdealTrackMerger* trackMerger = new PndGlobalIdealTrackMerger(iVerbose);
  fRun->AddTask(trackMerger);

  PndGlobalTrackMergerQA* trackMergerQA = new PndGlobalTrackMergerQA(iVerbose);
  fRun->AddTask(trackMergerQA);

  // ##################################################################   FIT MERGED TRACKS
  // -----   Prepare Geane   ------------------------------------------------
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----   Run Kalman fitter   --------------------------------------------
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("GlobalTrack");
  recoKalman->SetTrackOutBranchName("GlobalFitTrack");
  //recoKalman->SetNumIterations(3);
  fRun->AddTask(recoKalman);

  PndGlobalTrackFitQA* trackFitQA = new PndGlobalTrackFitQA(0);
  fRun->AddTask(trackFitQA);
  // ##################################################################

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  //  PndEmcMapper *emcMap = PndEmcMapper::Instance(2);
  fRun->Run(0, nEvents);

  rtdb->saveOutput();
  rtdb->print();

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
