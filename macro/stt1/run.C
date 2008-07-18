// Macro for running the STT1 simulation
// only the STT1 detector is ON
// output file testrun.root contains the MC hits
//13/09/2006 Pablo Genova

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
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libStt1");
  gSystem->Load("libGen");

  CbmRunSim *fRun = new CbmRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("./testrun.root");


  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  CbmModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

   CbmDetector *Stt = new CbmStt("STT", kTRUE);
   Stt->SetGeometryFileName("stt24.geo"); //
   fRun->AddModule(Stt);

  // Create and Set Event Generator
  //-------------------------------

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Urqmd  Generator
  //  CbmUrqmdGenerator* urqmdGen = new CbmUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //  primGen->AddGenerator(urqmdGen);

  // Particle Generator
  //CbmParticleGenerator* partGen = new CbmParticleGenerator(13, 1, 0.5, 0., 0.);
  //primGen->AddGenerator(partGen);

  // Box Generator
  CbmBoxGenerator* boxGen = new CbmBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(0.5,0.5); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0, 180); // Polar angle in lab system range [degree]
  // boxGen->SetXYZ(0., 0.37, 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);
 
  // Ion Generator
  //CbmIonGenerator *fIongen= new CbmIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  //  primGen->AddGenerator(fIongen);

  fRun->SetStoreTraj(kTRUE);

  //magnetic field
  CbmFieldConst *fMagField=new CbmFieldConst();
  fMagField->SetField(0.,0.,20.); // values are in kG
                                                                              
  
  fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)

  fRun->SetField(fMagField);
  fRun->Init();
  
  
// 
//   // Fill the Parameter containers for this run
//   //-------------------------------------------
//      
//   CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
//   Bool_t kParameterMerged=kTRUE;
//   CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
//   output->open("parfiles/testparams.root");
//   rtdb->setOutput(output);
//   rtdb->saveOutput();
//   rtdb->print();
     
  // Transport nEvents
  // -----------------
     
  Int_t nEvents = 50;
  fRun->Run(nEvents);
     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
