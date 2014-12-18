// Macro created A.Sanchez
// It creates a geant simulation file for forward tof wall
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSciT");

  FairRunSim *fRun = new FairRunSim();
  
  
  TString inFile3= "pbarC_3_GeV.root";

  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant3");
  
  
 
  fRun->SetOutputFile("test.root");
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave); 
  
   FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);

  
  FairDetector *SciT = new PndSciT("SCIT",kTRUE);
  SciT->SetGeometryFileName("BarrelScitil_Copy.root");//barrel-SciTil_18122012.root");//SciTil_Barrel_woPCB.root");
  fRun->AddModule(SciT);
 // 
   PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kTRUE); // for fast sim Cherenkov -> kFALSE
  // set reflectivity for the mirror at the bar end, in case of kFALSE reflectivity = 1 = const.
  Drc->SetMirrorReal(kTRUE);  
  Drc->SetDetEffAtProduction(kTRUE);
  Drc->SetStopTime(200.); 
  Drc->SetVerboseLevel(0);
  Drc->SetOnlyDirectPho(kFALSE);
  // put the geometry file you want into the next line:  
  Drc->SetGeometryFileName("dirc_l0_p0_updated.root"); 
  fRun->AddModule(Drc);
  
 PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(1);
  // See PndEmc::SetGeometryVersion() for available geometries and add there new one if necessary
  Emc->SetStorageOfData(kTRUE);
  fRun->AddModule(Emc);
  

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

   
  PndUrqmdSmmGenerator* urqmdGen = new PndUrqmdSmmGenerator(inFile3);
  
  primGen->AddGenerator(urqmdGen);
  
  // Box Generator: 
  // PndBoxGenerator* boxGen = new PndBoxGenerator(2212, 1); // 13 = muon; 1 = multipl. // 211 = pi+
//   // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  
//   boxGen->SetPRange(1.,1.); // GeV/c
//   // boxGen->SetPtRange(1.,1.); // GeV/c
//   boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
//   boxGen->SetThetaRange(20., 90.); // Polar angle in lab system range [degree]
//   boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
//   boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [cm]
//   primGen->AddGenerator(boxGen);  
  
  
  // Create and Set Magnetic Field
  //-------------------------------
  /*PndMultiField *fField= new PndMultiField("FULL");
      
    fRun->SetField(fField);*/
  
  /*
    PndConstField *fMagField=new PndConstField();
    fMagField->SetField(0, 0 ,20. ); // values are in kG
    // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
    fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
    fRun->SetField(fMagField);*/
  
  
  fRun->SetBeamMom(15);
  
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  
  /*FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    trajFilter->SetStepSizeCut(0.001); // 1 cm
    //  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
    // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
    //  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
    trajFilter->SetStorePrimaries(kTRUE);
    trajFilter->SetStoreSecondaries(kTRUE);*/ // not used for the others.????
  
  fRun->Init();
  
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
  /* PndMultiFieldPar* fieldPar = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if ( fField ) { fieldPar->SetParameters(fField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged();*/

  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("simparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
   
  // Set the number of events
  Int_t nEvents = 100; 
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  delete fRun;
  //exit(0);

}  
  
