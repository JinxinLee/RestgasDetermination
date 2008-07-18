// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
void sim_emc(Int_t nEvents = 10, Char_t part[]="e-", Double_t momentum_min = 1.0, Double_t momentum_max = 1.0, Double_t theta_min = 0, Double_t theta_max = 180, Double_t phi_min = 0, Double_t phi_max = 360, Char_t OutputSimFile[] = "sim_emc.root", Char_t OutputDatabaseFile[] = "simparams.root", Char_t TransportModel[] = "TGeant3", UInt_t seed=0)
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
  //gSystem->Load("libMCStack");
  //gSystem->Load("libField");
  //gSystem->Load("libPassive"); // add other detector's lib if you need them
  //gSystem->Load("libEmc");
  //gSystem->Load("libGen");
  
  CbmRunSim *fRun = new CbmRunSim();
  
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

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  //CbmModule *Magnet= new CbmMagnet("MAGNET");
  //Magnet->SetGeometryFileName("magnet.geo");
  //fRun->AddModule(Magnet);

  CbmDetector *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileName("emc_module12345.dat"); 
  fRun->AddModule(Emc);
  
  // Create and Set Event Generator
  //-------------------------------
  
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 

  TDatabasePDG *pdg = new TDatabasePDG();
  Int_t pid = pdg->GetParticle(part)->PdgCode();
  
  CbmBoxGenerator* boxGen = new CbmBoxGenerator(pid, 1);
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

  fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();
  
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
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
  
