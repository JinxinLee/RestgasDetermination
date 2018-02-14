run_sim(Int_t nEvents=3, Int_t pid=13, Float_t p1=5.0, Float_t p2=-1){
  //2212 proton                                        
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  fRun->SetName("TGeant3");

  fRun->SetOutputFile("points.root");

 // Fill the Parameter containers for this run
  //-------------------------------------------

  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("params.root");
  rtdb->setOutput(output);




  // Set Material file Name
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave);

  FairDetector *Fts= new PndFts("FTS", kTRUE);
  Fts->SetGeometryFileName("fts_reduced.geo");
  fRun->AddModule(Fts);

 // Create and Set Event Generator
 //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

 // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(pid, 1); // 13=muon; 2212=proton 1 = multipl.
  if (p2<0.) p2 = p1;
  boxGen->SetPRange(p1,p2); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0,360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(1, 5); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta();
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kTRUE);
  fRun->SetBeamMom(15.);


  PndMultiField *fField=new PndMultiField("FULL");
  fRun->SetField(fField);



  
  fRun->Init();
  
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
  
