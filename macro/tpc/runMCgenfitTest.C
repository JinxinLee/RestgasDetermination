{
  gRandom->SetSeed(1);
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
  Int_t nEvents = 1000;
  //Int_t nEvents = 250;

  //Set JOBNAME + JOBDIR (will not be created!)
  // --------------------------------------------------
  TString jobname="Test";
  TString jobdir="genfitTest";
  

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
  
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  /*FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);*/


  PndTpcDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpcGenfitTest3.geo"); // 8mm Si
  //PndTpc->SetGeometryFileName("tpc.geo"); 
  //ALICE Style MC (only for G3): =========================
  if(GEANT=="TGeant3") 
    PndTpc->SetAliMC();
  // ======================================================
  fRun->AddModule(PndTpc);

  
// Create and Set Event Generator
  //-------------------------------
  std::cout<<"Setup EvtGens"<<std::endl;std::cout.flush();
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
 
  // Box Generator
  
  //pdgs 211=pion 13=muon 11=electron, 2212 proton...
  //(PDG ID, MULTIPLICITY)
  FairBoxGenerator* boxGen = new FairBoxGenerator(11, 1);
  
  //boxGen->SetPRange(0.5,0.5); // GeV/c 
  //boxGen->SetPRange(1.0,1.0); // GeV/c 
  boxGen->SetPRange(1.5,1.5); // GeV/c 
  boxGen->SetPhiRange(-40, 40); // Azimuth angle range [degree]
  boxGen->SetThetaRange(90, 90); // Polar angle in lab system range [degree]
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
  
  // FairFieldMap *fMagField= new FairFieldMap("FIELD.v04_pavel.map");
  // Constant Field
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,20. ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 ); //
  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -2000, 2000);
      
  fRun->SetField(fMagField);
   
  fRun->SetStoreTraj(kTRUE);
  //fRun->SetStoreTraj(kFALSE);
  
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
  

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(dbfile.Data());
  rtdb->setOutput(output);

  PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  if ( fMagField ) {  fieldPar->SetParameters(fMagField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged(kTRUE);

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
  
