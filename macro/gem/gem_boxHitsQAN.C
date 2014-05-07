Int_t gem_boxHitsQAN(Int_t nStations, Int_t nparts, Int_t nEvents = 1000, Double_t pointEffDist = 0.1, Int_t pdgC = 211, int verboseLevel = 0)
{ 
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return;
  }

  // ----  Load libraries   -------------------------------------------------
  // gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  // Input file (MC events)
  TString baseName;
  baseName.Form("Gem_%dStations_%d_%dpart_n%d",nStations,pdgC,nparts,nEvents);

  TString MCFile  = baseName + ".root";
  TString parFile = baseName + "_par.root";
  TString hitFile = baseName + "_hits.root";
  TString outFile = baseName + Form("_QAhits_e%.1f.root",pointEffDist);
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(hitFile);
  fRun->SetOutputFile(outFile);

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/gem_3Stations.digi.par";
  if ( nStations == 4 ) allDigiFile = sysFile+"/macro/params/gem_4Stations.digi.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------

  // -----   Digitizer and Hit Finder   -------------------------------------
  
  PndGemFindHitsQA* gemFindHitsQA = new PndGemFindHitsQA(verboseLevel);
  gemFindHitsQA->SetPointEffDist(pointEffDist);
  fRun->AddTask(gemFindHitsQA);

  // -----   Ideal Hit Producer   ---------------------------------------
//   PndGemIdealHitProducer* gemHitProducer = new PndGemIdealHitProducer("GEM Ideal hit producer", verboseLevel);
//   fRun->AddTask(gemHitProducer);

/*
  //------ Track finder ------------------------------
  //Create and add finder task
  PndGemFindTracks* finderTask = new PndGemFindTracks("PndGemFindTracks");
  finderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  fRun->AddTask(finderTask);
  
  //------ Ideal Track finder ------------------------
//   PndGemTrackFinderIdeal* mcTrackFinder = new  PndGemTrackFinderIdeal();
//   mcTrackFinder->SetVerbose(verboseLevel);  // verbosity level
//   mcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
//   finderTask->UseFinder(mcTrackFinder);

  //------ Realistic Track finder --------------------
  PndGemTrackFinderOnHits* mcTrackFinder = new  PndGemTrackFinderOnHits();
  mcTrackFinder->SetVerbose(verboseLevel);  // verbosity level
  mcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------
  
  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  //--------------------------------------------------
  
  // -----   Run Kalman fitter   --------------------------------------------
  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
  recoKalman->SetTrackInBranchName("GEMTrack");
  recoKalman->SetTrackOutBranchName("GEMFitTrack");
  //recoKalman->SetNumIterations(3);
  fRun->AddTask(recoKalman);
  // ------------------------------------------------- 

  PndGemTrackFinderQA* trackFinderQA = new PndGemTrackFinderQA();
  trackFinderQA->SetVerbose(verboseLevel);
  fRun->AddTask(trackFinderQA);
*/

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  //  fRun->Run(858,859);

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

}
