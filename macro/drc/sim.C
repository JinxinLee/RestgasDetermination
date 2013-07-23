void sim(Int_t nEvents=100, TString simFile="sim.root", TString parFile="par.root"){

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

  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kTRUE); // for fast sim Cherenkov -> kFALSE
  // set reflectivity for the mirror at the bar end, in case of kFALSE reflectivity = 1 = const.
  Drc->SetMirrorReal(kTRUE);  
  Drc->StopSecondaries(kTRUE); 
  Drc->SetDetEffAtProduction(kTRUE);
  Drc->SetStopTime(50.);
  Drc->SetVerboseLevel(0);
  Drc->SetOnlyDirectPho(kFALSE);
  Drc->SetGeometryFileName("dirc_g2_l4.root");
  fRun->AddModule(Drc);  

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
  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1);// 211 = pion, 321 = kaon; 1 = multipl.
  boxGen->SetPRange(1,1);
  boxGen->SetPhiRange(10.825, 10.825); // Azimuth angle range [degree]
  boxGen->SetThetaRange(40.,60.);   // Polar a1ngle in lab system range [degree]
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kTRUE); // to store particle trajectories  

  // Create and Set Magnetic Field
  //-------------------------------
  fRun->SetBeamMom(15);
  // PndMultiField *fField= new PndMultiField("FULL");
  // fRun->SetField(fField);

  fRun->Init();
  
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();

  fRun->Run(nEvents); 

  timer.Stop();

  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
