// Panda FullSim macro

//void runLumi0SimBox(const int nEvents=10, const double mom=15, TString storePath="tmpOutput", const int verboseLevel=0, const int particle=-211)
void runLumiPixel0SimBox(const int nEvents=10, const int startEv=0, const double mom=15, TString storePath="tmpOutput", const int verboseLevel=0, const int particle=-2212, const int trkNum=1, const int seed=0)
{

  // ///PROOF lite
  // TProof::Open("");


  gRandom->SetSeed(seed);
  //gRandom->SetSeed(0);
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  cout<<"We start run for beam Mom = "<<mom<<endl;
  //output1
  TString simOutput=storePath+"/Lumi_MC_";
  simOutput +=startEv;
  simOutput +=".root";
 TString parOutput=storePath+"/Lumi_Params_";
 parOutput +=startEv;
 parOutput +=".root";


  //Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSds");
  gSystem->Load("libLmd");
  FairRunSim *fRun = new FairRunSim();
  cout<<"All libraries succsesfully loaded!"<<endl;

  //set the MC version used
  fRun->SetName("TGeant4");
  //  fRun->SetName("TGeant3");//GEANE uses GEANT3!
  
  fRun->SetOutputFile(simOutput);

  //set material
  fRun->SetMaterials("media_pnd.geo");


  //create and add detectors
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcaveVAC.geo");
  //  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  // FairModule *Pipe= new PndPipe("PIPE");
  // Pipe->SetGeometryFileName("../macro/lmd/geo/beampipe_201210.root");
  // fRun->AddModule(Pipe);
  

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);
 
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);



  /*FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);*/

  // FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  // Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  // fRun->AddModule(Mvd);
  
 /* PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
  fRun->AddModule(Emc);
  
  FairDetector *Tof = new PndTof("TOF",kFALSE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);

  FairDetector *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetGeometryFileName("dirc.geo"); 
  fRun->AddModule(Drc);

  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  Muo->SetGeometryFileName("muopars.root");
  Muo->SetMdtVersion("torino");
  fRun->AddModule(Muo);*/



  PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  //  Lum->SetGeometryFileName("../macro/lmd/geo/Test-Dipol-Design.root"); //sensors with trap shape
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

  // primGen->SetTarget(1050.,0.); //attemp to have different trk topology in lmd

  fRun->SetGenerator(primGen);

  // Box Generator
  FairBoxGenerator *fBox = new FairBoxGenerator(particle, trkNum);
  fBox->SetPRange(mom,mom);
  //  fBox->SetThetaRange(0.13,0.7); // 2... 12 mrad

  //fBox->SetThetaRange(0.13,0.65); // 2... 11 mrad
  // fBox->SetThetaRange(0.229183, 0.458366); //4 ... 8 mrad
  //fBox->SetThetaRange(0.458366, 0.458366); //!!! 8 mrad
  //  fBox->SetThetaRange(0., 45.);//TEST 
  fBox->SetPhiRange(0,360.);
  //  fBox->SetPhiRange(0.5,359.5); //FOR missed track check
  //  fBox->SetPhiRange(0.,20.);//!!! TEST
  //  fBox->SetThetaRange(0.344,0.344); //!!! TEST ~ 6 mrad
  //fBox->SetPhiRange(0,0);//TEST
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


  PndMultiField *fField= new PndMultiField("FULL"); 
  //TODO: change FULL to *_v1 maps
  //  PndMultiField *fField= new PndMultiField();
  // PndTransMap *map_t = new PndTransMap("TransMap_v1", "R");
  // PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1_v1", "R");
  // PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2_v1", "R");
  // PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
  // PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
  // PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
  // PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");
  // fField->AddField(map_t);
  // fField->AddField(map_d1);
  // fField->AddField(map_d2);
  // fField->AddField(map_s1);
  // fField->AddField(map_s2);
  // fField->AddField(map_s3);
  // fField->AddField(map_s4);
  
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
}

