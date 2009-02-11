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
  gSystem->Load("libField");
  gSystem->Load("libPndBase");  
  gSystem->Load("libPassive");
  gSystem->Load("libStt");
  gSystem->Load("libPlane");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");

  fRun->SetOutputFile("ex1.root");


  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

      FairDetector *Pla = new CbmPlane("Plane", kTRUE);
      Pla->SetGeometryFileName("plane3.geo"); 
      fRun->AddModule(Pla);

      //   FairDetector *Stt = new PndStt("STT", kTRUE);
//   Stt->SetGeometryFileName("optimized_stt.geo"); 
//   Stt->SetGeometryFileName("straws_skewed_blocks.geo"); 
//   fRun->AddModule(Stt);

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  PndBoxGenerator* boxGen = new PndBoxGenerator(13,1);
  boxGen->SetPRange(1.5,1.5); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  // boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen); 

  //magnetic field
  PndConstField *fMagField=new PndConstField();
   fMagField->SetField(0.,0.,20.); // values are in kG
   fMagField->SetFieldRegion(-150, 150, -150, 150, -250, 250);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
   fRun->SetField(fMagField);
                                                                                                     

  fRun->Init();

  // Fill the Parameter containers for this run
  //-------------------------------------------
     
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("ex1params.root");
  rtdb->setOutput(output);
 
   PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
   if ( fMagField ) {  fieldPar->SetParameters(fMagField); }
   fieldPar->setInputVersion(fRun->GetRunId(),1);
   fieldPar->setChanged(kTRUE);
   
   rtdb->saveOutput();
   rtdb->print();

  // Transport nEvents
  // -----------------
     
  Int_t nEvents = 1000;
  fRun->Run(nEvents);
     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
