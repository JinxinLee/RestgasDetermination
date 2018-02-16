int reco(Int_t nEvents = 5)
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a digi file and performs tracking

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment

  // Number of events to process
  //Int_t nEvents = 0;  // if 0 all the vents will be processed

  // Parameter file
  TString parFile = "data/simparams.root"; // at the moment you do not need it

  // Digitisation file (ascii)
  //TString digiFile = "all.par";
  //TString digiFile = "ftsInitial.par";
  TString digiFile = "fts.par";

  // Output file
  TString outFile = "data/reco.root";

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("data/sim.root");
  fRun->AddFriend("data/digi.root");
  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  PndFtsHoughTrackerTask* trackFts = new PndFtsHoughTrackerTask();
  trackFts->SetTrackOutput("FtsHoughTrack");
  trackFts->SetPersistence(kTRUE);
  Int_t debugLevel = 1;

  debugLevel *= PndFtsHoughTrackerTask::kHoughSpaces; // sed -i 's/PndFtsHoughSpace/TH2S/g' *histo*.rtg
  debugLevel *= PndFtsHoughTrackerTask::kAllFoundPeaksTogether;
  debugLevel *= PndFtsHoughTrackerTask::kEachFoundPeakSeparately;
  debugLevel *= PndFtsHoughTrackerTask::kMcTruthPeaksExclusively;
  debugLevel *= PndFtsHoughTrackerTask::kMcTruthPeaksProjected;
  debugLevel *= PndFtsHoughTrackerTask::kHitCurvesExclusively;
  debugLevel *= PndFtsHoughTrackerTask::kHitCurvesProjected;
  trackFts->SetSaveDebugInfo(debugLevel); // Make sure that /home/plots is writable for output of debugging histograms
  trackFts->SetVerbose(0);
  fRun->AddTask(trackFts);

  //PndMCTrackAssociator* trackMCfwd = new PndMCTrackAssociator();
  //trackMCfwd->SetTrackInBranchName("FtsHoughTrack");
  //trackMCfwd->SetTrackOutBranchName("FtsHoughTrackID");
  //fRun->AddTask(trackMCfwd);

  //PndRecoKalmanTask* recoKalmanFwd = new PndRecoKalmanTask();
  //recoKalmanFwd->SetTrackInBranchName("FtsHoughTrack");
  //recoKalmanFwd->SetTrackInIDBranchName("FtsHoughTrackID");
  //recoKalmanFwd->SetTrackOutBranchName("FtsHoughGenTrack");
  ////recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  ////recoKalmanFwd->SetIdealHyp(kTRUE);
  //recoKalmanFwd->SetNumIterations(3);
  //recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
  ////recoKalmanFwd->SetPropagateToIP(kFALSE);
  //fRun->AddTask(recoKalmanFwd);

  //PndMCTrackAssociator* trackMC3 = new PndMCTrackAssociator();
  //trackMC3->SetTrackInBranchName("FtsHoughGenTrack");
  //trackMC3->SetTrackOutBranchName("FtsHoughGenTrackID");
  //fRun->AddTask(trackMC3);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  cout << "fRun->Init()" << endl;
  fRun->Init();

  timer.Start();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  
  // Send cool info to the Dashboard
  cout << "<DartMeasurement name=\"Time\" type=\"numeric/double\">";
  cout << rtime;
  cout << "</DartMeasurement>" << endl;
  Float_t cpuUsage=ctime/rtime;
  cout << "<DartMeasurement name=\"CpuLoad\" type=\"numeric/double\">";
  cout << cpuUsage;
  cout << "</DartMeasurement>" << endl;
  FairSystemInfo sysInfo;
  Float_t maxMemory=sysInfo.GetMaxMemory();
  cout << "<DartMeasurement name=\"MaxMemory\" type=\"numeric/double\">";
  cout << maxMemory;
  cout << "</DartMeasurement>" << endl;
  // done sending Info

  // ------------------------------------------------------------------------
  cout << "Macro finished successfully." << endl;
  //return 0;
  return 0;
}
