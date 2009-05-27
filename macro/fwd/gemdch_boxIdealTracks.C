void gemdch_boxIdealTracks(Int_t nEvents = 1000, Double_t momentum = 2.0, Int_t theta = 10, Int_t phi = 20, int verboseLevel = 0)
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
//  Int_t iVerbose = 1;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
   // Input file (MC events)
  TString baseName;
  baseName.Form("$VMCWORKDIR/data/GemDch_4Stations_211_%.1fGeV_th%d_ph%d_n%d",momentum,theta,phi,nEvents);

  TString MCFile  = baseName + ".root";
  TString parFile = baseName + "_par.root";
  // ------------------------------------------------------------------------
  TString outFile = baseName + "_idealTracksFine.root";
  
  std::cout << "RecoFile: " << outFile.Data()<< std::endl;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->SetOutputFile(outFile);
  

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo2->open("../params/gem_4Stations_fine.digi.par","in");
  rtdb->setSecondInput(parIo2);

  fRun->LoadGeometry();

   PndGemIdealHitProducer* gemHitProducer = new PndGemIdealHitProducer("GEM Ideal hit producer", verboseLevel);
   fRun->AddTask(gemHitProducer);

  //------ Ideal Track finder --------------------
  PndGemFindTracks* gemFinderTask = new PndGemFindTracks("PndGemFindTracks");
  gemFinderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  fRun->AddTask(gemFinderTask);
  
  PndGemTrackFinderIdeal* gemMCTrackFinder = new  PndGemTrackFinderIdeal();
  gemMCTrackFinder->SetVerbose(0);  // verbosity level
  gemMCTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  gemFinderTask->UseFinder(gemMCTrackFinder);
  //--------------------------------------------------

  //--------------------------------------------------
  //---00000----0000--00--00--------------------------
  //---00--00--00-----000000--------------------------
  //---00000----0000--00--00--------------------------
  //--------------------------------------------------

  // -----   Ideal Hit Producer   ---------------------------------------
  // -----   Ideal Hit Producer   ---------------------------------------
  PndDchDigiProducer* digiProducer= new PndDchDigiProducer();
  digiProducer->SetVerbose(0);
  fRun->AddTask(digiProducer);
  // -------------------------------------------------   
  PndDchCylinderHitProducer* cylHitProducer= new PndDchCylinderHitProducer();
  cylHitProducer->SetVerbose(0);
  fRun->AddTask(cylHitProducer);
  // ------------------------------------------------- 
//   PndDchIdealHitProducer* hitProducer= new PndDchIdealHitProducer();
//   fRun->AddTask(hitProducer);

  //------ Ideal DCH track finder --------------------
  //Create and add finder task
  //------ Ideal DCH track finder --------------------
  PndDchFindTracks* finderTask = new PndDchFindTracks("dchFindTracks");
  finderTask->SetUseHitOrDigi("chit");
  finderTask->SetVerbose(0);
  fRun->AddTask(finderTask);
  // ------------------------------------------------- 
  PndDchTrackFinderIdealCylHit* mcTrackFinder = new  PndDchTrackFinderIdealCylHit();
  mcTrackFinder->SetVerbose(0);  
  mcTrackFinder->SetPrimary(1);  // 1 = Only primary tracks are processed, 0 = all (default)
  finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------
  PndDchMatchTracks *matchTask = new PndDchMatchTracks();//match PndDchTracks and MCTracks
  matchTask->SetUseHitOrDigi("chit");
  matchTask->SetVerbose(0);
  fRun->AddTask(matchTask);

//   PndDchFindTracks* finderTask = new PndDchFindTracks("PndDchFindTracks");
//   finderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
//   fRun->AddTask(finderTask);
  
//   PndDchTrackFinderIdeal* mcTrackFinder = new  PndDchTrackFinderIdeal();
//   mcTrackFinder->SetVerbose(3);  // verbosity level
//   mcTrackFinder->SetPrimary(1);  // 1 = Only primary tracks are processed, 0 = all (default)
//   finderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------
  
  
  
  //------ Match PndDchTracks and MCTracks tracks ----
  PndDchMatchTracks *matchTask = new PndDchMatchTracks();
  matchTask->SetUseHitOrDigi("hit"); //"hit" - default
  matchTask->SetVerbose(1);
  fRun->AddTask(matchTask);
  // -------------------------------------------------
  
  
  
  //------ Quality of PndDchTracks -------------------
  PndDchFindTracksQa *qualityTrack = new PndDchFindTracksQa();
  qualityTrack->SetUseHitOrDigi("hit");  //"hit" - default
  qualityTrack->SetVerbose(1);
  fRun->AddTask(qualityTrack);
  // -------------------------------------------------


  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  FairGeane *Geane = new FairGeane(MCFile);
  //--------------------------------------------------

  // -----   Prepare tracks for genfit   --------------------------------------------
  PndFwdPrepareKalmanTracks *prepareKalmanTracks = new PndFwdPrepareKalmanTracks();
  prepareKalmanTracks->SetVerbose(0);
  prepareKalmanTracks->UseGeane(kTRUE);
  prepareKalmanTracks->UseMC(kTRUE);
  prepareKalmanTracks->SetPDG(211);
  prepareKalmanTracks->SetPersistence();
  fRun->AddTask(prepareKalmanTracks);
  //--------------------------------------------------

  // -----   Run Kalman fitter   --------------------------------------------
  PndFwdKalmanTask* fwdKalman = new PndFwdKalmanTask();
  fwdKalman->SetVerbose(0);
  fwdKalman->SetNumIterations(6);
  fwdKalman->SetMomentum(momentum);
  fwdKalman->SetTheta(theta);
  fwdKalman->SetPhi(phi);
  //  fwdKalman->SetSmooth(kFALSE);
  fRun->AddTask(fwdKalman);
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
