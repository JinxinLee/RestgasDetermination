int runsim(Int_t nEvents=1000, Float_t pT=1.0, UInt_t seed=0){

  gRandom->SetSeed(seed);

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
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");  
  gSystem->Load("libStt"); 
  gSystem->Load("libEmc"); 
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc"); 
  gSystem->Load("libtpcreco");
  gSystem->Load("libtrackrep");
  gSystem->Load("librecotasks");
  gSystem->Load("libMvd");
  gSystem->Load("libMvdReco");
  gSystem->Load("libLHETrack");

 
  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("points_sttmvdemc.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);
 
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD14.root");
  fRun->AddModule(Mvd);

  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
  fRun->AddModule(Emc);

  //  FairDetector *Tof = new PndTof("TOF",kTRUE);
  //  Tof->SetGeometryFileName("tofSciF.geo");
  //  fRun->AddModule(Tof);
  
  //  FairDetector *Muo = new PndMuo("MUO",kTRUE);
  //  Muo->SetGeometryFileName("muon_newmag.geo"); 
  //  fRun->AddModule(Muo);
  
  //  FairDetector *Drc = new PndDrc("DIRC", kTRUE);
  //  Drc->SetGeometryFileName("dirc.geo"); 
  //  fRun->AddModule(Drc); 
  
  //  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  //  Dch->SetGeometryFileName("dch.root"); 
  //  fRun->AddModule(Dch);
  
  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  //  boxGen->SetPRange(1.,1.1); // GeV/c
  boxGen->SetPtRange(pT,pT); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 90.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kTRUE);
  
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
  
  fRun->Init();
  /*
  // -Trajectories Visualization
  // ----------------------------
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // Set cuts for storing the trajectories
  trajFilter->SetStepSizeCut(0.01); // 1 cm
  //   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  //   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  //   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);
  */
  // 
  //   // Fill the Parameter containers for this run
  //   //-------------------------------------------
  //      
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
     
  //if a field is used save the parameters in the RTDB
  /*    
	PndSolenoidPar* Par1 = (PndSolenoidPar*) rtdb->getContainer("PndSolenoidPar");
	if ( map2 ) {  Par1->SetParameters(map2); }
	Par1->setChanged();
	Par1->setInputVersion(fRun->GetRunId(),1);
 
	PndDipolePar* Par2 = (PndDipolePar*) rtdb->getContainer("PndDipolePar");
	if (map1 ) {  Par2->SetParameters(map1); }
	Par2->setInputVersion(fRun->GetRunId(),1);
	Par2->setChanged();
  
	PndTransPar* Par3 = (PndTransPar*) rtdb->getContainer("PndTransPar");
	if (map ) {  Par3->SetParameters(map); }
	Par3->setInputVersion(fRun->GetRunId(),1);
	Par3->setChanged();
  */

  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("testparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents);
   
  timer.Stop();
   
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  return 0;
   
}  
  
