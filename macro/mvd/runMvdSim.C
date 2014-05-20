// Macro to simulate the MVD in pandaroot
// Updated 30.11.2009
// Ralf Kliemt
runMvdSim(Int_t nEvents=10)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;

  //FileNames
  TString simOutput="Mvd_Test.root";
  TString parOutput="Mvd_Params.root";

  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  // Choose the Geant Navigation System

  fRun->SetOutputFile(simOutput);

  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);
  FairModule *Pipe= new PndPipe("PIPE");
  //fRun->AddModule(Pipe);
  //-------------------------  STT       -----------------
  FairDetector *Stt= new PndStt("STT", kFALSE);
  Stt->SetGeometryFileName("straws_skewed_blocks_pipe_120cm.geo");
  fRun->AddModule(Stt);
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root"); // only sensors, update follows
  Mvd->SetVerboseLevel(verboseLevel);
  fRun->AddModule(Mvd);

  //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryVersion(15);
  // See PndEmc::SetGeometryVersion() for available geometries and add there new one if necessary
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT",kFALSE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  //fRun->AddModule(Muo);
  //-------------------------  DCH       -----------------
  FairDetector *Dch = new PndDchDetector("DCH", kFALSE);
  Dch->SetGeometryFileName("dch.root"); 
  //fRun->AddModule(Dch);
 
  //-------------------------  GEM      -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kFALSE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  //fRun->AddModule(Gem);

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
 FairBoxGenerator *fBox = new FairBoxGenerator(211, 4);
    fBox->SetPRange(0.1,2.5);
    fBox->SetThetaRange(5,150);
    fBox->SetPhiRange(0.,360);
    fBox->SetCosTheta();
    primGen->AddGenerator(fBox);

  //EvtGen Generator
//    FairEvtGenGenerator* evtGen = new
//      FairEvtGenGenerator("/home/ralfk/Pandaroot/pandaroot/macro/mvd/output.evt");
//    primGen->AddGenerator(evtGen);

  // Urqmd  Generator
  //    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  //    primGen->AddGenerator(urqmdGen);

  // DPM Generator
  //PndDpmGenerator* dpmGen = new PndDpmGenerator("/home/kph/stockman/fairroot/cbmroot/pandaroot/pgenerators/DpmEvtGen/Background-micro.root");
  //primGen->AddGenerator(dpmGen);

  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format

	fRun->SetBeamMom(15);
  //---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
  fRun->SetRadLenRegister(kFALSE); // toggle for material budget study

   fRun->Init();

// Set cuts for storing the trajectpries
//   FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//     trajFilter->SetStepSizeCut(0.01); // 1 cm
//     trajFilter->SetVertexCut(-200., -200., -200, 200., 200., 200.);
//     trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//     trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
//     trajFilter->SetStorePrimaries(kTRUE);
//     trajFilter->SetStoreSecondaries(kTRUE);



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

