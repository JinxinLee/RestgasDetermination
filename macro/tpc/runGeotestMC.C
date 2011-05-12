void runGeotestMC(double momentum, double angle, TString outdir) {
  TStopwatch timer;
  timer.Start();

    
  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // --------------------------------------------------
  TString GEANT = "TGeant3";
  
  fRun->SetName(GEANT);
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");

  // SET NUMBER OF EVENTS
  // --------------------------------------------------
  Int_t nEvents = 1000;

  //Set JOBNAME + JOBDIR (will not be created!)
  // --------------------------------------------------
  TString jobdir=outdir;
  int length = jobdir.Length();
  if(jobdir[length-1] != "/") 
    jobdir.Append("/");
  
  TString jobname="GeotestElectron_deg";
  char bufferang[5];
  sprintf(bufferang, "%.1f", angle);
  jobname.Append(bufferang);
  char buffermom[5];
  jobname.Append("_mom");
  sprintf(buffermom, "%.1f", momentum);
  jobname.Append(buffermom);
  jobname.Append(".mc.root");

  TString outfile = jobdir+jobname;
  std::cout<<outfile<<std::endl;
  TString dbfile = outfile;
  std::cout<<dbfile<<std::endl;
  dbfile.ReplaceAll(".mc.root", ".param.root");
  std::cout<<dbfile<<std::endl;

  fRun->SetOutputFile(outfile);
  std::cout<<"Set output file to "<<outfile<<std::endl;
  
  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  

 // Fill the Parameter containers for this run
  //-------------------------------------------
  TString digiFile = gSystem->Getenv("VMCWORKDIR");
  digiFile += "/tpc/tpc.par";
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(digiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);       
  Bool_t kParameterMerged=kTRUE;
    
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  std::cout<<dbfile<<std::endl;
  output->open(dbfile.Data());
  rtdb->setOutput(output);
  


  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  //fRun->AddModule(Pipe);

  // FairModule *Target= new CbmTarget("Target");
  // Target->SetGeometryFileName("target_vacuum.geo");
  // fRun->AddModule(Target);
  
 FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  //fRun->AddModule(Magnet);


  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  //fRun->AddModule(Dipole);
  
  PndTpcDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetAllSensitive(true);
  PndTpc->SetGeometryFileName("TPC_V1.0.root");
  //PndTpc->SetGeometryFileName("tpc_prototype.root");
  //ALICE Style MC (only for G3): =========================
  if(GEANT=="TGeant3") 
    PndTpc->SetAliMC();
  // ======================================================
  fRun->AddModule(PndTpc);

  //OTHER SUBDETECTORS; Uncomment if you want to use

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  //fRun->AddModule(Mvd);
  
   //-------------------------  GEM      -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  //fRun->AddModule(Gem);

  
  // Create and Set Event Generator
  //-------------------------------
  std::cout<<"Setup EvtGens"<<std::endl;std::cout.flush();
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
 
  // Box Generator
  
  //pdgs 211=pion 13=muon 11=electron, ...
  //(PDG ID, MULTIPLICITY)
  FairBoxGenerator* boxGen = new FairBoxGenerator(11, 5); 
  
  boxGen->SetPRange(momentum,momentum); // GeV/c 
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(angle, angle); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);

  fRun->SetBeamMom(15);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  //fRun->SetStoreTraj(kTRUE);
  fRun->SetStoreTraj(kFALSE);
  
  std::cout<<"Starting INIT"<<std::endl;
  fRun->Init();
  std::cout<<"Ending INIT"<<std::endl;
  std::cout.flush();
  
  // -Trajectories Visualization (TGeoManager Only )
  // -----------------------------------------------
    
  // Set cuts for storing the trajectpries
  //   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  //   trajFilter->SetStepSizeCut(0.01); // 1 cm
  //   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  //   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  //   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  //   trajFilter->SetStorePrimaries(kTRUE);
  //   trajFilter->SetStoreSecondaries(kTRUE);

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
}  
  
