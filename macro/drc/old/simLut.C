void simLut(Int_t nEvents=100, TString simFile="simlut.root", TString parFile="parlut.root"){

  TStopwatch timer;
  timer.Start();
  gDebug=0;
  
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  TString digiFile = "all.par";
  
  FairRunSim *fRun = new FairRunSim();

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
  Drc->SetMirrorReal(kTRUE);  
  Drc->StopSecondaries(kTRUE); 
  Drc->SetDetEffAtProduction(kTRUE);
  Drc->SetVerboseLevel(0);
  Drc->SetOnlyDirectPho(kFALSE);
  Drc->SetGeometryFileName("dirc_l4_p0_R2.root");
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

 
  FairBoxGenerator* boxGen = new FairBoxGenerator(50000050, 1000);
  boxGen->SetPRange(3.e-9,3.e-9); // GeV/c
  boxGen->SetPhiRange(21.6/2.-180,21.6/2.+180);
  //boxGen->SetThetaRange(-70-180, 70-180); 
  boxGen->SetThetaRange(-115, 180); 
  boxGen->SetCosTheta();
  boxGen->SetBoxXYZ(46.3 ,7.4, 47.3,10.4, -119.4);
  primGen->AddGenerator(boxGen);

  //fRun->SetStoreTraj(kTRUE); // to store particle trajectories  

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
