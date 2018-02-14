// TEST 1: only check EvtGenDirect
// void qa_evtgendirect(){
int QAmacro_evtgen_1()
{
  TStopwatch timer;
  timer.Start();

  TString BaseDir =  gSystem->Getenv("VMCWORKDIR");
  TString DECAYInput = BaseDir + "/pgenerators/EvtGen/EvtGen/Private/DECAY.DEC";
  TString PDLInput = BaseDir + "/pgenerators/EvtGen/EvtGen/Private/evt.pdl";

  Bool_t fTest=kFALSE;
  
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  //fRun->SetName("TGeant4");

  fRun->SetOutputFile("qa_evtgen1.root");

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator* boxGen1 = new FairBoxGenerator(13, 2); // 13 = muon; 1 = multipl.
  boxGen1->SetPRange(0.1,2); // GeV/c
  boxGen1->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen1->SetThetaRange(5., 140.); // Polar angle in lab system range [degree]
  boxGen1->SetXYZ(0., 0., 0.); // mm o cm ??

  FairBoxGenerator* boxGen2 = new FairBoxGenerator(-13, 2); // 13 = muon; 1 = multipl.
  boxGen2->SetPRange(0.1,2); // GeV/c
  boxGen2->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen2->SetThetaRange(5., 140.); // Polar angle in lab system range [degree]
  boxGen2->SetXYZ(0., 0., 0.); // mm o cm ??

  // EvtGen Generator
  PndEvtGenDirect* evtGen = new PndEvtGenDirect("psi(3770)","",7,123456,DECAYInput,PDLInput);
  evtGen->SetStoreTree();

  primGen->AddGenerator(boxGen1);
  primGen->AddGenerator(evtGen);
  primGen->AddGenerator(boxGen2);

 
  // -----   Initialize and run   --------------------------------------------
  fRun->Init();
  fRun->Run(100);// just a few events
  // ------------------------------------------------------------------------

  fTest = kTRUE;


  if (fTest){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }
 timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  //exit(0); 
  return 0;
}  
  
