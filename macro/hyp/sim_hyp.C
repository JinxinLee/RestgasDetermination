// Macro created A.Sanchez
// It creates a geant simulation file for hyp
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive"); // add other detector's lib if you need them
  gSystem->Load("libHyp");
  gSystem->Load("libGen");
  gSystem->Load("libDpmEvtGen");
  gSystem->Load("libPGen");
  CbmRunSim *fRun = new CbmRunSim();
  
 
  
  //--Second xi minus momentum solution.
  //TString inFile= "/d/panda01/asanchez/PANDA/ximinAscii.dat";

// set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant4");
  
  fRun->SetOutputFile("./sim_hypgeantcheck.root");
  

  // Set Material file Name
  //-----------------------
  
  //fRun->SetMaterials("media_pnd_hyp.geo");
  //new media for hyp
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave); 
  /*
   CbmModule *Magnet= new CbmMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);
  */
  
  CbmDetector *Hyp = new PndHyp("HYP",kTRUE);
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
  Hyp->SetGeometryFileName("HypST_prueba24pipe.geo"); 
  fRun->AddModule(Hyp);

  // Create and Set Event Generator
  //-------------------------------
  
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);


    // Box Generator: 
  PndBoxGenerator* boxGen = new PndBoxGenerator(3312, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event

  
  boxGen->SetPRange(.1,0.7); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(20., 180.); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen);
  
  
  
  //CbmParticleGenerator* partGen = new CbmParticleGenerator(3312, 1, -0.3,0.3,0.3, 0., 0., -76);
  //primGen->AddGenerator(partGen);
  
  //*** with Ascii inFile ***
      // CbmAsciiGenerator* AsciiGen = new CbmAsciiGenerator(inFile);
      //primGen->AddGenerator(AsciiGen);
       

      PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,20. ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
  fRun->SetField(fMagField);
  
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 


  fRun->Init();

  /*CbmTrajFilter* trajFilter = CbmTrajFilter::Instance();
  trajFilter->SetStepSizeCut(0.001); // 1 cm
  //  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  //  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE);*/ // not used for the others.????

  
    
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open("simparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
   // Set the number of events
  Int_t nEvents =50; 

  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}  
  
