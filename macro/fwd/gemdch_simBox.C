void gemdch_simBox(Double_t momentum = 2., Double_t theta = 10., Double_t phi = 20., Int_t nEvents = 1000,int verboseLevel = 0)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //FileNames
  TString simOutput;
  simOutput.Form("$VMCWORKDIR/data/GemDch_4Stations_211_%.1fGeV_th%g_ph%g_n%d",momentum,theta,phi,nEvents);
  TString parOutput=simOutput;
  simOutput+=".root";
  parOutput+="_par.root";
  

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(simOutput);

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

//   FairModule *Magnet= new PndMagnet("MAGNET");
//   Magnet->SetGeometryFileName("FullSolenoid.root");
//   fRun->AddModule(Magnet);

//  FairModule *Dipole= new PndMagnet("MAGNET");
//  Dipole->SetGeometryFileName("dipole.geo");
//  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
//   Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

   FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
   Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
   Mvd->SetVerboseLevel(verboseLevel);
   fRun->AddModule(Mvd);

//   FairDetector *Tpc = new PndTpcDetector("TPC", kFALSE);
//   Tpc->SetGeometryFileName("tpc.elsa.geo"); // this is the "long" tpc - conflicts with 4 GEM planes
//   fRun->AddModule(Tpc);

//  PndEmc *Emc = new PndEmc("EMC",kTRUE);
//  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
//   fRun->AddModule(Emc);

//  FairDetector *Tof = new PndTof("TOF",kTRUE);
//  Tof->SetGeometryFileName("tofbarrel.geo");
//   fRun->AddModule(Tof);

//  FairDetector *Muo = new PndMdt("MDT",kTRUE);
//  Muo->SetGeometryFileName("muopars.root");
//   fRun->AddModule(Muo);

//  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
//  Drc->SetRunCherenkov(kFALSE);
  //Drc->SetGeometryFileName("dirc.geo");
//   fRun->AddModule(Drc);

   FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
   Dch->SetGeometryFileName("dch.root");
   fRun->AddModule(Dch);
   
   FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
   Gem->SetGeometryFileName("gem_4Stations.root");
   Gem->SetVerboseLevel(verboseLevel);
   fRun->AddModule(Gem);
   



  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator* boxGen = new FairBoxGenerator(211,1);
  boxGen->SetThetaRange(theta   -0.01,theta   +0.01);
  boxGen->SetPhiRange  (phi     -0.01,phi     +0.01);
  boxGen->SetPRange    (momentum-0.01,momentum+0.01);
  primGen->AddGenerator(boxGen);

    PndMultiField *fField= new PndMultiField();
    
    PndTransMap *map= new PndTransMap("TransMap", "R");
   PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
   fField->AddField(map);
   fField->AddField(map1);
  fField->AddField(map2);

  fRun->SetField(fField);

   // support event display?
   fRun->SetStoreTraj(kTRUE);
   
   fRun->SetRadLenRegister(kFALSE);


   fRun->Init();
//   Tpc->Initialize(); // tpc produces too much points, use radlenpoints instead

  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);

  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

