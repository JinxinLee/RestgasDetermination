void run_sim_alldet(Int_t nEvents=10 , Char_t part[]="e-", Double_t momentum_min = 1.0, Double_t momentum_max = 1.0, Double_t theta_min = 0, Double_t theta_max = 180, Double_t phi_min = 0, Double_t phi_max = 360, Int_t RUNID=0, Char_t OutputSimFile[] = "points_sttcombi.root", Char_t OutputDatabaseFile[] = "params_sttcombi.root", Char_t TransportModel[] = "TGeant3" )

{
  
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  gRandom->SetSeed(RUNID);
  
  CbmRunSim *fRun = new CbmRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile(OutputSimFile);

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  CbmModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  CbmModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
 
  CbmModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);
  
  CbmDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);

  CbmDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
  fRun->AddModule(Emc);   
  
  CbmDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);

  /*
  CbmDetector *Muo = new PndMdt("MDT",kTRUE);
   Muo->SetMdtVersion("torino");
  Muo->SetGeometryFileName("muopars.root");
  fRun->AddModule(Muo);
 */

  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc); 
  
  //CbmDetector *Dch = new PndDchDetector("DCH", kTRUE);
  //Dch->SetGeometryFileName("dch.root"); 
  //fRun->AddModule(Dch);
  
  // Create and Set Event Generator
  //-------------------------------

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  TDatabasePDG *pdg = new TDatabasePDG();
  Int_t pid = pdg->GetParticle(part)->PdgCode();
  
  // Box Generator
  CbmBoxGenerator* boxGen = new CbmBoxGenerator(pid, 1); // 13 = muon; 1 = multipl.
//  if (p2<0) p2 = p1;
  boxGen->SetPRange(momentum_min,momentum_max); // GeV/c
  boxGen->SetPhiRange(phi_min, phi_max); // Azimuth angle range [degree]
  boxGen->SetThetaRange(theta_min, theta_max); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kTRUE);
  
  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);
  
  fRun->SetField(fField);
  
  fRun->Init();
  
  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
     
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open("params_sttcombi.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents);
   
  timer.Stop();
   
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  //exit(0);
   
}  

