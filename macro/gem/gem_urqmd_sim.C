// Macro for running tha Panda gems  with Geant4 (R. Kliemt)
void gem_urqmd_sim(Int_t nEvents = 100, TString urqmdString = "pbarC_15_GeV", int verboseLevel = 0)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //FileNames
  TString simOutput;
  simOutput.Form("$VMCWORKDIR/data/Gem_4Stations_%s_n%d",urqmdString.Data(),nEvents);
  TString parOutput=simOutput;
  simOutput+=".root";
  parOutput+="_par.root";
  

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libGem");
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

//  FairDetector *Tpc = new PndTpcDetector("TPC", kFALSE);
//  Tpc->SetGeometryFileName("tpc.geo"); // this is the "long" tpc - conflicts with 4 GEM planes
//  fRun->AddModule(Tpc);

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

  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_4Stations.root");
  Gem->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Gem);




  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  PndUrqmdSmmGenerator* urqmdGen = new PndUrqmdSmmGenerator(Form("/d/panda02/urqmd_smm/%s.root",urqmdString.Data()));
  primGen->AddGenerator(urqmdGen);


  PndMultiField *fField= new PndMultiField();

  PndTransMap *map= new PndTransMap("TransMap", "R");
  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
  fField->AddField(map);
  fField->AddField(map1);
  fField->AddField(map2);

  fRun->SetField(fField);

   // support event display?
   fRun->SetStoreTraj(kFALSE);
   
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

