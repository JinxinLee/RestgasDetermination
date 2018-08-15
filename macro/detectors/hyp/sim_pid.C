// Macro created A.Sanchez
// It creates a geant simulation file for hyp
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs();
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  //rootlogon();
  
  //gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  
  FairRunSim *fRun = new FairRunSim();
  
  
  
  //--Second xi minus momentum solution.
  
  //TString inFile= "froot/ximinAsciiStpRate.dat";
  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant4");
  
  //fRun->SetOutputFile("./sim_hypgeantcheck.root");
  
  
  fRun->SetOutputFile("sim_pidC.root");
  
  // Set Material file Name
  //-----------------------
  
  //fRun->SetMaterials("media_pnd_hyp.geo");
  //new media for hyp
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  /*
    FairModule *Magnet= new FairMagnet("MAGNET");
    Magnet->SetGeometryFileName("magnet.geo");
    fRun->AddModule(Magnet);
  */
  
  PndHyp *Hyp = new PndHyp("HYP",kTRUE);
  //FairDetector *Hyp = new PndHyp("HYP",kTRUE);
  
  //Hyp->SetGeometryFileName("HypST_block.geo");
  //fRun->AddModule(Hyp);
  //--blocks
  //---Layers (si+abs)
  //Hyp->SetGeometryFileName("HypST_prueba2.geo"); 
  //fRun->AddModule(Hyp);
  //--layers C+Si
  // Hyp->SetGeometryFileName("HypST_prueba23.geo"); 
  //fRun->AddModule(Hyp);
  //--layers C+si+hyppipe
  //Hyp->SetGeometryFileName("HypST_prueba24pipe.geo"); 
  //Hyp->SetGeometryFileName("HypST_newxy3C.geo"); 

  // --- root geometry ------
  Hyp->SetAbsorberVol("stglAb"); // absorber layer
  Hyp->SetSensorVol("stglSi");   // silicon sensor
  Hyp->SetGeometryFileName("HYPST_assexy3C.root");
  // ---------------------------------

  //Hyp->SetHypSDtoFile(true,false);
  fRun->AddModule(Hyp);
  
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/SetFragments.C");
  //FragConfig(fRun);
  
  
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  
  // Box Generator: 
  PndBoxGenerator* boxGen = new PndBoxGenerator(3312, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  PndBoxGenerator* boxGen1 = new PndBoxGenerator(2212, 1);
  PndBoxGenerator* boxGen2 = new PndBoxGenerator(-211, 1);
  PndBoxGenerator* boxGen3 = new PndBoxGenerator(+211, 1);
  PndBoxGenerator* boxGen4 = new PndBoxGenerator(321, 1);
  PndBoxGenerator* boxGen5 = new PndBoxGenerator(-321, 1);
  PndBoxGenerator* boxGen6 = new PndBoxGenerator(-11, 1);
  PndBoxGenerator* boxGen7 = new PndBoxGenerator(11, 1);
  // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  
  boxGen->SetPRange(.1,2.); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(30., 180.); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen);
  
  boxGen1->SetPRange(.1,2); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen1->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen1->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen1->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen1->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen1);
  
  
  boxGen2->SetPRange(.1,2); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen2->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen2->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen2->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen2->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen2);

  boxGen3->SetPRange(.1,2); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen3->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen3->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen3->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen3->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen3);

  boxGen4->SetPRange(.1,2); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen4->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen4->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen4->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen4->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen4);


  boxGen5->SetPRange(.1,2); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen5->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen5->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen5->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen5->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen5);

  boxGen6->SetPRange(.1,2); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen6->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen6->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen6->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen6->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen6);


  boxGen7->SetPRange(.1,2); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen7->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen7->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen7->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen7->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen7);
  
  
  //FairParticleGenerator* partGen = new FairParticleGenerator(3312, 1, -0.3,0.3,0.3, 0., 0., -76);
  //primGen->AddGenerator(partGen);
  
  // *** with Ascii inFile ***
  /*  FairAsciiGenerator* AsciiGen = new FairAsciiGenerator(inFile);
      primGen->AddGenerator(AsciiGen);*/
  
  
  

   PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,10. ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
  fRun->SetField(fMagField);
  

    /*PndMultiField *fField= new PndMultiField();
  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  fRun->SetField(fField);*/
  
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  
  
  fRun->Init();
  
  /*FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    trajFilter->SetStepSizeCut(0.001); // 1 cm
    //  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
    // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
    //  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
    trajFilter->SetStorePrimaries(kTRUE);
    trajFilter->SetStoreSecondaries(kTRUE);*/ // not used for the others.????
  
  
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
  // PndMultiFieldPar* fieldPar = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  // if ( fField ) { fieldPar->SetParameters(fField); }
  // fieldPar->setInputVersion(fRun->GetRunId(),1);
  // fieldPar->setChanged();

 PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  if ( fMagField ) { fieldPar->SetParameters(fMagField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged();
  
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  //output->open("simparams.root");
  output->open("pidparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  // Set the number of events
  Int_t nEvents =100; 
  
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  delete fRun;
  exit(0);
}  
  
