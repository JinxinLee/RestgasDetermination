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
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libtpc");
 // gSystem->Load("libFsc");
 gSystem->Load("libStt");
 //  gSystem->Load("libMvd");
  gSystem->Load("libRpc");


  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  fRun->SetName("TGeant3");

  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("test.root");

  // Set Material file Name
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

// FairDetector *Stt = new PndStt("STT", kTRUE);
// Stt->SetGeometryFileName("straws_skewed_blocks.geo"); 
// fRun->AddModule(Stt);
  

//  FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
//  Tpc->SetGeometryFileName("tpc.geo");
//  fRun->AddModule(Tpc);

   FairDetector *Rpc = new PndRpcDetector("Rpc", kTRUE);
   Rpc->SetGeometryFileName("rpc.geo");
   fRun->AddModule(Rpc);


//  FairDetector *Fsc = new FscDetector("FSC", kTRUE);
//  Fsc->SetGeometryFileName("fsc.geo");
//  fRun->AddModule(Fsc);

  
 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Particle Generator
  //FairParticleGenerator* partGen = new FairParticleGenerator(211, 10, 1, 0,3,kTRUE);
  //primGen->AddGenerator(partGen);
 
 // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 5); //13 = muon, 211=pion; 1 = multipl.
  boxGen->SetPRange(0.1,2.1); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(22.,140.); // Polar angle in lab system range [degree]
//  boxGen->SetThetaRange(3.,6.); // Polar angle in lab system range [degree]
  primGen->AddGenerator(boxGen);



  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format
  // FairFieldMap *fMagField= new FairFieldMap("FIELD.v04_pavel.map");
  // Constant Field
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,20. ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
  
  // 2- Reading the new field map in the new format
  //  FairField *fMagField= new FairFieldMapSym3("FieldActive");
  // Active Shielding
  
  fRun->SetField(fMagField);
  fRun->SetStoreTraj(kTRUE);
  
  fRun->Init();
  
  
  // -Trajectories Visualization (TGeoManager Only )
  // -----------------------------------------------
  
  ;
  // Set cuts for storing the trajectpries
  //   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  //   trajFilter->SetStepSizeCut(0.01); // 1 cm
  //   trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  //   trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  //   trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  //   trajFilter->SetStorePrimaries(kTRUE);
  //   trajFilter->SetStoreSecondaries(kTRUE);
  
  
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

  Int_t nEvents =100;
  fRun->Run(nEvents);
 


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);


 TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
 
 TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
 c1->SetFillColor(10);
 
 geoMan->SetVisLevel(3); // Number of folded geo levels; 3 for emc. 
                         //Try to change the number and 
                         //to see how the geometry changes.
 geoMan->GetMasterVolume()->Draw("same");
 //c1->x3d();

}  
  
