// Macro for running Cbm  with Geant3  or Geant4 (M. Al-Turany , D. Bertini)
// Modified 22/06/2005 D.Bertini
void runGemSim(Int_t nEvents = 10,int verboseLevel = 0)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
//  int verboseLevel = 0;
//  Int_t nEvents = 1000;

  //FileNames
  TString simOutput="Gem_Test.root";

//  TString simOutput="Mvd_DPMfixed_4GeV_10000.root";
  TString parOutput="GemParams.root";

  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
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

  FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  Tpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(Tpc);

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
  Gem->SetGeometryFileName("gem.root");
  Gem->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Gem);




  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // DPM Generator
  PndDpmDirect* dpmGen = new PndDpmDirect(6.0,1); //0. - only inelastic, 2 only elastic, 1 both
  primGen->AddGenerator(dpmGen);


//  FairBoxGenerator *fBox2 = new FairBoxGenerator(2212, 5);
//  fBox2->SetPRange(0.9,1.1);
//  fBox2->SetThetaRange(0,50);
//  fBox2->SetPhiRange(0.,360.);
//  //fBox2->SetCosTheta();
//  primGen->AddGenerator(fBox2);


  PndMultiField *fField= new PndMultiField();

//  PndTransMap *map= new PndTransMap("TransMap", "R");
//  PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
  PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
//  fField->AddField(map);
//  fField->AddField(map1);
  fField->AddField(map2);

  fRun->SetField(fField);

   // support event display!
   fRun->SetStoreTraj(kTRUE);



   fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

 // Set cuts for storing the trajectpries
//   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//     trajFilter->SetStepSizeCut(0.01); // 1 cm
//     trajFilter->SetVertexCut(-200., -200., -200, 200., 200., 200.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//     trajFilter->SetStorePrimaries(kTRUE);
//     trajFilter->SetStoreSecondaries(kTRUE);


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

