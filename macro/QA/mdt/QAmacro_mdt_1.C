// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
int QAmacro_mdt_1()
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
 
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
  
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 
 FairBoxGenerator* boxGen = new FairBoxGenerator(-211, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
 boxGen->SetPRange(2.192840326,2.192840326); // GeV/c
// boxGen->SetPtRange(0.798217349,0.798217349); // GeV/c
 boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
 primGen->AddGenerator(boxGen);  
  
 //fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  /** Set the field*/
   fRun->SetBeamMom(15);
   PndMultiField *fField= new PndMultiField();

   PndTransMap *map_t= new PndTransMap("TransMap", "R");
   PndDipoleMap *map_d1= new PndDipoleMap("DipoleMap1", "R");
   PndDipoleMap *map_d2= new PndDipoleMap("DipoleMap2", "R");
   PndSolenoidMap *map_s1= new PndSolenoidMap("SolenoidMap1", "R");
   PndSolenoidMap *map_s2= new PndSolenoidMap("SolenoidMap2", "R");
   PndSolenoidMap *map_s3= new PndSolenoidMap("SolenoidMap3", "R");
   PndSolenoidMap *map_s4= new PndSolenoidMap("SolenoidMap4", "R");

   fField->AddField(map_t);
   fField->AddField(map_d1);
   fField->AddField(map_d2);
   fField->AddField(map_s1);
   fField->AddField(map_s2);
   fField->AddField(map_s3);
   fField->AddField(map_s4);


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
  
  return 0;
}  
  
