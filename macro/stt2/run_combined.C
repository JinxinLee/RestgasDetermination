// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
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
  gSystem->Load("libMCStack");
  gSystem->Load("libPassive");
  gSystem->Load("libStt2");
  gSystem->Load("libTst");
  gSystem->Load("libGen");



  CbmRunSim *fRun = new CbmRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  //fRun->SetOutputFile("twoparticle_fieldon.root");
  //  fRun->SetOutputFile("correct_geom_4proton_1gev_isotropic.root");
  fRun->SetOutputFile("test.root");

  // -----   Magnetic field   -------------------------------------------
  CbmFieldConst *fMagField = new CbmFieldConst("myField", -100., 100., -100., 100., -200., 200, 0., 0., 20.); // was 40
//  fMagField->SetPosition(0., 0., 0.);
//  fMagField->SetScale(1.);
  fRun->SetField(fMagField);
  // --------------------------------------------------------------------

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  CbmModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

  CbmDetector *Stt= new CbmStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_axial.geo");
  fRun->AddModule(Stt);

  CbmDetector *Mvd = new CbmTst("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_Rev14b_Corr.geo");
  fRun->AddModule(Mvd);

 // Create and Set Event Generator
 //-------------------------------

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // proton 2212 pi+ 211 pi- -211
  CbmParticleGenerator* partGen1 = new CbmParticleGenerator(2212, 1, -1., 0., 0., 20., 0., 0.);
  primGen->AddGenerator(partGen1);
  CbmParticleGenerator* partGen2 = new CbmParticleGenerator(2212, 1, -1., 0., 0.);
  primGen->AddGenerator(partGen2);
  CbmParticleGenerator* partGen3 = new CbmParticleGenerator(2212, 1, -1., 0., 0.);
  primGen->AddGenerator(partGen3);
  CbmParticleGenerator* partGen4 = new CbmParticleGenerator(2212, 1, -1., 0., 0.);
  primGen->AddGenerator(partGen4);

 //  fRun->SetStoreTraj(kTRUE)
   
   fRun->Init();


 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

;
 // Set cuts for storing the trajectpries
  /* CbmTrajFilter* trajFilter = CbmTrajFilter::Instance();
     trajFilter->SetStepSizeCut(0.01); // 1 cm
     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
     trajFilter->SetStorePrimaries(kTRUE);
     trajFilter->SetStoreSecondaries(kTRUE);
   */


  // Fill the Parameter containers for this run
  //-------------------------------------------

  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open("parfiles/testparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  Int_t nEvents = 1;
  fRun->Run(nEvents);



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
