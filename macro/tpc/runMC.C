{
  TStopwatch timer;
  timer.Start();
  
  // Load basic libraries in rootlogon
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // --------------------------------------------------
  TString GEANT = "TGeant3";
  
  fRun->SetName(GEANT);
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");

  // SET NUMBER OF EVENTS
  // --------------------------------------------------
  Int_t nEvents = 10000;

  //Set JOBNAME + JOBDIR (will not be created!)
  // --------------------------------------------------
  TString jobdir="TEST";
  TString jobname="Test";
 

  TString basejobdir=gSystem->Getenv("VMCWORKDIR");
  jobdir=(basejobdir+"/")+jobdir+"/";
  std::cout<<jobdir<<std::endl;


  TString copy = jobdir;
  
  jobdir+=jobname;

  TString base=jobdir;
  TString outfile=base+".mc.root";
  TString dbfile=base+".param.root";

  std::cout<<outfile;

  fRun->SetOutputFile(outfile);


  //SET USER CONFIG AND CUTS:
  //REQUIRES CUSTOM g3Config.C and SetCuts.C present in JOBDIR
  //COMMENT OUT IF YOU WANT TO USE THE STANDARD FILES FROM gconfig/
  // --------------------------------------------------------
  //fRun->SetUserCuts(copy+"SetCuts.C");
  //if(GEANT=="TGeant3")
  //  fRun->SetUserConfig(copy+"g3Config.C");
  //if(GEANT=="TGeant4")
  //  fRun->SetUserConfig(copy+"g4Config.C");  
  

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  

 // Fill the Parameter containers for this run
  //-------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/all.par";

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);       
  Bool_t kParameterMerged=kTRUE;
  
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(dbfile.Data());
  rtdb->setOutput(output);


  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);

  // FairModule *Target= new CbmTarget("Target");
  // Target->SetGeometryFileName("target_vacuum.geo");
  // fRun->AddModule(Target);
  
 FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);


  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  
  PndTpcDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpc.geo");
  //ALICE Style MC (only for G3): =========================
  if(GEANT=="TGeant3") 
    PndTpc->SetAliMC();
  // ======================================================
  fRun->AddModule(PndTpc);

  
  //OTHER SUBDETECTORS; Uncomment if you want to use

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  
   //-------------------------  GEM      -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);

  
  // Create and Set Event Generator
  //-------------------------------
  std::cout<<"Setup EvtGens"<<std::endl;std::cout.flush();
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
 
  // Box Generator
  
  //pdgs 211=pion 13=muon 11=electron, ...
  //(PDG ID, MULTIPLICITY)
  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); 
  
  boxGen->SetPRange(0.5,2.5); // GeV/c 
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(30, 140); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);

  //FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  //fRun->SetGenerator(primGen);

  //DPM
  //TString dpmfile = basejobdir+"10k_2Gev_el_and_inel_DPMDATA.root";
  //PndDpmGenerator* dpmGen = new PndDpmGenerator(dpmfile);
  //primGen->AddGenerator(dpmGen);  
   
  
  //FairEvtGenGenerator* evtGen = new
  //FairEvtGenGenerator("../data/evtgen.y4260.jpsipipi.vvpipi.dat");
  //primGen->AddGenerator(evtGen);
  
  
  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format
  //---------------------Create and Set the Field(s)---------- 

  
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
  

 

  //PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  //if ( fMagField ) {  fieldPar->SetParameters(fMagField); }
  //fieldPar->setInputVersion(fRun->GetRunId(),1);
  //fieldPar->setChanged(kTRUE);

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
  
