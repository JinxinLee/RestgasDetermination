// TEST 1: 100 events, 3 mu- and 3 mu+ each
// ptot  = 1 GeV/c
// theta in [20°, 140°]
// phi   in [0°, 360°]

void QAmacro_stt_1()
{
  TStopwatch timer;
  timer.Start();

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  Bool_t fTest=kFALSE;

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile("testrun.root");
 
  // -----   Magnetic field   -------------------------------------------
  // Constant Field
  PndConstField *fMagField = new PndConstField();
  fMagField->SetField(0., 0. ,20. ); // values are in kG
  fMagField->SetFieldRegion(-50, 50,-50, 50, -50, 120);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  fRun->SetField(fMagField);
  // --------------------------------------------------------------------

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);



 FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  // Box Generator
  FairBoxGenerator* boxGenMuM = new FairBoxGenerator(13, 3); // 13 = muon -; 1 = multipl.
  boxGenMuM->SetPRange(1.,1.); // GeV/c //setPRange vs setPtRange
  boxGenMuM->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGenMuM->SetThetaRange(20, 140); // Polar angle in lab system range [degree]
  boxGenMuM->SetCosTheta();//uniform generation on all the solid angle(default)
  // boxGenMuM->SetXYZ(0., 0.37, 0.); 
  primGen->AddGenerator(boxGenMuM);
 

  // Box Generator
  FairBoxGenerator* boxGenMuP = new FairBoxGenerator(-13, 3); // -13 = muon +; 1 = multipl.
  boxGenMuP->SetPRange(1.,1.); // GeV/c //setPRange vs setPtRange
  boxGenMuP->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGenMuP->SetThetaRange(20, 140); // Polar angle in lab system range [degree]
  boxGenMuP->SetCosTheta();//uniform generation on all the solid angle(default)
  // boxGenMuP->SetXYZ(0., 0.37, 0.); 
  primGen->AddGenerator(boxGenMuP);
 
  

  // Fill the Parameter containers for this run
  //-------------------------------------------

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("testparams.root");
  rtdb->setOutput(output);
 


  fRun->Init();
  
 
  Int_t nEvents = 100;
  fRun->Run(nEvents);

  rtdb->saveOutput();
  
  fTest = kTRUE;

  if (fTest){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }
  
  delete fRun;

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  exit(0); 
}  
  
