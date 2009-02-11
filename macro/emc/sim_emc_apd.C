// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
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
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive"); // add other detector's lib if you need them
  gSystem->Load("libEmc");
  gSystem->Load("libGen");
  
  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
  Int_t nEvents = 1000; 
  
  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile("sim_emc_apd.root");
  
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
  //fRun->AddModule(Magnet);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  //  fRun->AddModule(Pipe);

  FairDetector *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileName("emc_module1234.dat"); 
  fRun->AddModule(Emc);

  FairDetector *EmcApd = new PndEmcApd("APD",kTRUE);
  EmcApd->SetGeometryFileName("apd_module12.dat"); 
  fRun->AddModule(EmcApd);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  boxGen->SetPRange(1.,1.); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(50., 60.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(boxGen);  
  
  //fRun->SetStoreTraj(kTRUE); // to store particle trajectories 

   PndMultiField *fField= new PndMultiField();

   PndTransMap *map= new PndTransMap("TransMap", "R");
   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
   PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
   fField->AddField(map);
   fField->AddField(map1);
   fField->AddField(map2);


   fRun->SetField(fField);


   fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
   FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
   Bool_t kParameterMerged=kTRUE;
  
  
    PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
    if (fField) {  Par->SetParameters(fField); }
    Par->setInputVersion(fRun->GetRunId(),1);
    Par->setChanged();
  
   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
   output->open("simparams.root");
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
  
