// TEST 1: 100 events, 3 mu- and 3 mu+ each
// ptot  = 1 GeV/c
// theta in [20°, 140°]
// phi   in [0° , 360°]

void QAmacro_stt_1()
{
  TStopwatch timer;
  timer.Start();

  //   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  //   rootlogon();  gRandom->SetSeed();

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
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  //-------------------------  Magnet   ----------------- 
  FairModule *Magnet= new PndMagnet("MAGNET");
  //Magnet->SetGeometryFileName("FullSolenoid_V842.root");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  //-------------------------  Pipe     -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201112.root");
  fRun->AddModule(Pipe);
  //-------------------------  STT       -----------------
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);
  //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);
  //-------------------------  GEM       -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kFALSE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);
  //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC",kFALSE);
  Emc->SetGeometryVersion(1);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);
  //-------------------------  SCITIL    -----------------
  FairDetector *SciT = new PndSciT("SCIT",kFALSE);
  SciT->SetGeometryFileName("barrel-SciTil_07022013.root");
  fRun->AddModule(SciT);
  //-------------------------  DRC       -----------------
  PndDrc *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetGeometryFileName("dirc_l0_p0_updated.root"); 
  Drc->SetRunCherenkov(kFALSE);
  fRun->AddModule(Drc); 
  //-------------------------  DISC      -----------------
  PndDsk* Dsk = new PndDsk("DSK", kFALSE);
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  fRun->AddModule(Dsk);
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT", kFALSE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
  //-------------------------  FTS       -----------------
  FairDetector *Fts= new PndFts("FTS",  kFALSE);
  Fts->SetGeometryFileName("fts.geo");
  fRun->AddModule(Fts); 
  //-------------------------  FTOF      -----------------
  FairDetector *FTof = new PndFtof("FTOF", kFALSE);
  FTof->SetGeometryFileName("ftofwall.root");
  fRun->AddModule(FTof);
  //-------------------------  RICH       ----------------
  FairDetector *Rich= new PndRich("RICH", kFALSE);
  Rich->SetGeometryFileName("rich_v2.geo");
  fRun->AddModule(Rich);

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  // Box Generator
  FairBoxGenerator* boxGenMuM = new FairBoxGenerator(13, 2); // 13 = muon -; 1 = multipl.
  boxGenMuM->SetPRange(1.,1.); // GeV/c //setPRange vs setPtRange
  boxGenMuM->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGenMuM->SetThetaRange(20, 140); // Polar angle in lab system range [degree]
  boxGenMuM->SetCosTheta();//uniform generation on all the solid angle(default)
  // boxGenMuM->SetXYZ(0., 0.37, 0.); 
  primGen->AddGenerator(boxGenMuM);
 

  // Box Generator
  FairBoxGenerator* boxGenMuP = new FairBoxGenerator(-13, 2); // -13 = muon +; 1 = multipl.
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
  
 
  Int_t nEvents = 200;
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
  
