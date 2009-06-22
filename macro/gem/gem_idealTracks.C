// Macro created by Radoslaw Karabowicz
// This macro takes MC file and produces IDEAL DIGIS, then IDEAL TRACKS, and finally fits the IDEAL TRACKS

void gem_idealTracks(Int_t nEvents = 100, TString addString = "4Stations_15GeV", int verboseLevel = 0)
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
//  Int_t iVerbose = 1;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libGem");
  // Input file (MC events)
  TString baseName;
  baseName.Form("$VMCWORKDIR/data/Gem_%s_n%d",addString.Data(),nEvents);

  TString MCFile  = baseName + ".root";
  TString parFile = baseName + "_par.root";
  // ------------------------------------------------------------------------
  TString outFile = baseName + "_idealTracksFineM5.root";
  
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

  // -----   Ideal Hit Producer   ---------------------------------------
  PndGemIdealHitProducer* gemHitProducer = new PndGemIdealHitProducer("GEM Ideal hit producer", verboseLevel);
  fRun->AddTask(gemHitProducer);

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

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  FairGeane *Geane = new FairGeane(MCFile);
  //--------------------------------------------------

  // -----   Prepare tracks for genfit   --------------------------------------------
  PndGemPrepareKalmanTracks *prepareKalmanTracks = new PndGemPrepareKalmanTracks();
  prepareKalmanTracks->SetVerbose(0);
  prepareKalmanTracks->UseGeane(kTRUE);
  prepareKalmanTracks->UseMC(kFALSE);
  prepareKalmanTracks->SetPDG(13);
  prepareKalmanTracks->SetPersistence();
  fRun->AddTask(prepareKalmanTracks);
  //--------------------------------------------------

  // -----   Run Kalman fitter   --------------------------------------------
  PndGemKalmanTask* gemKalman = new PndGemKalmanTask();
  gemKalman->SetVerbose(0);
  gemKalman->SetNumIterations(6);
  //  gemKalman->SetSmooth(kFALSE);
  fRun->AddTask(gemKalman);
  // ------------------------------------------------- 

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  Geane->SetField(fRun->GetField());
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
