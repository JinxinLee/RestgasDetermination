// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini

void runsim(Int_t nEvents=1000, Float_t pT=1.0)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();//basic libraries

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("testrun.root");

  // -----   Magnetic field   -------------------------------------------

  // Constant Field
  /*
  PndConstField *fMagField = new PndConstField();
  fMagField->SetField(0., 0. ,20. ); // values are in kG
  fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  fRun->SetField(fMagField);
  */

  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);

  fRun->SetField(fField);

  // --------------------------------------------------------------------

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_pipe_120cm.geo");
  fRun->AddModule(Stt);

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPtRange(pT,pT); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0, 180); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta();//uniform generation on all the solid angle (default)
  primGen->AddGenerator(boxGen);
  
  fRun->SetStoreTraj(kTRUE);
  
  fRun->Init();

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("testparams.root");
  rtdb->setOutput(output);

  // PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  PndMultiFieldPar* fieldPar = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if ( fField ) {  fieldPar->SetParameters(fField); }
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
  
