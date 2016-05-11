void sim(Int_t nEvents=10, TString outFile="sim.root", TString parFile="par.root", TString geom="dirc_e3_b3_l6_m40.root", Int_t pdg=321 , Double_t mom=3.5, Double_t theta=140, Double_t phi=10.825){
  
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  TString vmcdir=gSystem->Getenv("VMCWORKDIR");
 
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName("TGeant4");
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetBeamMom(15);
  fRun->SetOutputFile(outFile);
  fRun->SetMaterials("media_pnd.geo");
  fRun->SetUseFairLinks(kTRUE);
  fRun->SetUserConfig(vmcdir+"/macro/drc/g4Config_Cherenkov.C");
  fRun->SetUserCuts(vmcdir+"/macro/drc/g4Cuts.C");
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  // Set the parameters
  //-------------------------------
  TString allDigiFile(vmcdir+"/macro/params/all.par");
   
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);  
      
  Bool_t kParameterMerged=kTRUE;	
  FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
  output->open(parFile.Data());
  rtdb->setOutput(output);

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  //-----------------------  Pipe  -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  fRun->AddModule(Pipe);

  // //-----------------------  MVD  -----------------
  // FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  // Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  // fRun->AddModule(Mvd);
  
  // //-----------------------  STT   -----------------
  // FairDetector *Stt= new PndStt("STT", kTRUE);
  // Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  // fRun->AddModule(Stt);
  
  //-----------------------  DRC  -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kTRUE); // for fast sim Cherenkov -> kFALSE
  Drc->SetMirrorReal(kTRUE);  
  Drc->StopChargedTrackAfterDIRC(kTRUE); 
  Drc->StopSecondaries(kTRUE); 
  Drc->SetTransportEffAtProduction(kTRUE);
  Drc->SetDetEffAtProduction(kTRUE);
  Drc->SetStopTime(150.);
  Drc->SetVerboseLevel(0);
  Drc->SetBlackLensSides(kTRUE);
  Drc->SetOptionForLUT(kFALSE);
  //Drc->SetGeometryFileName("dirc_g1_l6.root");
  Drc->SetGeometryFileName(geom);
  fRun->AddModule(Drc); 
  
  //  //-------------------------  SCITIL    -----------------
  //  FairDetector *SciT = new PndSciT("SCIT",kTRUE);
  //  SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
  //  fRun->AddModule(SciT);
  //  //-------------------------  GEM       -----------------
  //  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  //  Gem->SetGeometryFileName("gem_3Stations.root");
  //  fRun->AddModule(Gem);
  //  //-------------------------  EMC       -----------------
  //  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  //  Emc->SetGeometryVersion(1);
  //  Emc->SetStorageOfData(kFALSE);
  //  fRun->AddModule(Emc);
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
  FairBoxGenerator* boxGen = new FairBoxGenerator(pdg, 1);// 211 = pion, 321 = kaon; 13 = muon-; 1 = multipl.

  // boxGen->SetPRange(0,4);
  // boxGen->SetPhiRange(3.84, 3.84);
  // boxGen->SetPhiRange(3.84, 3.84);
  // boxGen->SetThetaRange(131.53,131.53);

  boxGen->SetPRange(mom,mom);
  boxGen->SetPhiRange(phi, phi);      // Azimuth angle range [degree]
  boxGen->SetThetaRange(theta,theta); // Polar a1ngle in lab system range [degree]
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kTRUE); // to store particle trajectories  

  // Create and Set Magnetic Field
  //-------------------------------
  PndMultiField *fField= new PndMultiField("FULL");
  // fRun->SetField(fField);

  fRun->Init();

  // FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // trajFilter->SetStorePrimaries(kFALSE);
  // trajFilter->SetStoreSecondaries(kTRUE); 

  fRun->Run(nEvents); 

  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
