// Macro for running Fair with Geant3 or Geant4

void runMC(TString base="mc", int nEvents = 1000 )
{

  TStopwatch timer;
  timer.Start();
  gDebug=0;


  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon(); 


  FairRunSim *fRun = new FairRunSim();
  


  // set the MC version used
  // ------------------------
  fRun->SetName("TGeant3");



  // ----------------------------------------------------------------------
  // Set of input file directory
  // Input file (Ascii)
  // Output file (MC events)
  TString outFile = base+".mc.root";
  //Parameter file
  TString parFile = base+".param.root";

  
  fRun->SetOutputFile(outFile);





  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid.root");
  //fRun->AddModule(Magnet);

  //FairModule *Dipole= new PndMagnet("MAGNET");
  //Dipole->SetGeometryFileName("dipole.geo");
  //fRun->AddModule(Dipole);
 
  //FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("pipebeamtarget.geo");
  //fRun->AddModule(Pipe);

  //FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  //Tpc->SetGeometryFileName("tpc.geo");
  //fRun->AddModule(Tpc);
  
  //FairDetector *Stt= new PndStt("STT", kTRUE);
  //Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  //fRun->AddModule(Stt);

  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  /*
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
  fRun->AddModule(Emc);   
  */
  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root"); //don't use this, is not working! Geometry file in /pandaroot/geometry
  Dch->SetVerboseLevel(0);
  fRun->AddModule(Dch);
  
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_4Stations.root");
  Gem->SetVerboseLevel(0);//verboseLevel
  fRun->AddModule(Gem);
  //PndGemSmearingTask* mvdirec = new PndGemSmearingTask(0.1,0.1,0.05);
  //mvdirec->SetVerbose(0);
  //fRun->AddTask(mvdirec);
  

  
  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 1); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(7.,7.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(5, 6.); // Polar angle in lab system range [degree]
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
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
   
  
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile.Data());
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
  
