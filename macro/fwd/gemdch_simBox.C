void gemdch_simBox(Double_t momentum = 1., Double_t theta = 4., Double_t phi = 20., Int_t nEvents = 1000,int verboseLevel = 0)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //FileNames
  TString simOutput;
  simOutput.Form("GemDch_4Stations_211_%.1fGeV_th%g_ph%g_n%d",momentum,theta,phi,nEvents);
  TString parOutput=simOutput;
  simOutput+=".root";
  parOutput+=".param.root";
  

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
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
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

  FairBoxGenerator* boxGen = new FairBoxGenerator(13,1);
  boxGen->SetThetaRange(theta  ,theta);
  boxGen->SetPhiRange  (phi    ,phi  );
  boxGen->SetPRange    (momentum,momentum);
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

 // Field Map Definition
  // --------------------
  fRun->SetBeamMom(15.);
  PndMultiField *fField= 0;
  fField = new PndMultiField();
  PndTransMap *map_t= new PndTransMap("TransMap", "R");
  PndSolenoidMap *map_s1= new PndSolenoidMap("SolenoidMap1", "R");
  PndSolenoidMap *map_s2= new PndSolenoidMap("SolenoidMap2", "R");
  PndSolenoidMap *map_s3= new PndSolenoidMap("SolenoidMap3", "R");
  PndSolenoidMap *map_s4= new PndSolenoidMap("SolenoidMap4", "R");
  PndDipoleMap *map_d1= new PndDipoleMap("DipoleMap1", "R");
  PndDipoleMap *map_d2= new PndDipoleMap("DipoleMap2", "R");
  fField->AddField(map_t);
  fField->AddField(map_s1);
  fField->AddField(map_s2);
  fField->AddField(map_s3);
  fField->AddField(map_s4);
  fField->AddField(map_d1);
  fField->AddField(map_d2);
  
  fRun->SetField(fField);

   // support event display?
   fRun->SetStoreTraj(kTRUE);
   
   fRun->SetRadLenRegister(kFALSE);


   fRun->Init();

  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);

 PndMultiFieldPar* fieldPar = 
    (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if(fField)  {  fieldPar->SetParameters(fField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged(kTRUE);
  
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

