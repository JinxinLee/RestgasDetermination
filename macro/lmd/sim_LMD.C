//###################################
// # Lmd DPM-Direct Sim macro 
// # for DPM+FairPrimaryGenerator generation
// # 23/07/2015
// # author: A.Karavdina
//###################################
void sim_LMD(const int nEvents=2000, const int startEvent=0, const double mom=1.5, TString storePath="", 
			       const int verboseLevel=0, const int seed=3267, const int mode = 1, const double th_min = 0.12)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //output1
  TString simOutput=storePath+"Lumi_MC_";
  simOutput += startEvent;
  simOutput += ".root";
  TString parOutput=storePath+"Lumi_Params_";
  parOutput += startEvent;
  parOutput += ".root";

  FairRunSim *fRun = new FairRunSim();

  //set the MC version used
  fRun->SetName("TGeant4");
  //fRun->SetName("TGeant3");
  
  fRun->SetOutputFile(simOutput);

  
  //set material
  fRun->SetMaterials("media_pnd.geo");

 // //  //create and add detectors
// // //-------------------------  CAVE      -----------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  //-------------------------  Magnet   ----------------- 
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  fRun->AddModule(Magnet);
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201407.root");
  fRun->AddModule(Pipe);

  //-------------------------  LMD     -----------------
  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  Lum->SetGeometryFileName("Luminosity-Detector.root");
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);


  //particle generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  //DPM
  PndDpmDirect *dpmGen = new PndDpmDirect(mom,mode,seed,th_min);
  primGen->AddGenerator(dpmGen);
  fRun->SetGenerator(primGen);

  //reading the new field map in the old format
  fRun->SetBeamMom(mom);

  PndMultiField *fField= new PndMultiField("AUTO");
  fRun->SetField(fField);
  
  if(nEvents<10)
    fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
  else
    fRun->SetStoreTraj(kFALSE);


  //FairLogger
  // get handle
  FairLogger *logger = FairLogger::GetLogger();
  //
  // log to screen and to file
  logger->SetLogToScreen(kTRUE);
  logger->SetLogToFile(kFALSE);

  logger->SetLogVerbosityLevel("LOW");

  // Set different levels of verbosity. In the example everything >=INFO goes to the
  // file and everything >= ERROR is printed on the screen
  // LogLevels are (FATAL, ERROR, WARNING, INFO, DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG4)
  logger->SetLogScreenLevel("ERROR"); //Only FATAL and ERROR to screen

  fRun->Init();


  // // Fill the Parameter containers for this run
  // //-------------------------------------------
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
