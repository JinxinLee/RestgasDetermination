int QAmacro_gem_1()
{
  //FileNames
  TString OutputFile    = "tstQA.points.root";
  TString ParOutputFile = "tstQA.param.root";
  
  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName("TGeant4");
  fRun->SetOutputFile(OutputFile.Data());
  fRun->SetBeamMom(15.0);
  fRun->SetMaterials("media_pnd.geo");
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
 //---------------------Set Parameter output      ---------- 
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputFile.Data());
  rtdb->setOutput(output);

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);
  
//  FairModule *Magnet= new PndMagnet("MAGNET");
//  Magnet->SetGeometryFileName("FullSolenoid.root");
//  fRun->AddModule(Magnet);
  
  FairModule *dipole= new PndMagnet("MAGNET");
  dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(dipole);
 
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations_Tube.root");
  Gem->SetVerboseLevel(0);
  fRun->AddModule(Gem);
  
  // Event generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  FairBoxGenerator* boxGen = new FairBoxGenerator(13,1);
  boxGen->SetThetaRange(12,12);
  boxGen->SetPhiRange  (0.,360.);
  boxGen->SetPRange    (2.,2.);
  primGen->AddGenerator(boxGen);
  
  //---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  //-----------end of Bfield stuff

  // support event display?
  fRun->SetStoreTraj(kFALSE);
  fRun->SetRadLenRegister(kFALSE);

  fRun->Init();

  // Transport nEvents
  // -----------------

  fRun->Run(100);

  rtdb->saveOutput();
  rtdb->print();

  cout << " Test passed" << endl;
  cout << " All ok " << endl;  

  //delete fRun;
  //return 0; 
  return 0;
}  
  
