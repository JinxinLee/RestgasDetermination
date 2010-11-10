void reco_sttcombi(char inDigiFile [] = "digi_sttcombi.root",   //Input file Digi
                   char inSimFile  [] = "points_sttcombi.root", // Input file Sim
                   char parFile    [] = "params_sttcombi.root",     // Parameter file
                   char outFile    [] = "reco_sttcombi.root" // Output file
                   )
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  
  // Input file
  TString inDigiFile = inDigiFile;
  TString inSimFile  = inSimFile;
  
  // Parameter file
  TString parFile = parFile;
  
  // Output file
  TString outFile = outFile;
  
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
  fRun->SetInputFile(inDigiFile);
  fRun->AddFriend(inSimFile);
  fRun->SetOutputFile(outFile);
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  // ------------------------------------------------------------------------
  
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/all.par";
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile);
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile,"in");
  
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------
  // -----   LHETRACK  ---------------------------------
  
  PndLheHitsMaker* trackMS = new PndLheHitsMaker("Tracking routine");
  // 0 OFF, 1 SttPoint, 2 SttHit, (3) SttHelixHit, 4 SttHelixHit MC,
  // 5 SttPRHelixHit // STTPoint smearing [cm], if negative no smearing
  trackMS->SetSttMode(5);
  trackMS->SetMvdMode(2);// 0 OFF, 1 MVDPoint, 2 MVDHit // MVDPoint smearing [cm], if negative no smearing
  trackMS->SetGemMode(2);// 0 OFF, 1 GEMPoint, 2 GEMHit // GEMPoint smearing [cm], if negative no smearing
  fRun->AddTask(trackMS);
  
  PndLheTrackFinder* trackFinder    = new PndLheTrackFinder();
  //PndLheTrackFinderIdeal* trackFinder    = new PndLheTrackFinderIdeal();
  fRun->AddTask(trackFinder);
  
  PndLheTrackFitter* trackFitter    = new PndLheTrackFitter("TrackFitting");
  fRun->AddTask(trackFitter);
  
  
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("LheTrack");
  recoKalman->SetTrackOutBranchName("LheGenTrack");
  // recoKalman->SetNumIterations(3);
  fRun->AddTask(recoKalman);
  
  //////////////////
  /*
    PndRecoMultiKalmanTask* recoKalman = new PndRecoMultiKalmanTask();
    recoKalman->SetTrackInBranchName("LheTrack");
    recoKalman->SetTrackOutBranchName("LheGenTrack");
    fRun->AddTask(recoKalman);
  */
  //////////////////// 
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(6);
  
  fRun->Init();
  fRun->Run(0, nEvents);
  
  rtdb->saveOutput();
  rtdb->print();
  
  // ------------------------------------------------------------------------
  
  // -----   Finish   -------------------------------------------------------
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "\n\nMacro finished succesfully.\n"
       << "Output file is "    << outFile
       << "\nParameter file is " << parFile
       << "Real time " << rtime << " s, CPU time " << ctime << " s.\n\n";
  // ------------------------------------------------------------------------
  exit(0);
}
