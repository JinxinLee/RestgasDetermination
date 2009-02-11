void sim_complete(Int_t nEvents=100, Double_t mom=10.0)
{
  TStopwatch timer;

  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");  
  rootlogon();
  
  FairRunSim *fRun = new FairRunSim();
  
  TString parFile = "simparams.root";
    
  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant4");
  
  fRun->SetOutputFile("sim_complete.root");
  
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
  
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);
  
  //FairDetector *Emc = new PndEmc("EMC",kTRUE);
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  //Emc->SetGeometryFileName("emc_module12345.dat"); // if you want to use old geometry for FwEndCap
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
  fRun->AddModule(Emc);
  
  PndMvdDetector *Mvd = new PndMvdDetector("MVD",kTRUE);
  Mvd->SetGeometryFileName("MVD14.root");
  fRun->AddModule(Mvd);
  
  PndStt *Stt= new PndStt("STT",kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);
  
  PndTof *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofSciF.geo");
  fRun->AddModule(Tof);
  
  PndDrc *Drc = new PndDrc("DIRC",kTRUE);
  Drc->SetGeometryFileName("dirc.geo"); 
  //fRun->AddModule(Drc); 
  
  PndDchDetector *Dch = new PndDchDetector("DCH",kTRUE);
  Dch->SetGeometryFileName("dch.root");
  fRun->AddModule(Dch);  
  
  PndMdt *Mdt = new PndMdt("MDT",kTRUE);
  Mdt->SetGeometryFileName("muopars.root");
  Mdt->SetMdtVersion("torino");
  fRun->AddModule(Mdt);

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  PndDpmDirect *dpmGen=new PndDpmDirect(mom,0);
  primGen->AddGenerator(dpmGen);
    
  // fRun->SetStoreTraj(kTRUE); // to store particle trajectories 

  // Field Map Definition
  // --------------------

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
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile.Data());
  rtdb->setOutput(output);
  //PndConstPar* fieldPar = (PndConstPar*) rtdb->getContainer("PndConstPar");
  PndMultiFieldPar* fieldPar = 
    (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if(fField) {  fieldPar->SetParameters(fField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged(kTRUE);
  
  /**All parameters are initialized and ready to be saved*/
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  timer.Start();
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);  
}  

