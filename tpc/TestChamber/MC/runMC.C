// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");

  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");

  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");

  //gSystem->Load("pgenerators/DpmEvtGen/lib/libDpmEvtGen");
  gSystem->Load("libPGen");


  gSystem->ListLibraries();

  Int_t nEvents = 2999;

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("demo.mc.root");

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  

  std::cout<< "Materials set" << std::endl;
  

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairDetector *PndTpc = new PndTpcDetector("TPC", kTRUE);
  PndTpc->SetGeometryFileName("tpcTestChamber.geo");
  fRun->AddModule(PndTpc);
  
   
 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(0.5,0.5); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(17, 17); // Azimuth angle range [degree]
  boxGen->SetThetaRange(40,40); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??

  char * cPath;
  cPath = getenv ("VMCWORKDIR");
  std::string sPath(cPath);
  sPath+="/../workdir/prim.txt";

  FairAsciiGenerator *asciiGen = new FairAsciiGenerator(sPath.c_str());

  primGen->AddGenerator(asciiGen);

  fRun->SetGenerator(primGen);

  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,0. ); // values are in kG
  fMagField->SetFieldRegion(-500, 500,-500, 500, -200, 200);

  // 2- Reading the new field map in the new format


  fRun->SetField(fMagField);
  
  fRun->SetStoreTraj(kTRUE);
  

   fRun->Init();


  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("demo.param.root");
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
  
