// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
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
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libDch");

  CbmRunSim *fRun = new CbmRunSim();
  
  // set the MC version used
  fRun->SetName("TGeant3");

  fRun->SetOutputFile("tst.root");

  // Set Material file Name
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  CbmModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);

  CbmDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root");
  Dch->SetVerboseLevel(1);
  fRun->AddModule(Dch);

 // Create and Set Event Generator
 //-------------------------------

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Particle Generator
  //CbmParticleGenerator* partGen = new CbmParticleGenerator(211, 10, 1, 0,3,kTRUE);
  //primGen->AddGenerator(partGen);
 
 // Box Generator
  CbmBoxGenerator* boxGen = new CbmBoxGenerator(13, 10); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(0.1.,10.); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(2, 10); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kFALSE);
  

  // Field Map Definition
  // --------------------
  PndMultiField *fField= 0;
  fField = new PndMultiField();
  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  fRun->SetField(fField);
  //-----------end of Bfield stuff

  fRun->Init();
  
  // Transport nEvents
  // -----------------
  Int_t nEvents = 1000;
  fRun->Run(nEvents);
   
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  
  // TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("CBMGeom");
  //  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
  //  c1->SetFillColor(10);
  //  geoMan->SetVisLevel(3); 
  // geoMan->GetMasterVolume()->Draw("same");
 
  exit(0);
}  
  
