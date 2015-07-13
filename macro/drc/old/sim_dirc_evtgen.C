sim_dirc_evtgen(Int_t nEvents=10)
{



  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  
  TString digiFile = "all.par";
  TString parFile = "params_testrun1.root";
  
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  //fRun->SetName("TGeant4");

  fRun->SetOutputFile("testrun1.root");
 
  // Set the parameters
  //-------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
 
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  Bool_t kParameterMerged=kTRUE;
	
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile);
  rtdb->setOutput(output);
  
  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  //FairModule *Magnet= new PndMagnet("MAGNET");
  ////Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  //fRun->AddModule(Magnet);

  //FairModule *Dipole= new PndMagnet("MAGNET");
  //Dipole->SetGeometryFileName("dipole.geo");
  //fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);

  //FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  //Tpc->SetGeometryFileName("tpc.geo");
  //fRun->AddModule(Tpc);

  //FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  //Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  //fRun->AddModule(Mvd);

  //PndEmc *Emc = new PndEmc("EMC",kTRUE);
  //Emc->SetGeometryVersion(15); 
  //Emc->SetStorageOfData(kFALSE);
  //fRun->AddModule(Emc);
  
  //FairDetector *Tof = new PndTof("TOF",kTRUE);
  //Tof->SetGeometryFileName("tofbarrel.geo");
  //fRun->AddModule(Tof);
  
  //PndMdt *Muo = new PndMdt("MDT",kTRUE);
  //Muo->SetBarrel("torino");
  //Muo->SetEndcap("torino");
  //Muo->SetMuonFilter("torino");
  //Muo->SetMdtMagnet(kTRUE);
  //Muo->SetMdtMFIron(kTRUE);
  //fRun->AddModule(Muo);

  //FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  //Gem->SetGeometryFileName("gem_3Stations.root");
  //fRun->AddModule(Gem);

  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kTRUE); // for fast sim Cherenkov -> kFALSE
  //Drc->SetGeometryFileName("dirc.geo"); 
  fRun->AddModule(Drc);
  
  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);


  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator("output.evt");
  primGen->AddGenerator(evtGen); 


  fRun->SetStoreTraj(kTRUE); // to store particle trajectories  

  // Create and Set Magnetic Field
  //-------------------------------
  fRun->SetBeamMom(15);
  //PndMultiField *fField= new PndMultiField("FULL");
  //fRun->SetField(fField);

  // EMC Hit producer
  //-------------------------------
  //PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  //fRun->AddTask(emcHitProd);
   
  /**Initialize the session*/
  fRun->Init();
  
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  Int_t   nEvents=10; 

  // Transport nEvents
  // -----------------
  fRun->Run(nEvents);

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
