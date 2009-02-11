//Macro designed to create Hits for the Space Charge Calculation
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
  gSystem->Load("libPGen");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  //  gSystem->Load("libGen");
  //  gSystem->Load("libgenfit");


  FairRunSim *fRun = new FairRunSim();

  fRun->SetName("TGeant4");
  
  fRun->SetOutputFile("spacecharge_in.root");

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new CbmCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);


  FairDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(PndTpc);
  
 
 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);


  PndDpmGenerator* dpmGen = new PndDpmGenerator("Background-micro.root");
  primGen->AddGenerator(dpmGen);  



 // Box Generator
 // FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); // 211 = PION; 1 = multipl.
 // boxGen->SetPRange(0.1, 1.0); // GeV/c //setPRange vs setPtRange
 // boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
 // boxGen->SetThetaRange(0, 180); // Polar angle in lab system range [degree]
 // boxGen->SetXYZ(0., 0.37, 0.); // mm o cm ??
 // primGen->AddGenerator(boxGen);



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
   //fRun->SetStoreTraj(kFALSE);
 

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

  // Transport nEvents
  // -----------------

  Int_t nEvents = 10000;
  fRun->Run(nEvents);



  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
