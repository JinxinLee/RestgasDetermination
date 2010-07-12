// Panda FullSim macro

void runLumi0SimBox(const int nEvents=10, const double mom=8.9, TString storePath="tmpOutput", const int verboseLevel=0, const int particle=-2212)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  //output1
  TString simOutput=storePath+"/Lumi_MC_0.root";
  TString parOutput=storePath+"/Lumi_Params_0.root";

  //Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSds");
  // gSystem->Load("libSdsReco");
  gSystem->Load("libLmd");
  // gSystem->Load("libLmdReco");
  //  gSystem->Load("libLmdTrk");
  FairRunSim *fRun = new FairRunSim();
  cout<<"All libraries succsesfully loaded!"<<endl;

  //set the MC version used
  fRun->SetName("TGeant4");
  //  fRun->SetName("TGeant3");
  
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

  //FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("pipebeamtarget.geo");
  //fRun->AddModule(Pipe);

  /*FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks.geo");
  fRun->AddModule(Stt);*/

  FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  fRun->AddModule(Mvd);
  
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
  Lum->SetGeometryFileName("../macro/lmd/geo/Test-Dipol-Design.root"); //!!!
  //  Lum->SetGeometryFileName("../macro/lmd/geo/MyTest-Dipol-Design.root"); //!!!
   
  //  Mvd->SetGeometryFileName("lumi.geo");
  //   Mvd->SetGeometryFileName("MVD14.root");
  Lum->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Lum);

  //  FairDetector *Stt= new CbmStt("STT", kTRUE);
  //  Stt->SetGeometryFileName("straws_axial.geo");
  //  fRun->AddModule(Stt);
  
  //  FairDetector *Emc = new CbmEmc("EMC",kTRUE);
  //  Emc->SetGeometryFileName("emc_module12345.dat");
  //  fRun->AddModule(Emc);
  
  //  FairDetector *Drc = new CbmDrc("DIRC", kTRUE);
  //  Drc->SetGeometryFileName("dirc.geo");
  //  fRun->AddModule(Drc);

  
  //particle generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Particle Generator (pdgid,mult, px,py,pz, vx,vy,vz)
  
  // single pions for testing
  //      FairParticleGenerator* partGenX   = new FairParticleGenerator(211,1, 1.,0.,0., 0.,0.,0.);
  //      FairParticleGenerator* partGenY   = new FairParticleGenerator(211,1, 0.,1.,0., 0.,0.,0.);
  //      FairParticleGenerator* partGenZ   = new FairParticleGenerator(211,1, 0.,0.1,1., 0.,0.,0.);
  //      FairParticleGenerator* partGenXYZ = new FairParticleGenerator(211,1, 1.,1.,5., 0.,0.,0.);
  //      primGen->AddGenerator(partGenX);
  //      primGen->AddGenerator(partGenY);
  //      primGen->AddGenerator(partGenZ);
  //      primGen->AddGenerator(partGenXYZ);
  
  // Ion Generator
  //    FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  //    primGen->AddGenerator(fIongen);
  
  // Box Generator
  //Pions
  FairBoxGenerator *fBox = new FairBoxGenerator(particle, 1);
  fBox->SetPRange(mom,mom);
  // fBox->SetThetaRange(0.2,0.4); //3...8 mrad
  fBox->SetThetaRange(0.15,0.52); //2...9 mrad
  fBox->SetPhiRange(0.,360.);
  //  fBox->SetThetaRange(0.3,0.3);
  // fBox->SetPhiRange(0.,0.);
  //fBox->SetCosTheta();
  primGen->AddGenerator(fBox);

  //EvtGen Generator
  //    FairEvtGenGenerator* evtGen = new
  //      FairEvtGenGenerator("/home/ralfk/Pandaroot/pandaroot/macro/mvd/output.evt");
  //    primGen->AddGenerator(evtGen);
  
  // Urqmd  Generator
  //    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //    primGen->AddGenerator(urqmdGen);
  
  // DPM Generator
  //  PndDpmGenerator* dpmGen = new PndDpmGenerator("DpmInput/el_100k_aida/el_6_2GeV.root");
  //  primGen->AddGenerator(dpmGen);
  

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

