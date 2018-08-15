{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 10000;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("../Libs.C");

  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

//   fRun->SetName("TGeant3");
  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System

//  PndFileNameCreator namecreator("../data/mvdmaterial.root");
  PndFileNameCreator namecreator("../data/mvdTestGeo.root");
  std::string filename = namecreator.GetSimFileName();
  fRun->SetOutputFile(filename.c_str());

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

//   FairModule *Magnet= new PndMagnet("MAGNET");
//   Magnet->SetGeometryFileName("magnet.geo");
//   fRun->AddModule(Magnet);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);//kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
//  Mvd->SetGeometryFileName("MVD_v1.0.root");
//   Mvd->SetGeometryFileName("MVD14.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);



 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  //ROOTino
  PndBoxGenerator *fBox1 = new PndBoxGenerator(0, 1);
     fBox1->SetXYZ(0.,0.,0.);
     fBox1->SetPRange(0.1,0.5);
     fBox1->SetThetaRange(0.,180.);
     fBox1->SetPhiRange(0.,360.);
     fBox1->SetCosTheta();
     primGen->AddGenerator(fBox1);


 
   //fRun->SetStoreTraj(kTRUE);
   fRun->SetStoreTraj(kFALSE);
   fRun->SetRadLenRegister(kTRUE);
// gRandom->SetSeed(12345);
   fRun->Init();
   Mvd->Initialize(); // do this, if the detector is not set active

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("../data/mvdTestGeo1.0_par.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------
  fRun->SetTrackingDebugMode(false);
  fRun->Run(nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

