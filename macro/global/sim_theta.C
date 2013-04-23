void sim_theta(Int_t nEvents=1000, Int_t pid=13, Double_t theta=5., Int_t nStations = 3) {
  
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

  TString parOutput = Form("params_%dStations_p%d_theta%.1f.root",nStations,pid,theta);

  TString outputFile = Form("points_%dStations_p%d_theta%.1f.root",nStations,pid,theta);



  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant4");
  
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
  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
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
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
  fRun->AddModule(Drc);
  //-------------------------  EMC       -----------------
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(1); 
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);
  //-------------------------  TOF       -----------------  
  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  FTof->SetGeometryFileName("ftofwall.root");
  fRun->AddModule(FTof);
  //-------------------------  MDT       -----------------
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("fast");
  Muo->SetEndcap("fast");
  Muo->SetMuonFilter("fast");
  Muo->SetForward("fast");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
  //======================================================

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  FairBoxGenerator *fBox = new FairBoxGenerator(pid, 5);
  fBox->SetPRange(1.,1.);
  fBox->SetThetaRange(theta,theta);
  fBox->SetPhiRange(0.,360);
  fBox->SetCosTheta();
  primGen->AddGenerator(fBox);  
  
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
  
