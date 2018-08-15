// Macro for running Simulation of the Full Panda Detector with Geant4 (M. Al-Turany )
// Modified 13/02/2009
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  FairRunSim *fRun = new FairRunSim();
  // set the MC version used
  // ------------------------
  fRun->SetName("TGeant4");
  
  fRun->SetOutputFile("simfullG4.root");

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

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

  FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  Tpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(Tpc);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
  fRun->AddModule(Emc);
  
  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);

  FairDetector *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc.geo"); 
  fRun->AddModule(Drc); 

  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
  

  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root"); 
  fRun->AddModule(Dch);
 
  /** Create and Set Event Generator */
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  gRandom->SetSeed(3523);
  FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 3); // 13=muon; 2212=proton 1 = multipl.
  boxGen->SetPRange(1.,10.); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(-180,180); // Azimuth angle range [degree]
  boxGen->SetThetaRange(1, 5); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  FairBoxGenerator* boxGen1 = new FairBoxGenerator(22, 10); // 13 = muon; 1 = multipl. // 211 = pi+
   boxGen1->SetPRange(1.,1.); // GeV/c
   boxGen1->SetPhiRange(0., 360.); // Azimuth angle range [degree]
   boxGen1->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
   boxGen1->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
   primGen->AddGenerator(boxGen1);  

   FairBoxGenerator* boxGen2 = new FairBoxGenerator(-211, 5); // 13 = muon; 1 = multipl. // 211 = pi+
   boxGen2->SetPRange(2.192840326,2.192840326); // GeV/c
   boxGen2->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
   primGen->AddGenerator(boxGen2);  

   FairBoxGenerator *fBox = new FairBoxGenerator(211, 5);
   fBox->SetPRange(0.1,2.5); // GeV/c
   fBox->SetPhiRange(0.,360); // Azimuth angle range [degree]
   fBox->SetThetaRange(5,150); // Polar angle in lab system range [degree]
   fBox->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
   primGen->AddGenerator(fBox);


   /** Set the field*/
   fRun->SetBeamMom(15);
   
  //---------------------Create and Set the Field(s)---------- 
   PndMultiField *fField= new PndMultiField("FULL");
   fRun->SetField(fField);
   
   fRun->Init();
 
  // 
  //   // Fill the Parameter containers for this run
  //   //-------------------------------------------
  //      
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("g4params.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  // -----------------1
  
  Int_t nEvents = 1;
  fRun->Run(nEvents);
     
  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  delete fRun;
  return 0;

}  
  
