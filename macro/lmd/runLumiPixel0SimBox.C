// Panda FullSim macro

//void runLumi0SimBox(const int nEvents=10, const double mom=15, TString storePath="tmpOutput", const int verboseLevel=0, const int particle=-211)
int runLumiPixel0SimBox(const int nEvents=10, const int startEv=0, TString storePath="tmpOutput", const int verboseLevel=0, const int particle=-2212, const double mom=15, const int trkNum=1, const int seed=0, const double dP=0)
{

  // ///PROOF lite
  // TProof::Open("");


  gRandom->SetSeed(seed);
  //gRandom->SetSeed(0);
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  mom +=dP;
  cout<<"We start run for beam Mom = "<<mom<<endl;
  //output1
  TString simOutput=storePath+"/Lumi_MC_";
  simOutput +=startEv;
  simOutput +=".root";
 TString parOutput=storePath+"/Lumi_Params_";
 parOutput +=startEv;
 parOutput +=".root";


  // //Load basic libraries
  // gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // gSystem->Load("libSds");
  // gSystem->Load("libLmd");
  FairRunSim *fRun = new FairRunSim();
  cout<<"All libraries succsesfully loaded!"<<endl;

  //set the MC version used
  fRun->SetName("TGeant4");
  //fRun->SetName("TGeant3");//GEANE uses GEANT3!
  
  fRun->SetOutputFile(simOutput);

  //set material
  fRun->SetMaterials("media_pnd.geo");


  //create and add detectors
  FairModule *Cave= new PndCave("CAVE");
  //Cave->SetGeometryFileName("../macro/lmd/pndcaveVAC.geo");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairModule *Pipe= new PndPipe("PIPE");
  //  Pipe->SetGeometryFileName("../macro/lmd/geo/beampipe_201303.root");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  fRun->AddModule(Pipe);
  

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  fRun->AddModule(Magnet);
 
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);


  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  //  Lum->SetGeometryFileName("../macro/lmd/geo/Test-Dipol-Design.root"); //sensors with trap shape
  //  Lum->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design-29052013.root"); // LMD including box etc
  Lum->SetGeometryFileName("Luminosity-Detector.root");
  //Lum->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design-SensorsOnly.root"); // LMD, sensors only
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);
 //  //Other PANDA systems ----
 // //-------------------------  STT       -----------------
 //  PndStt *Stt= new PndStt("STT", kTRUE);
 //  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
 //  fRun->AddModule(Stt);
 //  //-------------------------  MVD       -----------------
 //  PndMvdDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
 //  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
 //  fRun->AddModule(Mvd);
 //  //-------------------------  GEM       -----------------
 //  PndGemDetector *Gem = new PndGemDetector("GEM", kTRUE);
 //  Gem->SetGeometryFileName("gem_3Stations.root");
 //  fRun->AddModule(Gem);
 //  //-------------------------  EMC       -----------------
 //  PndEmc *Emc = new PndEmc("EMC",kTRUE);
 //  Emc->SetGeometryVersion(1);
 //  Emc->SetStorageOfData(kTRUE);
 //  fRun->AddModule(Emc);
 //  //-------------------------  SCITIL    -----------------
 //  PndSciT *SciT = new PndSciT("SCIT",kTRUE);
 //  SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
 //  fRun->AddModule(SciT);
 //  //-------------------------  DRC       -----------------
 //  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
 //  Drc->SetGeometryFileName("dirc_l0_p0_updated.root"); 
 //  Drc->SetRunCherenkov(kTRUE);
 //  fRun->AddModule(Drc); 
 //  //-------------------------  DISC      -----------------
 //  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
 //  Dsk->SetStoreCerenkovs(kTRUE);
 //  Dsk->SetStoreTrackPoints(kTRUE);
 //  fRun->AddModule(Dsk);
 //  // //-------------------------  MDT       -----------------
 //  PndMdt *Muo = new PndMdt("MDT",kTRUE);
 //  Muo->SetBarrel("fast");
 //  Muo->SetEndcap("fast");
 //  Muo->SetMuonFilter("fast");
 //  Muo->SetForward("fast");
 //  Muo->SetMdtMagnet(kTRUE);
 //  Muo->SetMdtMFIron(kTRUE);
 //  fRun->AddModule(Muo);
 //  //-------------------------  FTS       -----------------
 //  PndFts *Fts= new PndFts("FTS",kTRUE);
 //  Fts->SetGeometryFileName("fts.geo");
 //  fRun->AddModule(Fts); 
 //  //-------------------------  FTOF      -----------------
 //  PndFtof *FTof = new PndFtof("FTOF",kTRUE);
 //  FTof->SetGeometryFileName("ftofwall.root");
 //  fRun->AddModule(FTof);
 //  //-------------------------  RICH       ----------------
 //  PndRich *Rich= new PndRich("RICH",kTRUE);
 //  Rich->SetGeometryFileName("rich_v2_shift.geo");
 //  fRun->AddModule(Rich);
 //  // //[END] Other PANDA systems ---

  //particle generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  //  primGen->SetEventMeanTime(100);
  // primGen->SmearVertexXY(kTRUE);
  // primGen->SmearAngle(kTRUE);
  // primGen->SmearVertexZ(kTRUE);
  // primGen->SetBeamAngularDivergence(0.0003);// sigmaTheta~0.3 mrad
  //  primGen->SetBeam(0., 0., 2*0.08, 2*0.08); //sigmaX=sigmaY=0.8 mm ["square"shape of beam ]
  // primGen->SetTarget(0.,0.1); // sigmaZ=1mm, gaus

  //  primGen->SetBeam(0.,0., 2*0.08, 2*0.08); //sigmaX=sigmaY=0.8 mm ["square"shape of beam ]
  // primGen->SetTarget(1050.,0.); //attemp to have different trk topology in lmd

  fRun->SetGenerator(primGen);

  // Box Generator
  FairBoxGenerator *fBox = new FairBoxGenerator(particle, trkNum);
  fBox->SetPRange(mom,mom);
  //  fBox->SetThetaRange(0.52,0.63); // 9 ... 11 mrad
  //  fBox->SetThetaRange(0.12,0.7); // 2... 12 mrad
  //  fBox->SetThetaRange(1.3,1.4); // mrad outside of the detector geometry by purpose
  //fBox->SetThetaRange(0.13,0.65); // 2... 11 mrad
  //fBox->SetThetaRange(0.12,0.65); // 2... 11 mrad
    fBox->SetThetaRange(0.229183, 0.458366); //4 ... 8 mrad
  // //  fBox->SetThetaRange(0.229183,0.31512);//4..5.5 mrad
  // //fBox->SetThetaRange(0.229,0.229);//4..mrad
  // //  fBox->SetThetaRange(0.458366, 0.458366); //!!! 8 mrad
  // //  fBox->SetThetaRange(0., 45.);//TEST 
  // //  fBox->SetPhiRange(90,90.); 
  fBox->SetPhiRange(0,360.);
  // //  fBox->SetPhiRange(0.5,359.5); //FOR missed track check
  // //  fBox->SetPhiRange(0.,20.);//!!! TEST
  // fBox->SetThetaRange(0.344,0.344); //!!! TEST ~ 6 mrad
  // fBox->SetPhiRange(45,45);//TEST
  primGen->AddGenerator(fBox); 

  
 // //EvtGen Generator
  // PndEvtGenDirect *EvtGen = new PndEvtGenDirect("pbarpSystem","PBARSYSTEMTO4PIPHSP.DEC",mom);
  // primGen->AddGenerator(EvtGen);


  // Urqmd  Generator
  //    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //    primGen->AddGenerator(urqmdGen);
  
  // DPM Generator
  //  PndDpmGenerator* dpmGen = new PndDpmGenerator("DpmInput/el_100k_aida/el_6_2GeV.root");
  //  primGen->AddGenerator(dpmGen);
  

  //reading the new field map in the old format
  fRun->SetBeamMom(mom);


  PndMultiField *fField= new PndMultiField("AUTO"); 
  // // //TEST
  // PndMultiField *fField= new PndMultiField();
  // // PndTransMap *map_t = new PndTransMap("TransMap_v1", "R");
  // // PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1_v1", "R");
  // // PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2_v1", "R");

  // PndTransMap *map_t = new PndTransMap("TransMap", "R");
  // PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R");
  // PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R");
  // fField->AddField(map_t);
  // fField->AddField(map_d1);
  // fField->AddField(map_d2);
  // PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
  // PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
  // PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
  // PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");
  // // if(mom<3){ //TODO: solenoid mag.field should be standart
  // //   map_s1->SetScale(0.5);
  // //   map_s2->SetScale(0.5);
  // //   map_s3->SetScale(0.5);
  // //   map_s4->SetScale(0.5);
  // // }
  // fField->AddField(map_s1);
  // fField->AddField(map_s2);
  // fField->AddField(map_s3);
  // fField->AddField(map_s4);
 

  // // //TEST: const dipole field
  // // PndConstField *fDipField=new PndConstField();
  // // fDipField->SetField(0,0.75,0);
  // // fDipField->SetFieldRegion(-240,240,-240,240,400,600); 
  // // fField->AddField(fDipField);
  // // //TEST: const solenoid Mag.Field 
  // // PndConstField *fSolField=new PndConstField();
  // // // fSolField->SetField(0,0,20); // values are in kG //for cross-check with results from maps set Bz=2T
  // // fSolField->SetField(0,0,10); // values are in kG //for cross-check with results from maps set Bz=2T
  // // fSolField->SetFieldRegion(-240,240,-240,240,-172,283.7); //z range is sum from Solenoid#1-#4 maps
  // // fField->AddField(fSolField);
  
  fRun->SetField(fField);
  
  if(nEvents<101)
    fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
  else
    fRun->SetStoreTraj(kFALSE);

  // fRun->SetStoreTraj(kTRUE);
  fRun->Init();

 // -Trajectories Visualization (TGeoManager Only )
 // -----------------------------------------------

 // Set cuts for storing the trajectpries
//   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//     trajFilter->SetStepSizeCut(0.01); // 1 cm
//     trajFilter->SetVertexCut(-200., -200., -200, 200., 200., 200.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//     trajFilter->SetStorePrimaries(kTRUE);
//     trajFilter->SetStoreSecondaries(kTRUE);


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
  return 0;
}

