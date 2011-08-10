void runMC(TString jobdir="/nfs/nas/data/panda/tpc/SIM/evtmix/",TString jobname="phys_EtaCPhiPhi", Int_t nEvents=3000){
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
  //Int_t nEvents = 2000;

  //Set JOBNAME + JOBDIR (will not be created!)
  // --------------------------------------------------
  //TString jobdir="TEST";
  //TString jobname="DPM";
 
 // highly relatvistic approximation:
  double Minv=2.9803; // eta_c
  double p=Minv*Minv/ (2.*0.938);


  //TString basejobdir=gSystem->Getenv("VMCWORKDIR");
  //jobdir=(basejobdir+"/")+jobdir+"/";
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
  Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);


  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  
  PndTpcDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("TPC_V1.1.root");  
  //ALICE Style MC (only for G3): =========================
  if(GEANT=="TGeant3") 
    PndTpc->SetAliMC();
  // ======================================================
  fRun->AddModule(PndTpc);

  
  //OTHER SUBDETECTORS; Uncomment if you want to use

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
    Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);
  
   //-------------------------  GEM      -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);

  
  // Create and Set Event Generator
  //-------------------------------
  std::cout<<"Setup EvtGens"<<std::endl;std::cout.flush();
 
 
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  primGen->SetTarget(0., 0.5/2.355);
  primGen->SmearVertexZ(kTRUE);
  primGen->SmearGausVertexZ(kTRUE);
  primGen->SetBeam(0., 0., 0.1, 0.1);
  primGen->SmearVertexXY(kTRUE);
  fRun->SetGenerator(primGen);
  
  double mom=p;
  double mode=1;
  PndDpmDirect *dpmGen = new PndDpmDirect(mom,mode, gRandom->GetSeed(), 2.);
  //primGen->AddGenerator(dpmGen);

  PndEvtGenDirect *EvtGen = new PndEvtGenDirect("eta_c","EtaCInclusive_2phi.dec");
  EvtGen->SetStoreTree(kFALSE);
  primGen->AddGenerator(EvtGen);

  
  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format
  //---------------------Create and Set the Field(s)---------- 

 
  fRun->SetBeamMom(p);
  PndMultiField *fField= new PndMultiField("FULL");

  //PndConstField *fMagField=new PndConstField();
  //  fMagField->SetField(0, 0 ,20. ); // values are in kG
     // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  //fMagField->SetFieldRegion(-500, 500,-500, 500, -500, 500);


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

  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);


  rtdb->saveOutput();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
