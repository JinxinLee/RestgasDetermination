// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
int runLumi0DPMbkgSte(Int_t nEvents=10, Float_t mom = 5., Int_t mode =1, const int verboseLevel=0, UInt_t seed=0){
  gRandom->SetSeed(seed);
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString parFile = "params_sttcombi.root";
  
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  //fRun->SetName("TGeant4");

  fRun->SetOutputFile("points_sttcombi.root");

  // Set the parameters
  //-------------------------------
 
 
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile);
  rtdb->setOutput(output);
  
  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcaveVAC.geo");
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

  // FairDetector *Stt= new PndStt("STT", kTRUE);
  // Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  // fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  // FairDetector *Fts= new PndFts("FTS", kTRUE);
  // Fts->SetGeometryFileName("fts.geo");
  // fRun->AddModule(Fts);

  // FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  // FTof->SetGeometryFileName("ftofwall.root");
  // fRun->AddModule(FTof);
  
  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  Lum->SetGeometryFileName("../macro/lmd/geo/Test-Dipol-Design.root"); //!!!
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);
  
  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  PndDpmDirect *dpmGen = new PndDpmDirect(mom, mode, gRandom->GetSeed(), .2);
  primGen->AddGenerator(dpmGen);

  // Create and Set Magnetic Field
  //-------------------------------
  fRun->SetBeamMom(mom);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

 
  /**Initialize the session*/
  fRun->Init();
  
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------
  fRun->Run(nEvents);

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  return 0;
}
