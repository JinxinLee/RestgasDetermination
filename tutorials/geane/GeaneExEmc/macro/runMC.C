void runMC(Int_t nrOfEvents=10, Double_t momentum=1.0, Char_t particle[]="e-")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");
  gSystem->Load("libField");
  gSystem->Load("libPassive");

  gSystem->Load("libStt"); 
  gSystem->Load("libMuo");
  gSystem->Load("libEmc");
  gSystem->Load("libMvd");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libTof");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("libTrkBase");
  gSystem->Load("libtrackrep");
  gSystem->Load("librecotasks");

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");

  fRun->SetOutputFile("mc.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("magnet.geo");
  //fRun->AddModule(Magnet);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);
  
  //  FairDetector *Stt= new PndStt("STT", kTRUE);
  //  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  //  fRun->AddModule(Stt);
  
  FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  Tpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(Tpc);
  
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD14.root");
  fRun->AddModule(Mvd);

  FairDetector *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc.geo"); 
  fRun->AddModule(Drc);

  FairDetector *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileName("emc_module1234.dat"); 
  fRun->AddModule(Emc);

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  TDatabasePDG *pdg = new TDatabasePDG();
  Int_t pid = pdg->GetParticle(particle)->PdgCode();
  
  PndBoxGenerator* boxGen = new PndBoxGenerator(pid,1);
  boxGen->SetPRange(momentum,momentum); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  // boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen); 

  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  
  //magnetic field
  /*  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0.,0.,20.); // values are in kG
  fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  fField->AddField(fMagField);
  */

  fRun->SetField(fField);

  fRun->SetStoreTraj(kTRUE); 

  fRun->Init();

  // -Trajectories Visualization
  // ----------------------------
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // Set cuts for storing the trajectpries
  trajFilter->SetStepSizeCut(0.01); // 1 cm
  //     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  //     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  //     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);

  // Fill the Parameter containers for this run
  //-------------------------------------------
     
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("params.root");
  rtdb->setOutput(output);
 
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();
   
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------
     
  Int_t nEvents = nrOfEvents;
  fRun->Run(nEvents);
     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
