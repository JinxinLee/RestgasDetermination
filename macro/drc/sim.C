void sim(Int_t nEvents=10, TString simFile="sim.root", TString parFile="par.root", Double_t theta=30, Double_t phi=10.825, Int_t pdg=13){

  TStopwatch timer;
  timer.Start();
  gDebug=0;
  
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  TString digiFile = "all.par";
  
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------
  //fRun->SetName("TGeant3");
  fRun->SetName("TGeant4");

  fRun->SetOutputFile(simFile);
 
  // Set the parameters
  //-------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);  
      
  Bool_t kParameterMerged=kTRUE;	
  FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
  output->open(parFile);
  rtdb->setOutput(output);
  
  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  //-----------------------  Pipe  -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);
  // //-----------------------  STT   -----------------
  // FairDetector *Stt= new PndStt("STT", kTRUE);
  // Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  // fRun->AddModule(Stt);
  // //-----------------------  MVD  -----------------
  // FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  // Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  // fRun->AddModule(Mvd);
  
  //-----------------------  DRC  -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kTRUE); // for fast sim Cherenkov -> kFALSE
  Drc->SetMirrorReal(kFALSE);  
  Drc->StopChargedTrackAfterDIRC(kTRUE); 
  Drc->StopSecondaries(kFALSE); 
  Drc->SetTransportEffAtProduction(kTRUE);
  Drc->SetDetEffAtProduction(kTRUE);
  Drc->SetStopTime(150.);
  Drc->SetVerboseLevel(0);
  Drc->SetOnlyDirectPho(kFALSE);
  Drc->SetBlackLensSides(kTRUE);
  Drc->SetOptionForLUT(kFALSE);
  Drc->SetGeometryFileName("dirc_g1_l6.root");
  fRun->AddModule(Drc); 


 // //-------------------------  STT       -----------------
 //  FairDetector *Stt= new PndStt("STT", kTRUE);
 //  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
 //  fRun->AddModule(Stt);
 //  //-------------------------  MVD       -----------------
 //  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
 //  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
 //  fRun->AddModule(Mvd);
 //  //-------------------------  GEM       -----------------
 //  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
 //  Gem->SetGeometryFileName("gem_3Stations.root");
 //  fRun->AddModule(Gem);
 //  //-------------------------  EMC       -----------------
 //  PndEmc *Emc = new PndEmc("EMC",kTRUE);
 //  Emc->SetGeometryVersion(1);
 //  Emc->SetStorageOfData(kFALSE);
 //  fRun->AddModule(Emc);
  // //-------------------------  SCITIL    -----------------
  // FairDetector *SciT = new PndSciT("SCIT",kTRUE);
  // SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
  // fRun->AddModule(SciT);
 
 //  //-------------------------  DISC      -----------------
 //  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
 //  Dsk->SetStoreCerenkovs(kFALSE);
 //  Dsk->SetStoreTrackPoints(kFALSE);
 //  fRun->AddModule(Dsk);
 //  //-------------------------  MDT       -----------------
 //  PndMdt *Muo = new PndMdt("MDT",kTRUE);
 //  Muo->SetBarrel("fast");
 //  Muo->SetEndcap("fast");
 //  Muo->SetMuonFilter("fast");
 //  Muo->SetForward("fast");
 //  Muo->SetMdtMagnet(kTRUE);
 //  Muo->SetMdtMFIron(kTRUE);
 //  fRun->AddModule(Muo);
 //  //-------------------------  FTS       -----------------
 //  FairDetector *Fts= new PndFts("FTS", kTRUE);
 //  Fts->SetGeometryFileName("fts.geo");
 //  fRun->AddModule(Fts); 
 //  //-------------------------  FTOF      -----------------
 //  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
 //  FTof->SetGeometryFileName("ftofwall.root");
 //  fRun->AddModule(FTof);
 //  //-------------------------  RICH       ----------------
 //  FairDetector *Rich= new PndRich("RICH",kFALSE);
 //  Rich->SetGeometryFileName("rich_v2_shift.geo");
 //  fRun->AddModule(Rich);

  // Set Random Number seed
  Int_t rndm=0;
  if (gSystem->Getenv("RANDOM")) {
    rndm = atoi(gSystem->Getenv("RANDOM"));
  }
  gRandom->SetSeed(rndm); // Set 0 to use the current time
  cout<<"Seed for random number generation= "<<gRandom->GetSeed()<<endl;

  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(321, 1);// 211 = pion, 321 = kaon; 13 = muon-; 1 = multipl.
  boxGen->SetPRange(3,3);
  boxGen->SetPhiRange(phi, phi);      // Azimuth angle range [degree]
  boxGen->SetThetaRange(theta,theta); // Polar a1ngle in lab system range [degree]  
  boxGen->SetXYZ(0.,0.,0.);

  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kFALSE); // to store particle trajectories  

  // Create and Set Magnetic Field
  //-------------------------------
  fRun->SetBeamMom(15);
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

  fRun->Init();

  // FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // trajFilter->SetStorePrimaries(kFALSE);
  // trajFilter->SetStoreSecondaries(kTRUE);
  
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents); 

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
