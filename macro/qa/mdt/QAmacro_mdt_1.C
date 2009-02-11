// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
void QAmacro_mdt_1()
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libMdt");
  gSystem->Load("libEmc");
  gSystem->Load("libGen");
//  gSystem->Load("libMvd");
//  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");

  gRandom->SetSeed(1235);	
  
  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
  Int_t nEvents = 20;
  
  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant4");
  
  fRun->SetOutputFile("pygen_sim_01_test04_test2.root");
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  PndMdt *Mdt = new PndMdt("MDT",kTRUE);
  Mdt->SetGeometryFileName("muopars.root");
  Mdt->SetMdtVersion("torino");
  fRun->AddModule(Mdt);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 
 FairBoxGenerator* boxGen = new FairBoxGenerator(-211, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
 boxGen->SetPRange(2.192840326,2.192840326); // GeV/c
 boxGen->SetPtRange(0.798217349,0.798217349); // GeV/c
 boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
 primGen->AddGenerator(boxGen);  
  
 //fRun->SetStoreTraj(kTRUE); // to store particle trajectories 

   PndMultiField *fField= new PndMultiField();

//   PndTransMap *map= new PndTransMap("TransMap", "R");
//   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
//   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
//   fField->AddField(map);
//   fField->AddField(map1);
//   fField->AddField(map2);

//    PndMdtBFM* pmb = new PndMdtBFM();

//   fRun->SetField(pmb);

   fRun->SetField(fField);

   fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
   FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
   Bool_t kParameterMerged=kTRUE;
  
  
//    PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
//    if (fField) {  Par->SetParameters(fField); }
//    Par->setInputVersion(fRun->GetRunId(),1);
//    Par->setChanged();
  
   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
   output->open("simparams_piG4_test04_testi2.root");
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

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  exit(0);


  
}  
  
