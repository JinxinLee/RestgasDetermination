void simLut(Int_t nEvents=20, Int_t barId = 2, TString simFile="simlut.root", TString parFile="parlut.root", Int_t bars=5, TString geom="dirc_e3_b5_l6.root"){
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  TString digiFile = "all.par";
  TString vmcdir=gSystem->Getenv("VMCWORKDIR");
  
  FairRunSim *fRun = new FairRunSim();

  fRun->SetName("TGeant4");
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetOutputFile(simFile);
  fRun->SetMaterials("media_pnd.geo");
  fRun->SetUseFairLinks(kTRUE);
  fRun->SetUserConfig(vmcdir+"/macro/drc/g4Config_Cherenkov.C");
 
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

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kTRUE); // for fast sim Cherenkov -> kFALSE
  Drc->SetMirrorReal(kTRUE);  
  Drc->StopSecondaries(kTRUE); 
  Drc->SetDetEffAtProduction(kFALSE);
  Drc->SetStopTime(50);
  Drc->SetVerboseLevel(0);
  Drc->SetOnlyDirectPho(kFALSE);
  Drc->SetBlackLensSides(kTRUE);
  Drc->SetOptionForLUT(kTRUE);
  Drc->SetGeometryFileName(geom);
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

  Double_t x5[] = { 45.543481, 46.148229, 46.752977, 47.357725, 47.962473 };
  Double_t y5[] = { 15.265154, 12.102453,  8.939751,  5.777050,  2.614349 };

  Double_t x3[] = {45.748, 46.752977,47.775 };
  Double_t y3[] = {14.26, 8.939751, 3.591 };

  FairBoxGenerator* boxGen = new FairBoxGenerator(50000050, 1000);
  boxGen->SetPRange(3.18e-9,3.18e-9); // GeV/c //3.18eV <==> 390nm
  //boxGen->SetPhiRange(0,360);
  boxGen->SetThetaRange(90, 180); 
  //boxGen->SetThetaRange(0, 180); 
  boxGen->SetCosTheta();
  //boxGen->SetBoxXYZ(46.3 ,7.4, 47.3,10.4, -118.9+0.01+60);
  // boxGen->SetXYZ(46.8,8.9,-119+0.01); 
  if(bars==3)  boxGen->SetXYZ(x3[barId],y3[barId],-119+0.0001);
  if(bars==5)  boxGen->SetXYZ(x5[barId],y5[barId],-119+0.0001);
  primGen->AddGenerator(boxGen);
  
  // // Box Generator
  // FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1);// 211 = pion, 321 = kaon; 13 = muon-; 1 = multipl.
  // boxGen->SetPRange(3,3);
  // boxGen->SetPhiRange(10.825, 10.825);  // Azimuth angle range [degree]
  // boxGen->SetThetaRange(87.5,87.5);  // Polar a1ngle in lab system range [degree]
  // boxGen->SetXYZ(0.,0.,0.);
  // primGen->AddGenerator(boxGen);

  //fRun->SetStoreTraj(kTRUE);

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
