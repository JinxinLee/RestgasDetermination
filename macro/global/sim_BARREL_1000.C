void sim_BARREL_1000(Int_t fileId = 0, Int_t nEvents=1000, Int_t pid=13, Int_t nStations = 3) {
  TTimeStamp* timeStamp = new TTimeStamp();
  Int_t newSeed = 1000*(Int_t)(timeStamp->AsDouble())+timeStamp-(Int_t)(timeStamp->AsDouble());
  gRandom->SetSeed(newSeed);
  
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
    return;
  }

  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  TString parOutput  = Form("params_22Part_n%d.root",nEvents);
  TString outputFile = Form("points_22Part_n%d_f%d.root",nEvents,fileId);

  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile(outputFile.Data());

  // Set Material file Name
  //-----------------------

  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  
  FairModule *Pipe= new PndPipe("PIPE");
  fRun->AddModule(Pipe);
  
  //-------------------------  MVD       -----------------
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  //  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root"); // only sensors, update follows
  fRun->AddModule(Mvd);
  //-------------------------  STT       -----------------
  FairDetector *Stt= new PndStt("STT", kTRUE);
  if ( nStations == 3 ) 
    Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  if ( nStations == 4 ) 
    Stt->SetGeometryFileName("straws_skewed_blocks_pipe_120cm.geo");
  fRun->AddModule(Stt);
  //-------------------------  GEM       -----------------
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName(Form("gem_%dStations.root",nStations));
  fRun->AddModule(Gem);
  //-------------------------  DRC       -----------------
//   PndDrc *Drc = new PndDrc("DIRC", kTRUE);
//   Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
//   fRun->AddModule(Drc);
//   //-------------------------  EMC       -----------------
//   PndEmc *Emc = new PndEmc("EMC",kTRUE);
//   Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
//   fRun->AddModule(Emc);  
  //-------------------------  TOF       -----------------  
  //  FairDetector *Tof = new PndTof("TOF",kTRUE);
  // Tof->SetGeometryFileName("tofbarrel.geo");
  //fRun->AddModule(Tof);
  /*  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);*/
  //======================================================

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  FairBoxGenerator *fBox = new FairBoxGenerator(pid, 2);
  fBox->SetPRange(.3,10.);
  fBox->SetThetaRange(2.,100.);
  fBox->SetPhiRange(  0.,360);
  //  fBox->SetCosTheta();
  primGen->AddGenerator(fBox);  
  FairBoxGenerator *fBox2 = new FairBoxGenerator(-pid, 2);
  fBox2->SetPRange(.3,10.);
  fBox2->SetThetaRange(2.,100.);
  fBox2->SetPhiRange(  0.,360);
  //  fBox2->SetCosTheta();
  primGen->AddGenerator(fBox2);  

  fRun->SetBeamMom(15);
  //---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  
  fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
  fRun->SetRadLenRegister(kFALSE); // toggle for material budget study
  
  fRun->Init();

  // Fill the Parameter containers for this run
  //-------------------------------------------
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutput.Data(),"RECREATE");
  rtdb->setOutput(output);

  // Transport nEvents
  // -----------------

  fRun->Run(nEvents);

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
 
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  //exit(0);
   
}  
  
