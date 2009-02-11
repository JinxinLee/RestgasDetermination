// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 3;
  Int_t nEvents = 10;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("../Libs.C");

  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");

  fRun->SetOutputFile("../data/testMCgeo.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

//  FairModule *Magnet= new PndMagnet("MAGNET");
//  Magnet->SetGeometryFileName("magnet.geo");
//  fRun->AddModule(Magnet);

 FairModule *Pipe= new PndPipe("PIPE");
 Pipe->SetGeometryFileName("pipebeamtarget.geo");
 fRun->AddModule(Pipe);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
//   Mvd->SetGeometryFileName("MVD14.root");
  Mvd->SetGeometryFileName("MVD_v1.0.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);


 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);


 // Box Generator
    PndBoxGenerator *fBox = new PndBoxGenerator(211, 1);
    fBox->SetPRange(0.1,10);
    fBox->SetThetaRange(0,180); //all
    fBox->SetYRange(-100,100);
    fBox->SetCosTheta();
    primGen->AddGenerator(fBox);

   fRun->SetStoreTraj(kFALSE);


   fRun->Init();
  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("../data/testMCGeoParams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------
   fRun->Run(nEvents);

  gGeoManager->SetVisLevel(1000);
  gGeoManager->DefaultColors();
  gGeoManager->GetMasterVolume()->Draw("ogl");
  gGeoManager->GetListOfVolumes()->Print();

//   gGeoManager->CheckOverlaps();
//   gGeoManager->PrintOverlaps();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}

