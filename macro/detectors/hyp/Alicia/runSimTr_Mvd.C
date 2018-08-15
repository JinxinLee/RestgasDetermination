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
  
  gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  
  FairRunSim *fRun = new FairRunSim();
  
  
  
  //--Second xi minus momentum solution.
  
  //TString inFile= "froot/ximinAsciiStpRate.dat";
  // set the MC version used

  // -- UrqmdSmm background events 
  // Int_t nEvents =99480;
  //TString inFile= "/d/panda02/urqmd_smm/pbarC_3GeV/pbarC_1.root";

  // ------------------------
  
  fRun->SetName("TGeant3");
  
  //fRun->SetOutputFile("./sim_hypgeantcheck.root");
  
  
  fRun->SetOutputFile("sim_piTr.root");
  
  // Set Material file Name
  //-----------------------
  
  //fRun->SetMaterials("media_pnd_hyp.geo");
  //new media for hyp
  fRun->SetMaterials("media_pnd_cuts.geo");
  
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

  //-------------------------  TPC       -----------------
  /*
  PndTpcDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpcHyp.geo");
  //if(SimEngine=="TGeant3")  PndTpc->SetAliMC();
  fRun->AddModule(PndTpc);
  */

  
  /* FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.2_Simplified_onlyStrip5_z-verschoben550.root"); // only sensors, update follows
  //Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);*/
  
  
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
  //Hyp->SetGeometryFileName("HYPST_assexy3C5Lay.root");
  Hyp->SetGeometryFileName("HYPST_assexy3C5Lay_mvd.root");//HYPST_assexy3C5Lay_test.root");
  

  // xxxxxxxx Sebastian asymmetric geo root xxxxxxxxxxx
  
  /*  Hyp->SetAbsorverVol("Absorber"); // absorber layer
  Hyp->SetSensorVol("Sensor");   // silicon sensor
  Hyp->SetGeometryFileName("SekTarget_open_varAbs4Si5_3Q_HYPbe_1mm_MVD.root");
  */
  
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
  PndVolGenerator* boxGen = new PndVolGenerator(-211, 2); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event

  boxGen->SetPRange(.1,.1); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(90., 90.); // Azimuth angle range [degree] 90
  boxGen->SetThetaRange(70., 70.); // Polar angle in lab system range [degree] 70
  //boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  //boxGen->SetXYZ(0., 0., -55.0); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen);

 // PndBoxGenerator* boxGen2 = new PndBoxGenerator(-211, 1); // 13 = muon; 1 = multipl. // 211 = pi+
 //  // first number: PDG particle code: 2nd number: particle multiplicity per event

 //  boxGen2->SetPRange(.1,.1); // GeV/c
 //  // boxGen->SetPtRange(1.,1.); // GeV/c
 //  boxGen2->SetPhiRange(90., 90.); // Azimuth angle range [degree] 90
 //  boxGen2->SetThetaRange(80., 80.); // Polar angle in lab system range [degree] 70
 //  //boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
 //  boxGen2->SetXYZ(0., 0., -55.0); // vertex coordinates [cm]
 //  primGen->AddGenerator(boxGen2);


  // Background events UrqmdSmm
  /* PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile);
  
  primGen->SetTarget(-76.5,0.);
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
  // cout<<" gGeoManager "<<gGeoManager<<endl;
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
  output->open("PiTrparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  // Set the number of events
  Int_t nEvents =1000; 
  
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  delete fRun;
  exit(0);
}  
  
