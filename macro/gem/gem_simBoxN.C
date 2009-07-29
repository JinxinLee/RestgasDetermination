void gem_simBoxN(Int_t nparts, Int_t nEvents = 1000,int verboseLevel = 0)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //FileNames
  TString simOutput;
  simOutput.Form("$VMCWORKDIR/data/Gem_4Stations_211_%dpart_n%d",nparts,nEvents);
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

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);
  
  //  FairModule *Magnet= new PndMagnet("MAGNET");
  //  Magnet->SetGeometryFileName("PandaSolenoidV833.root");
  //  fRun->AddModule(Magnet);
  
//   FairModule *dipole= new PndMagnet("MAGNET");
//   dipole->SetGeometryFileName("dipole.geo");
//   fRun->AddModule(dipole);
 
  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root");
  Dch->SetVerboseLevel(0);
  fRun->AddModule(Dch);

  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_4Stations.root");
  Gem->SetVerboseLevel(0);
  fRun->AddModule(Gem);
  
  // Event generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator* boxGen = new FairBoxGenerator(211,nparts);
  boxGen->SetThetaRange(6,19);
  boxGen->SetPhiRange  (0.,360.);
  boxGen->SetPRange    (0.5,5.);
  primGen->AddGenerator(boxGen);

  fRun->SetBeamMom(15.);

  // Field Map Definition
  // --------------------
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
  //-----------end of Bfield stuff

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
  output->open(parOutput.Data());
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

