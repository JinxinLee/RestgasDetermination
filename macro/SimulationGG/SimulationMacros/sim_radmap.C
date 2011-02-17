// Macro 
// It creates a geant simulation file
void sim_olaf(TString simdatei, TString input, TString param, Int_t nEvents,  Char_t TransportModel[] = "TGeant4", UInt_t seed=0)
{
  gRandom->SetSeed(seed);
  TStopwatch timer;
  timer.Start();
  gDebug=0;
// Load basic libraries
      gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
      rootlogon();

  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
 // Int_t nEvents = n; 
  
  // set the MC version used
  // ------------------------
  
  fRun->SetName(TransportModel);
  
  fRun->SetOutputFile(simdatei);
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);

  FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryVersion(15);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);

  //FairDetector *Tof = new PndTof("TOF",kFALSE);
  //Tof->SetGeometryFileName("tofbarrel.geo");
  //fRun->AddModule(Tof);
  
  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);

  FairDetector *Gem = new PndGemDetector("GEM", kFALSE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);

  PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  Dsk->SetGeometryFileName("dsk.root");
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  fRun->AddModule(Dsk);

  PndDrc *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);


  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // EvtGen Generator: 

  //  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(input);
  //  primGen->AddGenerator(evtGen);  

//  PndFlukaGenerator* flugen = new PndFlukaGenerator(input);
//  primGen->AddGenerator(flugen);
   
    PndDpmGenerator* dpmGen = new PndDpmGenerator(input);
    primGen->AddGenerator(dpmGen);

/*
  FairBoxGenerator* boxGen = new FairBoxGenerator(-211, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(.4,.4); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0.001, 180); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta();//uniform generation on all the solid angle(default)
  // boxGen->SetXYZ(0., 0.37, 0.); 
  primGen->AddGenerator(boxGen);
*/ 

  fRun->SetStoreTraj(kFALSE); // to store particle trajectories 
  //  fRun->SetRadLenRegister(kTRUE); // radiation length manager
  fRun->SetRadMapRegister(kTRUE); // radiation map manager

  //magnetic field: no field when commented put
  //    PndFieldConst *fMagField=new PndFieldConst();
  //    fMagField->SetField(0.,0.,20.); // values are in kG
  //    fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  //    fRun->SetField(fMagField);
  
//  fRun->SetBeamMom(6.2);
  fRun->SetBeamMom(15.0);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

  fRun->Init();

  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  /*
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(param);
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  */

  // Transport nEvents
  // -----------------
  
  gGeoManager->SetVisLevel(10);
  //  gGeoManager->GetMasterVolume()->Draw("ogl");

  // test
  //  TH3F* dose1 = new TH3F("dose1","dose1",100,-100.,100.,100,0,.02,100,0.,1000.);


  fRun->Run(nEvents);
   
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}  
 
