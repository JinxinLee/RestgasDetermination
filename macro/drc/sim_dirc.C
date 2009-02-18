// Macro for running the DRC simulation
// only the DRC detector is ON
// output file testrun.root contains the MC hits
//11/10/2006 Annalisa Cecchi

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
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libDrcProp");  
  gSystem->Load("libDrc");
  gSystem->Load("libTof");
  gSystem->Load("libGen");

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("./testrun1.root");
  //  fRun->SetOutputFile("./Pi-100ev0.1_0.5.root");


  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  
  // Create and add detectors
  //-------------------------
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);
      
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  //Drc->SetRunCherenkov(kFALSE);
  // Drc->SetGeometryFileName("dirc.geo"); 
  fRun->AddModule(Drc);

  FairDetector *Tof = new PndTof("TOF", kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo"); 
  // fRun->AddModule(Tof);


   // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Urqmd  Generator
  //  FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //  primGen->AddGenerator(urqmdGen);

  // Particle Generator
  //  FairParticleGenerator* partGen = new FairParticleGenerator(-211, 2, 2., 0., 0.);
  //  primGen->AddGenerator(partGen);

  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(-13, 1); // 13 = muon; 1 = multipl.
  //  FairBoxGenerator* boxGenMu = new FairBoxGenerator(13, 1);  
  boxGen->SetPRange(3.,3.); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(22., 135.); // (22 - 135) Polar angle in lab system range [degree]
  // boxGen->SetXYZ(0., 0.37, 0.); // mm o cm ??

  //  boxGenMu->SetPRange(1., 1.5 ); // GeV/c //setPRange vs setPtRange
  //  boxGenMu->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  //  boxGenMu->SetThetaRange(22., 140.); // Polar angle in lab system range [degree]

  primGen->AddGenerator(boxGen);
  //  primGen->AddGenerator(boxGenMu);

  // Ion Generator
  //FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  //  primGen->AddGenerator(fIongen);

  fRun->SetStoreTraj(kTRUE);

  //magnetic field
  PndMultiField *fField= new PndMultiField();

  //PndTransMap *map= new PndTransMap("TransMap", "R");
  //PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  //fField->AddField(map);
  //fField->AddField(map1);
  fField->AddField(map2);


  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0.,0.,20.); // values are in kG
  //fMagField->SetField(0.,0.,0.);
  fMagField->SetFieldRegion(-50, 50,-50, 50, -150, 110); // values are in cm (xmivoln,xmax,ymin,ymax,zmin,zmax)
  //  fField->AddField(fMagField);
  
  fRun->SetField(fField);
  fRun->Init();
   
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("./testparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
     
  // Transport nEvents
  // -----------------
     
  Int_t nEvents = 2;

  fRun->Run(nEvents);
     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);



}  
  
