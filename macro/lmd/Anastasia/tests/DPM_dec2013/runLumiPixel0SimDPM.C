// Lmd DPM Sim macro
void runLumiPixel0SimDPM(const int nEvents=10, const int startEvent=0, const double mom=15, TString input, TString storePath="tmpOutputDPM", 
const double beam_X0 =0.0, const double beam_Y0 =0.0, const double beam_width_sigma_X =0.0, const double beam_width_sigma_Y =0.0, // beam offset and smearing parameters
const double beam_grad_X =0.0, const double beam_grad_Y =0.0, const double beam_grad_sigma_X =0.0, const double beam_grad_sigma_Y =0.0, // beam gradiant parameters
const int verboseLevel=0)
{
  // gRandom->SetSeed(seed);
  Int_t mode = 1;
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
  fRun->SetName("TGeant4");
  
  fRun->SetOutputFile(simOutput);

  
  //set material
  fRun->SetMaterials("media_pnd.geo");

 //  //create and add detectors
// //-------------------------  CAVE      -----------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  //  Cave->SetGeometryFileName("pndcaveVAC.geo"); //LMD is working in vacuum!!!
  fRun->AddModule(Cave); 
  //-------------------------  Magnet   ----------------- 
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  //  Pipe->SetGeometryFileName("../macro/lmd/geo/beampipe_201303.root");
  //  Pipe->SetGeometryFileName("../macro/lmd/geo/beampipe_201309.root");//test with real vacuum
  fRun->AddModule(Pipe);

  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  //  Lum->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design-29052013.root"); // new sensors
  Lum->SetGeometryFileName("Luminosity-Detector.root");
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);

  //particle generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  
  // DPM Generator
  PndDpmGenerator* dpmGen = new PndDpmGenerator(input);
  primGen->AddGenerator(dpmGen);
 
  fRun->SetGenerator(primGen);

  //reading the new field map in the old format
  fRun->SetBeamMom(mom);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  if(nEvents<100)
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
