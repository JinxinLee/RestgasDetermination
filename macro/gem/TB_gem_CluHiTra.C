// Macro created by Radoslaw Karabowicz
// This macro takes digis and produces clusters, hits, tracks

Int_t TB_gem_CluHiTra (Int_t nEvents = 1000, Int_t anaEv=1000, Int_t nofC=1000, Int_t cWid=10, Int_t nStations=3, int verboseLevel = 0)

{ 
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations (" << nStations << "), only 3 or 4 allowed." << endl;
    return;
  }

  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  // Input file (MC events)
  TString baseName;
  baseName.Form("Gem_MvdStt_%dStations_DPM_n%d",nStations,nEvents);

  TString MCFile    = baseName + ".root";
  TString parFile   = baseName + "_par.root";
  TString digiFile  = baseName + Form("_digiSorted_a%d_c%dx%d.root",anaEv,nofC,cWid);
  TString trackFile = baseName + Form("_CluHiTrack_a%d_c%dx%d.root",anaEv,nofC,cWid);
  //  TString trackFile = baseName + Form("_clustReal2RRtime_Hits_TracksTB_gemOnly_a%d_c%dx%d_CT.root",anaEv,nofC,cWid);

  std::cout << "Output File: " << trackFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(digiFile);
  fRun->SetOutputFile(trackFile);
  
  fRun->SetEventMeanTime(50);

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
  PndGemFindClustersTB* gemFindClusters = new PndGemFindClustersTB("GEM Find Clusters", 0);//verboseLevel);
  gemFindClusters->SetInBranchName("GEMDigiSorted");
  fRun->AddTask(gemFindClusters);
  
  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", verboseLevel);
  gemFindHits->SetUseClusters();
  fRun->AddTask(gemFindHits);
  // ------------------------------------------------------------------------

  //Create and add finder task
  PndGemFindTracks* finderTask = new PndGemFindTracks("PndGemFindTracks");
  finderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  fRun->AddTask(finderTask);
  // //------ Realistic Track finder --------------------
  PndGemTrackFinderOnHitsTB* mcTrackFinder = new  PndGemTrackFinderOnHitsTB();
  mcTrackFinder->SetVerbose(verboseLevel);  // verbosity level
  mcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << trackFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;

}

