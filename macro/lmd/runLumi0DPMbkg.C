// Panda FullSim macro
int runLumi0DPMbkg(const int nEvents=10, const int startEvent=0, const double mom=15, TString storePath="tmpOutputDPM", const int verboseLevel=0, TString input="Background.root")
//void runLumi0DPMbkg(const int nEvents=10, const int startEvent=0, const double mom=15, Int_t mode =1, UInt_t seed=0, TString storePath="tmpOutputDPM", const int verboseLevel=0)
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
  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile(simOutput);

  
  //set material
  fRun->SetMaterials("media_pnd.geo");

 //  //create and add detectors
// //-------------------------  CAVE      -----------------

  FairModule *Cave= new PndCave("CAVE");
  // Cave->SetGeometryFileName("pndcave.geo");
  Cave->SetGeometryFileName("pndcaveVAC.geo"); //LMD is working in vacuum!!!
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
  Pipe->SetGeometryFileName("beampipe_201210.root");
  fRun->AddModule(Pipe);
//   //-------------------------  STT       -----------------
//   FairDetector *Stt= new PndStt("STT", kTRUE);
//   Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
//   fRun->AddModule(Stt);
//   //-------------------------  MVD       -----------------
//   FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
//   Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
//   fRun->AddModule(Mvd);
//   //-------------------------  GEM       -----------------
//   FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
//   Gem->SetGeometryFileName("gem_3Stations.root");
//   fRun->AddModule(Gem);
//   //-------------------------  EMC       -----------------
//   PndEmc *Emc = new PndEmc("EMC",kTRUE);
//   Emc->SetGeometryVersion(1);
//   Emc->SetStorageOfData(kFALSE);
//   fRun->AddModule(Emc);
//   //-------------------------  DRC       -----------------
//   PndDrc *Drc = new PndDrc("DIRC", kTRUE);
//   Drc->SetGeometryFileName("dirc_l0_p0_updated.root"); 
//   Drc->SetRunCherenkov(kFALSE);
//   fRun->AddModule(Drc); 
//   //-------------------------  DISC      -----------------
//   PndDsk* Dsk = new PndDsk("DSK", kTRUE);
//   Dsk->SetStoreCerenkovs(kFALSE);
//   Dsk->SetStoreTrackPoints(kFALSE);
//   fRun->AddModule(Dsk);
//   //-------------------------  MDT       -----------------
//   PndMdt *Muo = new PndMdt("MDT",kTRUE);
//   Muo->SetBarrel("fast");
//   Muo->SetEndcap("fast");
//   Muo->SetMuonFilter("fast");
//   Muo->SetForward("fast");
//   Muo->SetMdtMagnet(kTRUE);
//   Muo->SetMdtMFIron(kTRUE);
//   fRun->AddModule(Muo);
//   //-------------------------  FTS       -----------------
//   FairDetector *Fts= new PndFts("FTS", kTRUE);
//   Fts->SetGeometryFileName("fts.geo");
//   fRun->AddModule(Fts); 
//   //-------------------------  FTOF      -----------------
//   FairDetector *FTof = new PndFtof("FTOF",kTRUE);
//   FTof->SetGeometryFileName("ftofwall.root");
//   fRun->AddModule(FTof);
//   //-------------------------  RICH       ----------------
//   FairDetector *Rich= new PndRich("RICH",kFALSE);
//   Rich->SetGeometryFileName("rich_v2.geo");
//   fRun->AddModule(Rich);

  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  Lum->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design.root"); // new sensors
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);

  //particle generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  // primGen->SmearVertexXY(kTRUE);
  // primGen->SmearAngle(kTRUE);
  // primGen->SmearVertexZ(kTRUE);
  // primGen->SetBeamAngularDivergence(0.0003);// sigmaTheta~0.3 mrad
  // primGen->SetBeam(0., 0., 2*0.08, 2*0.08); //sigmaX=sigmaY=0.8 mm ["square"shape of beam ]
  // primGen->SetTarget(0.,0.1); // sigmaZ=1mm, gaus
  fRun->SetGenerator(primGen);

  
  // DPM Generator
  PndDpmGenerator* dpmGen = new PndDpmGenerator(input);
  primGen->AddGenerator(dpmGen);
  // PndDpmDirect *dpmGen = new PndDpmDirect(mom, mode, gRandom->GetSeed(), 0.1);
  // primGen->AddGenerator(dpmGen);

  //reading the new field map in the old format
  fRun->SetBeamMom(mom);
  // PndMultiField *fField= new PndMultiField("FULL");
  // fRun->SetField(fField);
  PndMultiField *fField= new PndMultiField();
  PndTransMap *map_t = new PndTransMap("TransMap_v1", "R");
  PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1_v1", "R");
  PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2_v1", "R");
  // PndTransMap *map_t = new PndTransMap("TransMap", "R");
  // PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R");
  // PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R");
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


  // // Fill the Parameter containers for this run
  // //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);

  // PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  // if (fField) {  Par->SetParameters(fField); }
  // Par->setInputVersion(fRun->GetRunId(),1);
  // Par->setChanged();

  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}

