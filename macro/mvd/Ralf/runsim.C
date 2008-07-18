// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  Int_t nEvents = 100;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("../Libs.C");

  CbmRunSim *fRun = new CbmRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System

//   PndMvdFileNameCreator namecreator("../data/mvddpm6GeV.root");
  PndMvdFileNameCreator namecreator("../data/mvdStrip.root");
  std::string filename = namecreator.GetSimFileName();
  fRun->SetOutputFile(filename.c_str());

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

//  CbmModule *Magnet= new PndMagnet("MAGNET");
//  Magnet->SetGeometryFileName("magnet.geo");
//  fRun->AddModule(Magnet);

  CbmModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  PndMvdDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  //Mvd->SetGeometryFileName("MVD14.root");
  Mvd->SetVerboseLevel(verboseLevel);
  Mvd->SetExclusiveSensorType("StripActive");
  fRun->AddModule(Mvd);
// fRun->AddModule((CbmDetector*)Mvd);

 // Create and Set Event Generator
 //-------------------------------

   CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
   fRun->SetGenerator(primGen);

 // Particle Generator (pdgid,mult, px,py,pz, vx,vy,vz)

     // single pions for testing
//      CbmParticleGenerator* partGenX   = new CbmParticleGenerator(211,1, 1.,0.,0., 0.,0.,0.);
//      CbmParticleGenerator* partGenY   = new CbmParticleGenerator(211,1, 0.,1.,0., 0.,0.,0.);
//      CbmParticleGenerator* partGenZ   = new CbmParticleGenerator(211,1, 0.,0.1,1., 0.,0.,0.);
//      CbmParticleGenerator* partGenXYZ = new CbmParticleGenerator(211,1, 1.,1.,5., 0.,0.,0.);
//      primGen->AddGenerator(partGenX);
//      primGen->AddGenerator(partGenY);
//      primGen->AddGenerator(partGenZ);
//      primGen->AddGenerator(partGenXYZ);

 // Ion Generator
 //    CbmIonGenerator *fIongen= new CbmIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);

 // Box Generator
    PndBoxGenerator *fBox = new PndBoxGenerator(2212, 5);
    fBox->SetPRange(0.5,0.5);
    fBox->SetThetaRange(135,150); 
//     fBox->SetThetaRange(15,150); //all
//     fBox->SetCosTheta();
    primGen->AddGenerator(fBox);

  //EvtGen Generator
// CbmEvtGenGenerator* evtGen = new
//   CbmEvtGenGenerator("/home/ralfk/Pandaroot/pandaroot/pgenerators/EvtGen/lambda.evt");
// primGen->AddGenerator(evtGen);  



 // Urqmd  Generator
 //    CbmUrqmdGenerator* urqmdGen = new CbmUrqmdGenerator("../../input/00-03fm.100ev.f14");
 //    primGen->AddGenerator(urqmdGen);


 // DPM Generator
//  PndDpmGenerator* dpmGen = new PndDpmGenerator("../data/Events/dpm6GeV.root");
//  primGen->AddGenerator(dpmGen);


  //K+
//   CbmBoxGenerator *fBox1 = new CbmBoxGenerator(321, 3);
//      fBox1->SetPRange(0.01,1.);
//      fBox1->SetThetaRange(5,150);
//      fBox1->SetPhiRange(0.,360);
// //      fBox1->SetCosTheta();
//      primGen->AddGenerator(fBox1);
  //P
//   CbmBoxGenerator *fBox2 = new CbmBoxGenerator(2212, 3);
//      fBox2->SetPRange(0.5,0.5);
//      fBox2->SetThetaRange(50,80);
//      fBox2->SetPhiRange(0.,360);
//      fBox2->SetCosTheta();
//      primGen->AddGenerator(fBox2);
  //Pi+
//   CbmBoxGenerator *fBox3 = new CbmBoxGenerator(211, 3);
//      fBox3->SetPRange(0.1,1.);
//      fBox3->SetThetaRange(5,150);
//      fBox3->SetPhiRange(0.,360);
// //      fBox3->SetCosTheta();
//      primGen->AddGenerator(fBox3);

  //Mu-
//  CbmBoxGenerator *fBox4 = new CbmBoxGenerator(13, 10);
//     fBox4->SetPRange(1.,1.);
//     fBox4->SetThetaRange(0,180);
//     fBox4->SetPhiRange(0.,360);
// //      fBox4->SetCosTheta();
//     primGen->AddGenerator(fBox4);


 // Field Map Definition
 // --------------------
   PndMultiField *fField= new PndMultiField();

   PndTransMap *map= new PndTransMap("TransMap", "R");
   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
   fField->AddField(map);
   fField->AddField(map1);
   fField->AddField(map2);

   fRun->SetField(fField);

   //fRun->SetStoreTraj(kTRUE);
   fRun->SetStoreTraj(kFALSE);


   fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------


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
  output->open("../data/mvdStrip_par.root");
  rtdb->setOutput(output);
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

