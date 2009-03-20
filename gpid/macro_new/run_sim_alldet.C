void run_sim_alldet(const int nEvents = 20, const char* part="pi-",
		    const float p1 = 0.6, const float p2 = 1.0,
		    const char* SimOut = "SimOut.root",
		    const char* paramOut = "ParamOut.root"
		    )
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  //fRun->SetName("TGeant3");
  fRun->SetName("TGeant4");
  
  //fRun->SetOutputFile("el_sttcombi.root");
  fRun->SetOutputFile(SimOut);

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
 
  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);
  
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
  fRun->AddModule(Emc);   
  
  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);
 
  
  PndMdt* Mdt = new PndMdt("MDT",kTRUE);
  Mdt->SetMdtVersion("torino");
  Mdt->SetGeometryFileName("muopars.root");
  fRun->AddModule(Mdt);

  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc); 
  
  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root"); 
  fRun->AddModule(Dch);
  
  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Use DPM generator
  Float_t pbarP=4.0;
  PndDpmDirect *Dpm= new PndDpmDirect(pbarP,1);
  //primGen->AddGenerator(Dpm);

  // Box Generator
  TDatabasePDG pdg;// = new TDatabasePDG();
  int pid = pdg->GetParticle(part)->PdgCode();

  // 13 = muon; 1 = multipl.
  FairBoxGenerator* boxGen = new FairBoxGenerator(pid, 1);
  if (p2 < 0) p2 = p1;
  
  boxGen->SetPRange(p1,p2);// GeV/c
  boxGen->SetPhiRange(0., 360.);// Azimuth angle range [degree]
  boxGen->SetThetaRange(50., 140.);// Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.);// mm o cm ??
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kFALSE);
  
  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  
  fRun->SetField(fField);
  
  fRun->Init();
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
     
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  //output->open("params_sttcombi.root");
  output->open(paramOut);
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents);
   
  timer.Stop();
   
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n\n",rtime,ctime);
  exit(0);  
}
