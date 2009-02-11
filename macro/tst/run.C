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
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libTst");
  gSystem->Load("libGen");


  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("test3.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new CbmPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);
  
  
  //FairModule *Magnet= new CbmMagnet("MAGNET");
  // 1- Active shielding Geometry
  //Magnet->SetGeometryFileName("magnet_active.geo");
  // 2- Iron Magnet
  // Magnet->SetGeometryFileName("magnet_iron.geo");
  //fRun->AddModule(Magnet);
  
  FairDetector *Mvd = new CbmTst("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_Rev14b_Corr+Dead.geo");
  fRun->AddModule(Mvd);
  
 // FairDetector *Sts= new CbmSts("STS", kTRUE);
 // Sts->SetGeometryFileName("sts_mvd.geo");
 // fRun->AddModule(Sts);

  
 FairModule *Target= new CbmTarget("Target");
 Target->SetGeometryFileName("target_vacuum.geo");
 fRun->AddModule(Target);		

 //FairDetector *Tof= new CbmTof("TOF", kTRUE );
 //Tof->SetGeometryFileName("tof.geo");
 //fRun->AddModule(Tof);
	
 //FairDetector *Trd= new CbmTrd("TRD",kTRUE );
 //Trd->SetGeometryFileName("trd_9.geo");
 //fRun->AddModule(Trd);

 // FairDetector *Rich= new CbmRich("RICH", kTRUE);
 // Rich->SetGeometryFileName("rich.geo");
 // fRun->AddModule(Rich);

  //FairDetector *Ecal= new CbmEcal("ECAL", kTRUE);
  //Ecal->SetGeometryFileName("ecal.geo");
  //fRun->AddModule(Ecal);


 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Urqmd  Generator
//  FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
//  primGen->AddGenerator(urqmdGen);

 // Particle Generator
   FairParticleGenerator* partGen = new FairParticleGenerator(211, 10, 1, 0,3,kTRUE);
   primGen->AddGenerator(partGen);
 
 // Ion Generator
 //    FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
 //    primGen->AddGenerator(fIongen);
 // Box Generator
 //    FairBoxGenerator *fBox = new FairBoxGenerator(211, 100);
 //    fBox->SetPRange(1,10);
 //    fBox->SetThetaRange(0,180);
 //    fBox->SetYRange(-100,100);
 //    primGen->AddGenerator(fBox);


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

;
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
  output->open("testparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------

  Int_t nEvents = 10;
  fRun->Run(nEvents);



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
