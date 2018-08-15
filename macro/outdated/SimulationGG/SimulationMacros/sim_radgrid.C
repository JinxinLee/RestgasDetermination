// Macro sim_radgrid
// It creates a geant simulation file giving RadGrid histograms according
// to the defined mesh(s)
int sim_radgrid(TString output, TString input, Int_t nEvents,  Char_t TransportModel[] = "TGeant3", UInt_t seed=0)
{
  //   activate TMemStat info file (ROOT >=5.28)
  //   TMemStat mm("gnubuiltin");
  
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
  
  fRun->SetOutputFile(output);
  
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
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
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
  
  //  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(input);
  //  primGen->AddGenerator(evtGen);  
  
  //  PndFlukaGenerator* flugen = new PndFlukaGenerator(input);
  //  primGen->AddGenerator(flugen);

  /*  
  PndDpmGenerator* dpmGen = new PndDpmGenerator(input);
  //  dpmGen->SetStartEvent(4001);
  primGen->AddGenerator(dpmGen);
  */



    FairBoxGenerator* boxGen = new FairBoxGenerator(-211, 1); // 13 = muon; 1 = multipl.
    boxGen->SetPRange(.4,.4); // GeV/c //setPRange vs setPtRange
    boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
    boxGen->SetThetaRange(0.001, 180); // Polar angle in lab system range [degree]
    boxGen->SetCosTheta();//uniform generation on all the solid angle(default)
    // boxGen->SetXYZ(0., 0.37, 0.); 
    primGen->AddGenerator(boxGen);

  
  fRun->SetStoreTraj(kFALSE); // to store particle trajectories 
  
  fRun->SetRadGridRegister(kTRUE); // activate RadGridManager

  // define two example meshs for dosimetry	
  FairMesh* aMesh1 = new FairMesh("test1");
  aMesh1->SetX(-200,200,200);
  aMesh1->SetY(-200,200,200);
  aMesh1->SetZ(-0.2,0.2,1.);
  
  FairMesh* aMesh2 = new FairMesh("test2");
  aMesh2->SetX(-100,100,100);
  aMesh2->SetY(-100,100,100);
  aMesh2->SetZ(-0.1,0.1,1.);
  
  aMesh1->print();
  aMesh2->print();
  
  fRun->AddMesh( aMesh1 );
  fRun->AddMesh( aMesh2 );

  //  fRun->SetBeamMom(3.0);
  fRun->SetBeamMom(15.0);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

  fRun->Init();

  //  gGeoManager->SetVisLevel(10);
  //  gGeoManager->GetMasterVolume()->Draw("ogl");

  fRun->Run(nEvents);
   
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 0;
}  
 
