// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc

sim_emc_test(Int_t nEvents = 10, Float_t mom = 1.){
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString digiFile = "emc.par";
  TString parFile = "simparams.root";
  
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------
  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile("sim_emc.root");

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
  
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  //fRun->AddModule(Pipe);

  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);

  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameTriple("emc_module125.dat","emc_module3new.root","emc_module4_StraightGeo24.4.root"); //MapperVersion: 6
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);

  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);
  
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);

  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);

  //Old EMC geometry
  //FairDetector *Emc = new PndEmc("EMC",kTRUE);
  //PndEmc *Emc = new PndEmc("EMC",kTRUE);
  //Emc->SetGeometryFileName("emc_module12345.dat"); // if you want to use old geometry for FwEndCap & BwEndCap
  //Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); //if you want to use new geometry for FwEndCap
  //Emc->SetGeometryFileNameDouble("emc_module1235.dat","emc_module4_StraightGeo26.root",1); //1 -means: *1235.dat + *.root -> new backward endcap, Mapper==7
  //
  // For testing a new version of Backward End-Cap (BwEndCap):
  // a) with the forward endcap geometry: "emc_module4_FwEndCapGeo.root"
  // b) with straight crystals 26mm x 26mm x 200mm: "emc_module4_StraightGeo26.root"
  // you can use SetGeometryFileName() or SetGeometryFileNameDouble() or SetGeometryFileNameTriple(), 
  // depends on the EMC geometry compination, like below:
  //
  // Emc->SetGeometryFileName("emc_module4_StraightGeo26.root");    //only new BwEndCap, MapperVersion: 6
  // Emc->SetGeometryFileName("emc_module4_StraightGeo26_Al.root"); //only new BwEndCap with dead material in front, MapperVersion: 6
  // Emc->SetGeometryFileNameDouble("emc_module1235.dat","emc_module4_StraightGeo26.root",1); //*1235.dat + _new_ BwEndCap, MapperVersion: 7
  // Emc->SetGeometryFileNameTriple("emc_module125.dat","emc_module3new.root","emc_module4_StraightGeo26.root"); //*125.dat + _new_ FwEndCap + _new_ BwEndCap, MapperVersion: 6
  //
  // MapperVersion you need to set in the "emc.par" file in ../trunk/macro/params/emc.par
  //fRun->AddModule(Emc);


  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 
  PndEvtGen* boxGen = new PndEvtGen();
  primGen->AddGenerator(boxGen);  

  
  // Create and Set Magnetic Field
  //-------------------------------
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

  // EMC Hit producer
  //-------------------------------
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd);
  
  // Set the parameters
  //-------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;
 
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  Bool_t kParameterMerged=kTRUE;
	
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile);
  rtdb->setOutput(output);
  
  /**Initialize the session*/
  fRun->Init();
  
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

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
