// Macro sim_radmap
// It creates a geant simulation file with the RadMap branch
void sim(TString output="/tmp/test.root", TString input, Int_t nEvents=100,  Char_t TransportModel[] = "TGeant3", UInt_t seed=0)
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
  Pipe->SetGeometryFileName("beampipe_201112.root");
  fRun->AddModule(Pipe);

  FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe_electronics.geo");
  //  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);

  //FairDetector *Tof = new PndTof("TOF",kFALSE);
  //Tof->SetGeometryFileName("tofbarrel.geo");
  //fRun->AddModule(Tof);
  
  // including forward detectors
  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetBarrel("muon_TS_barrel_strip_v1_noGeo.root");
  Muo->SetEndcap("muon_TS_endcap_strip_v1_noGeo.root");
  Muo->SetForward("muon_Forward_strip_v1_noGeo.root");
  Muo->SetMuonFilter("muon_MuonFilter_strip_v1_noGeo.root");
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
  Drc->SetGeometryFileName("dirc_l0_p0.root");
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);

  PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  Dsk->SetGeometryFileName("dsk.geo");
  fRun->AddModule(Dsk);
 
  //-------------------------  FTS       -----------------
  FairDetector *Fts= new PndFts("FTS", kFALSE);
  Fts->SetGeometryFileName("fts.geo");
  fRun->AddModule(Fts);

  //---------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF",kFALSE);
  FTof->SetGeometryFileName("ftofwall_shifted.root"); // not default
  fRun->AddModule(FTof);

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  PndFlukaGenerator* flugen = new PndFlukaGenerator(input);
  primGen->AddGenerator(flugen);

  /*    
  PndDpmGenerator* dpmGen = new PndDpmGenerator(input);
  //  dpmGen->SetStartEvent(4001);
  primGen->AddGenerator(dpmGen);
  */

  /*  
  //  FairBoxGenerator* boxGen = new FairBoxGenerator(-211, 1); // 13 = muon; 1 = multipl.
   FairBoxGenerator* boxGen = new FairBoxGenerator(2112, 2);
  boxGen->SetPRange(.4,.4); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0.001, 180); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta();//uniform generation on all the solid angle(default)
  // boxGen->SetXYZ(0., 0.37, 0.); 
  primGen->AddGenerator(boxGen);
  */
 
  fRun->GetListOfModules()->Print();
 
  fRun->SetStoreTraj(kFALSE); // to store particle trajectories 
  
  fRun->SetRadMapRegister(kTRUE); // radiation map manager
    
  fRun->SetBeamMom(6.2);
  // fRun->SetBeamMom(15.0);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

  fRun->Init();
  //  fRun->WriteGeometry(); // requires modifications in FairBase

  //  gGeoManager->SetVisLevel(10);
  //  gGeoManager->GetMasterVolume()->Draw("ogl");

  fRun->Run(nEvents);
   
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  
  
}  
 
