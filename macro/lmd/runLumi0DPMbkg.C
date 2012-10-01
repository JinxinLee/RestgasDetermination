// Panda FullSim macro
void runLumi0DPMbkg(const int nEvents=10, const int startEvent=0, const double mom=15, TString storePath="tmpOutputDPM", const int verboseLevel=0, TString input="Background.root")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //output1
  TString simOutput=storePath+"/Lumi_MC_";
  simOutput += startEvent;
  simOutput += ".root";
  TString parOutput=storePath+"/Lumi_Params_";
  parOutput += startEvent;
  parOutput += ".root";
  //Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSds");
  gSystem->Load("libLmd");
  FairRunSim *fRun = new FairRunSim();
  cout<<"All libraries succsesfully loaded!"<<endl;

  //set the MC version used
  //  fRun->SetName("TGeant4");
  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile(simOutput);

  //set material
  fRun->SetMaterials("media_pnd.geo");

  //create and add detectors
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcaveVAC.geo");
  fRun->AddModule(Cave);

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("pipebeamtarget.geo");
  Pipe->SetGeometryFileName("beampipe_201112.root");
  fRun->AddModule(Pipe);

  /*FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);*/

  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  

  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  Lum->SetGeometryFileName("../macro/lmd/geo/Test-Dipol-Design.root"); //!!!
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);

  //particle generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);


  
  // DPM Generator
  PndDpmGenerator* dpmGen = new PndDpmGenerator(input);
  primGen->AddGenerator(dpmGen);
  

  //reading the new field map in the old format
  fRun->SetBeamMom(mom);
  PndMultiField *fField= new PndMultiField();
  
  PndTransMap *map_t = new PndTransMap("TransMap", "R");
  PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R");
  PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R");
  PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
  PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
  PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
  PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");
  
  fField->AddField(map_t);
  fField->AddField(map_d1);
  fField->AddField(map_d2);
  fField->AddField(map_s1);
  fField->AddField(map_s2);
  fField->AddField(map_s3);
  fField->AddField(map_s4);
  
  fRun->SetField(fField);
  
  if(nEvents<101)
    fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
  else
    fRun->SetStoreTraj(kFALSE);
  fRun->Init();


  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();

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

