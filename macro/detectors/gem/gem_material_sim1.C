//root macro to run the simulation with RadLen to measure Radiation Length for GEM materials
// After this , run gem_material_ana1.C to draw the histograms , ( x vs y vs ELoss )
//
Int_t gem_material_sim1( Double_t momentum = 15., Int_t nEvents = 1000000, int verboseLevel = 0)
 { 
  //FileNames   
   TString OutputFile = "gem_mat_sim_GemPion.root";
   //TString OutputFile = "gem_mat_sim_GemPipe.root";
   //TString OutputFile = "gem_mat_sim_withoutpipe.root";
   //TString OutputFile = "gem_mat_sim_GPSM.root";
   TString ParOutputFile = "gem_mat_simparams_GemPion.root";
   //TString ParOutputFile = "gem_mat_simparams_GemPipe.root";
   // TString ParOutputFile = "gem_mat_simparams_withoutpipe.root";
   //TString ParOutputFile = "gem_mat_simparams_GPSM.root"; 
  
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
  fRun->SetUseFairLinks(kTRUE);
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  
 //---------------------Set Parameter output------------------------- 
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(ParOutputFile.Data());
  rtdb->setOutput(output);

  // Create and add detectors 
  //-------------------------  CAVE      -----------------------------  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);
  //-------------------------  Magnet   -------------------------------
  //FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  //Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  //fRun->AddModule(Magnet);
  //FairModule *Dipole= new PndMagnet("MAGNET");
  //Dipole->SetGeometryFileName("dipole.geo");
  //fRun->AddModule(Dipole);
  //-------------------------  Pipe     --------------------------
  //FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("beampipe_201309.root");
  //fRun->AddModule(Pipe); 
  // FairModule *Pipe= new PndPipe("PIPE");
  // Pipe->SetGeometryFileName("pipebeamtarget.geo");
  // fRun->AddModule(Pipe);
  //-------------------------  STT       -----------------
  //FairDetector *Stt= new PndStt("STT", kTRUE);
  //Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  //fRun->AddModule(Stt);
  //-------------------------  MVD       -----------------
  //FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  //Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  //fRun->AddModule(Mvd);
  //-------------------------GEM----------------------------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations_realistic_v1.root");
  Gem->SetVerboseLevel(0);
  fRun->AddModule(Gem);
  
  //----------------- Event generator-----------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
    FairBoxGenerator* boxGen = new FairBoxGenerator(211,1);  /// 211 = pion
  //FairBoxGenerator* boxGen = new FairBoxGenerator(13,1);  /// 13 = muon
  //FairBoxGenerator* boxGen = new FairBoxGenerator(22,1);  /// 22 = gamma
  //FairBoxGenerator* boxGen = new FairBoxGenerator(0,1);  /// 0 = geantino
    boxGen->SetThetaRange(0.0001,45);
    boxGen->SetPhiRange  (0.,360.);
    boxGen->SetPRange    (0.05,10.0);
    primGen->AddGenerator(boxGen);
 
    // PndDpmDirect* dpmGen = new PndDpmDirect(momentum,1); //0. - only inelastic, 2 only elastic, 1 both
    // primGen->AddGenerator(dpmGen);
  
  //---------------------Create and Set the Field(s)-------------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  //-----------end of Bfield stuff---------------------------------

  //--------------- support event display?------------------------
  fRun->SetStoreTraj(kFALSE);  
  fRun->SetRadLenRegister(kTRUE);
  fRun->SetRadMapRegister(kTRUE);

  fRun->Init();
  //------------------- Transport nEvents----------------------------
  fRun->Run(nEvents);
  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  return 1;
}

