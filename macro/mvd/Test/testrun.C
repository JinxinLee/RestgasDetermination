// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 3;
  Int_t nEvents = 100;
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

  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");

  gSystem->Load("libMvd");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");

  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");

  PndMvdFileNameCreator namecreator("/home/ralfk/mvdmacros/data/Test/testMC.root");
  std::string filename = namecreator.GetSimFileName(false);
  fRun->SetOutputFile(filename.c_str());

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

//  FairModule *Pipe= new PndPipe("PIPE");
//  Pipe->SetGeometryFileName("pipebeamtarget.geo");
//  fRun->AddModule(Pipe);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  //Mvd->SetGeometryFileName("MVD_Rev14b_Corr+Dead.geo");
  Mvd->SetGeometryFileName("MVD14.root");
  //Mvd->SetGeometryFileName("MVD14_b.root");
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);


 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Urqmd  Generator
 //    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
 //    primGen->AddGenerator(urqmdGen);

 // Particle Generator (pdgid,mult, px,py,pz, vx,vy,vz)

     // single pions for testing
//      FairParticleGenerator* partGenX   = new FairParticleGenerator(211,1, 1.,0.,0., 0.,0.,0.);
//      FairParticleGenerator* partGenY   = new FairParticleGenerator(211,1, 0.,1.,0., 0.,0.,0.);
//      FairParticleGenerator* partGenZ   = new FairParticleGenerator(211,1, 0.,0.1,1., 0.,0.,0.);
//      FairParticleGenerator* partGenXYZ = new FairParticleGenerator(211,1, 1.,1.,1., 0.,0.,0.); 
//      primGen->AddGenerator(partGenX);
//      primGen->AddGenerator(partGenY);
//      primGen->AddGenerator(partGenZ);
//      primGen->AddGenerator(partGenXYZ);

 // Ion Generator
 //    FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);

 // Box Generator
    PndBoxGenerator *fBox = new PndBoxGenerator(211, 3);
    fBox->SetPRange(0.1,10);
    fBox->SetThetaRange(0,180);
    fBox->SetYRange(-100,100);
    fBox->SetCosTheta();
    primGen->AddGenerator(fBox);

 // DPM Generator
 //PndDpmGenerator* dpmGen = new PndDpmGenerator("/home/kph/stockman/fairroot/cbmroot/pandaroot/pgenerators/DpmEvtGen/Background-micro.root");
 //primGen->AddGenerator(dpmGen);

 // Field Map Definition
 // --------------------
  // 1- Reading the new field map in the old format

     // FairFieldMap *fMagField= new FairFieldMap("FIELD.v04_pavel.map");
   // Constant Field
     //  CbmConstField *fMagField=new CbmConstField();
     //  fMagField->SetFieldXYZ(0, 14 ,0 ); // values are in kG
     // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
     //  fMagField->SetFieldRegions(-74, -39 ,-22 , 74, 39 , 160 );

  // 2- Reading the new field map in the new format

//  FairField *fMagField= new FairFieldMapSym3("FieldActive");
  // Active Shielding

//   fRun->SetField(fMagField);

   //fRun->SetStoreTraj(kTRUE);
   fRun->SetStoreTraj(kFALSE);


   fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------


 // Set cuts for storing the trajectpries
  /* FairTrajFilter* trajFilter = FairTrajFilter::Instance();
     trajFilter->SetStepSizeCut(0.01); // 1 cm
     trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
     trajFilter->SetStorePrimaries(kTRUE);
     trajFilter->SetStoreSecondaries(kTRUE);
   */

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("testParams.root");
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

