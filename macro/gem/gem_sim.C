Int_t gem_sim(Int_t nStations, Double_t momentum = 15., Int_t nEvents = 1000, int verboseLevel = 0)
{ 
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return;
  }

  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //FileNames
  TString OutputFile;
  OutputFile.Form("Gem_%dStations_%gGeV_n%d",nStations,momentum,nEvents);
  TString ParOutputFile=OutputFile;
  OutputFile   +=".root";
  ParOutputFile+="_par.root";
  
  TString  SimEngine      = "TGeant4";
  Double_t BeamMomentum   = momentum;
  TString  MediaFile      = "media_pnd.geo";
  gDebug                  =  0;
  //------------------------------------------------------------------

  TStopwatch timer;
  timer.Start();
  
  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName(SimEngine.Data() );
  fRun->SetOutputFile(OutputFile.Data());
  fRun->SetBeamMom(BeamMomentum);
  fRun->SetMaterials(MediaFile.Data());
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
  Gem->SetGeometryFileName(Form("gem_%dStations.root",nStations));
  Gem->SetVerboseLevel(0);
  fRun->AddModule(Gem);
  
  // Event generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
//   FairBoxGenerator* boxGen = new FairBoxGenerator(13,1);
//   boxGen->SetThetaRange(4,4);
//   boxGen->SetPhiRange  (0.,360.);
//   boxGen->SetPRange    (2.,2.);
//   primGen->AddGenerator(boxGen);
   PndDpmDirect* dpmGen = new PndDpmDirect(momentum,1); //0. - only inelastic, 2 only elastic, 1 both
   primGen->AddGenerator(dpmGen);
  
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

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  return 1;
}

