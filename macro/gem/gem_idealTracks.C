// Macro created by Radoslaw Karabowicz
// This macro takes IDEAL HITS and produces IDEAL TRACKS, and finally fits the IDEAL TRACKS
 
Int_t gem_idealTracks(Int_t nStations, Double_t momentum = 15., Int_t nEvents = 1000, int verboseLevel = 0)
{ 
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return;
  }

  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  // Input file (MC events)
  TString baseName;
  baseName.Form("Gem_%dStations_%gGeV_n%d",nStations,momentum,nEvents);

  TString MCFile  = baseName + ".root";
  TString parFile = baseName + "_par.root";
  TString hitFile = baseName + "_idealHits.root";
  TString outFile = baseName + "_idealTracks.root";

  std::cout << "Output File: " << outFile.Data()<< std::endl;

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

  //------ Ideal Track finder --------------------
  //Create and add finder task
  PndGemFindTracks* finderTask = new PndGemFindTracks("PndGemFindTracks");
  finderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  fRun->AddTask(finderTask);
  
  PndGemTrackFinderIdeal* mcTrackFinder = new  PndGemTrackFinderIdeal();
  mcTrackFinder->SetVerbose(0);  // verbosity level
  mcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------

  //------ Track finder QA ---------------------------
  PndGemTrackFinderQA* trackFinderQA = new PndGemTrackFinderQA();
  trackFinderQA->SetVerbose(verboseLevel);
  fRun->AddTask(trackFinderQA);
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

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);


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

