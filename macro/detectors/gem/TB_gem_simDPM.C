Int_t TB_gem_simDPM( Int_t nEvents = 1000, Int_t nStations=3,int verboseLevel = 0)
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
  OutputFile.Form("Gem_MvdStt_%dStations_DPM_n%d",nStations,nEvents);
  TString ParOutputFile=OutputFile;
  OutputFile   +=".root";
  ParOutputFile+="_par.root";
  
  TString  SimEngine      = "TGeant3";
  Double_t BeamMomentum   = 15.0;
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
 
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);

  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  PndGemDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName(Form("gem_%dStations.root",nStations));
  Gem->SetVerboseLevel(0);
  fRun->AddModule(Gem);
  
  // Event generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  PndDpmDirect* dpmGen = new PndDpmDirect(BeamMomentum,1); //0. - only inelastic, 2 only elastic, 1 both
  primGen->AddGenerator(dpmGen);
  
  //---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  //-----------end of Bfield stuff

  // support event display?
  fRun->SetStoreTraj(kFALSE);
  
  fRun->SetRadLenRegister(kFALSE);


  fRun->Init();

  //  Gem->MisalignDetector(); 

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

