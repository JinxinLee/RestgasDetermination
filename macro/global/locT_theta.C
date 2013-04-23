void locT_theta(Int_t pid = 13, Double_t theta = 5., Int_t nStations = 3) {
  
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return;
  }

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString inDigiFile = Form("digi_%dStations_p%d_theta%.1f.root",nStations,pid,theta);
  TString inSimFile = Form("points_%dStations_p%d_theta%.1f.root",nStations,pid,theta);

  // Parameter file
  TString parFile = Form("params_%dStations_p%d_theta%.1f.root",nStations,pid,theta);

  // Output file
  TString outFile = Form("locT_%dStations_p%d_theta%.1f.root",nStations,pid,theta);

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
  if ( nStations == 4 ) allDigiFile = sysFile+"/macro/params/all4.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ##################################################################   MVD
  // -----   MVD ideal tracking  --------------------------------------------
  PndMvdIdealTrackFinderTask* mvdTrackFinder = new PndMvdIdealTrackFinderTask();
  mvdTrackFinder->SetVerbose(iVerbose);
  fRun->AddTask(mvdTrackFinder);
  // ##################################################################   GEM
  // trackfinding ....
  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);
  /*  // trackmatching ....
  PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
  sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttTrackMatcher);  
  // trackfitting ....
  PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(0);
  PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter); 
  sttFitTracks->AddHitCollectionName("STTHit");
  fRun->AddTask(sttFitTracks);*/
  // helix hit production ....
//   PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
//   fRun->AddTask(sttHHProducer);
  // ##################################################################   GEM
  // -----   GEM ideal tracking  --------------------------------------
  PndGemFindTracks* gemFinderTask = new PndGemFindTracks("PndGemFindTracks");
  gemFinderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  fRun->AddTask(gemFinderTask);
  // ------------------------------------------------------------------  
  PndGemTrackFinderIdeal* gemMcTrackFinder = new  PndGemTrackFinderIdeal();
  gemMcTrackFinder->SetVerbose(0);  // verbosity level
  gemMcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  gemFinderTask->UseFinder(gemMcTrackFinder);
  // ------------------------------------------------------------------  
  PndGemTrackFinderQA* gemTrackFinderQA = new PndGemTrackFinderQA();
  gemTrackFinderQA->SetVerbose(0);
  fRun->AddTask(gemTrackFinderQA);
  // ################################################################## 

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
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
