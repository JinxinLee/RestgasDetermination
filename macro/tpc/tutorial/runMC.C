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

  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  //  gSystem->Load("librecotasks");
  gSystem->Load("libjobdb");

  gSystem->Load("libPGen");

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  
  
  Int_t nEvents = 100;

  TString base="tutorial";
  TString outfile=base+".mc.root";
  TString dbfile=base+".param.root";

  fRun->SetOutputFile(outfile);
 

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);
  
  FairDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(PndTpc);
  

 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 
  // Box Generator

  // pdgs 211=pion 13=muon 11=electron
  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); // 13 = muon; 1 = multipl.
   boxGen->SetPtRange(1.,1.); // GeV/c //setPRange vs setPtRange
   boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
   boxGen->SetThetaRange(40, 40); // Polar angle in lab system range [degree]
   boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
   primGen->AddGenerator(boxGen);


   // Field Map Definition
   // --------------------
   // 1- Reading the new field map in the old format
   
   // Constant Field
   PndConstField *fMagField=new PndConstField();
   fMagField->SetField(0, 0 ,20. ); // values are in kG
   // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
   fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
   fRun->SetField(fMagField);
   
   fRun->SetStoreTraj(kTRUE);

   fRun->Init();


   // Fill the Parameter containers for this run
   //-------------------------------------------

   FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
   Bool_t kParameterMerged=kTRUE;
   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
   output->open(dbfile.Data());
   rtdb->setOutput(output);
   
   PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
   if ( fMagField ) {  fieldPar->SetParameters(fMagField); }
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
  
