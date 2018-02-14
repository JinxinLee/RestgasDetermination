int run_reco_complete(TString FileName="test"){
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
 
  Int_t iVerbose = 0;

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------

  PndFileNameCreator creator(FileName.Data());

  // Input file
  TString inSimFile =  creator.GetSimFileName();
  TString inDigiFile = creator.GetDigiFileName();

  // Parameter file
  TString parFile = creator.GetParFileName();

  // Output file
  TString outFile = creator.GetRecoFileName();

  // Number of events to process
  Int_t nEvents = 0;

  // ------------------------------------------------------------------------
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
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
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------
 
  
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  PndSttMvdGemTrackingIdeal* trackStt = new PndSttMvdGemTrackingIdeal();
  trackStt->SetRelativeMomentumSmearing(0.05);
  trackStt->SetVertexSmearing(0.05, 0.05, 0.05);
  trackStt->SetTrackingEfficiency(1.);
  trackStt->SetTrackOutput("SttMvdGemIdealTrack");
  fRun->AddTask(trackStt);

  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
   recoKalman->SetTrackInBranchName("SttMvdGemIdealTrack");
   //recoKalman->SetTrackInIDBranchName("SttMvdGemIdealTrackID");
   recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
   recoKalman->SetBusyCut(50); // CHECK to be tuned
   //recoKalman->SetIdealHyp(kTRUE);
   //recoKalman->SetNumIterations(3);
   fRun->AddTask(recoKalman);


//  PndMvdRiemannTrackFinderTask* mvdTrackFinderRiemann = new PndMvdRiemannTrackFinderTask();
//  mvdTrackFinderRiemann->SetVerbose(iVerbose);
//  //  mvdTrackFinder->SetPersistence(kFALSE); // warum??
//  mvdTrackFinderRiemann->SetPersistence(kTRUE);
//  mvdTrackFinderRiemann->SetMaxDist(0.05);
//  fRun->AddTask(mvdTrackFinderRiemann);

  


//
//// Unified Pattern Recognition
//  //==========================================
//  //  use the constructor with input :
//  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
//  PndTrkTracking* tracking = new PndTrkTracking(0,false,false,false);
//  tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
//  // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
//  //  don't do the Pattern Recognition second part, starting from the Mvd;
//  tracking->NoMvdAloneTracking();
//  // do Cleanup only when there is Mixing;
//  // tracking->Cleanup();
//  fRun->AddTask(tracking);
//
//  PndSttMvdGemTracking * SttMvdGemTracking = new PndSttMvdGemTracking(0);
//  //SttMvdGemTracking->SetPdgFromMC();
//  fRun->AddTask(SttMvdGemTracking);
//
//  PndMCTrackAssociator* trackMC = new PndMCTrackAssociator();
//  trackMC->SetTrackInBranchName("SttMvdGemTrack");
//  trackMC->SetTrackOutBranchName("SttMvdGemTrackID");
//  fRun->AddTask(trackMC);
//
//  PndRecoKalmanTask* recoKalman = new PndRecoKalmanTask();
//  recoKalman->SetTrackInBranchName("SttMvdGemTrack");
//  recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
//  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
//  recoKalman->SetBusyCut(50); // CHECK to be tuned
//  //recoKalman->SetIdealHyp(kTRUE);
//  //recoKalman->SetNumIterations(3);
//  fRun->AddTask(recoKalman);
//
//  PndMCTrackAssociator* trackMC2 = new PndMCTrackAssociator();
//  trackMC2->SetTrackInBranchName("SttMvdGemGenTrack");
//  trackMC2->SetTrackOutBranchName("SttMvdGemGenTrackID");
//  fRun->AddTask(trackMC2);
//
//  PndFtsTrackerIdeal* trackFts = new PndFtsTrackerIdeal();
//  trackFts->SetRelativeMomentumSmearing(0.05);
//  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
//  trackFts->SetTrackingEfficiency(1.);
//  trackFts->SetTrackOutput("FtsIdealTrack");
//  fRun->AddTask(trackFts);
//
//  PndRecoKalmanTask* recoKalmanFwd = new PndRecoKalmanTask();
//  recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
//  //recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
//  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
//  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
//  //recoKalmanFwd->SetIdealHyp(kTRUE);
//  //recoKalmanFwd->SetNumIterations(3);
//  fRun->AddTask(recoKalmanFwd);
//
//  PndMCTrackAssociator* trackMC3 = new PndMCTrackAssociator();
//  trackMC3->SetTrackInBranchName("FtsIdealGenTrack");
//  trackMC3->SetTrackOutBranchName("FtsIdealGenTrackID");
//  fRun->AddTask(trackMC3);

  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init();
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

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  return 0;
}
