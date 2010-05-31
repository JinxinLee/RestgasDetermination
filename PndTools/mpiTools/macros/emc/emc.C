void emc(Int_t nEvents = 10, Char_t part[]="e-", Double_t momentum_min = 1.0, Double_t momentum_max = 1.0, Double_t theta_min = 0, Double_t theta_max = 180, Double_t phi_min = 0, Double_t phi_max = 360, Char_t OutputSimFile[] = "sim_emc.root", Char_t OutputDatabaseFile[] = "simparams.root", Char_t TransportModel[] = "TGeant3", UInt_t seed=0, Bool_t savepoints=kFALSE, Bool_t savehits=kFALSE, Bool_t savewaves=kFALSE, Bool_t savedigis=kFALSE, Bool_t saveclusters=kTRUE, Bool_t savebumps=kTRUE)
{
  gRandom->SetSeed(seed);
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands

  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs();

  // Load this example libraries
  //gSystem->Load("libGeoBase");
  //gSystem->Load("libParBase");
  //gSystem->Load("libBase");
  //gSystem->Load("libPndData");
  //gSystem->Load("libField");
  //gSystem->Load("libPassive"); // add other detector's lib if you need them
  //gSystem->Load("libEmc");
  //gSystem->Load("libGen");
  
  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
  //  Int_t nEvents = 10000; 
  
  // set the MC version used
  // ------------------------
  
  fRun->SetName(TransportModel);
  
  fRun->SetOutputFile(OutputSimFile);
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  //FairModule *Magnet= new CbmMagnet("MAGNET");
  //Magnet->SetGeometryFileName("magnet.geo");
  //fRun->AddModule(Magnet);

  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(15);
  Emc->SetStorageOfData(savepoints);
  fRun->AddModule(Emc);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 

  TDatabasePDG *pdg = new TDatabasePDG();
  Int_t pid = pdg->GetParticle(part)->PdgCode();
  
  FairBoxGenerator* boxGen = new FairBoxGenerator(pid, 1);
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  boxGen->SetPRange(momentum_min,momentum_max); // GeV/c
  boxGen->SetPhiRange(phi_min, phi_max); // Azimuth angle range [degree]
  boxGen->SetThetaRange(theta_min, theta_max); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(boxGen);  
  
  fRun->SetStoreTraj(kFALSE); // to store particle trajectories 

  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);

  fRun->SetField(fField);

  // -------- Analysis part ------------------------

  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  emcHitProd->SetStorageOfData(savehits);
  fRun->AddTask(emcHitProd);

  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(0);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(0);
  emcHitsToWaveform->SetStorageOfData(savewaves);
  emcWaveformToDigi->SetStorageOfData(savedigis);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization
 
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(0);
  emcMakeCluster->SetStorageOfData(saveclusters);
  fRun->AddTask(emcMakeCluster);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  emcMakeBump->SetStorageOfData(savebumps);
  fRun->AddTask(emcMakeBump);

  // -------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");

  TString digiFile = "emc.par";
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;

  TString parFile = OutputDatabaseFile; 

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOutput = new FairParRootFileIo(kParameterMerged);
  parOutput->open(parFile.Data());
        
  FairParAsciiFileIo* parIo = new FairParAsciiFileIo();
  parIo->open(emcDigiFile.Data(),"in");
 
  rtdb->setOutput(parOutput); 
  rtdb->setFirstInput(parIo);
  // ----------------------------------------------------------

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
  
