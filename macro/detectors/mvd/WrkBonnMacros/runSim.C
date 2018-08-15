int runSim(TString runfiles="test.root", Double_t momentum, Int_t nEvents=0, Int_t runSeed=1234)
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  int verboseLevel = 0;
  int evtverbose = 1;
  gRandom->SetSeed(runSeed);
  Int_t eventseed=1111+runSeed; // just take something else then the runseed
  
  // Load basic libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  PndFileNameCreator namecreator(runfiles.Data());
  TString simFile = namecreator.GetSimFileName();
  TString parFile = namecreator.GetParFileName();
  
  FairRunSim *fRun = new FairRunSim();
  
  // set the MC version used
  // ------------------------
  fRun->SetName("TGeant4");
  
  fRun->SetOutputFile(simFile);

  // Set the parameters
  //-------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/all.par";
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parFile.Data(),"RECREATE");
  rtdb->setOutput(output);
  
  
  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
  fRun->AddModule(Magnet);
  
  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);
  
  FairModule *Pipe= new PndPipe("PIPE");
  
  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
  fRun->AddModule(Mvd);
  
  FairDetector *Stt= new PndStt("STT", kTRUE);
  Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
  fRun->AddModule(Stt);

  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(15); 
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);
  
  PndMdt *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetBarrel("torino");
  Muo->SetEndcap("torino");
  Muo->SetMuonFilter("torino");
  Muo->SetMdtMagnet(kTRUE);
  Muo->SetMdtMFIron(kTRUE);
  fRun->AddModule(Muo);
  
  FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
  Gem->SetGeometryFileName("gem_3Stations.root");
  fRun->AddModule(Gem);
  
  PndDsk* Dsk = new PndDsk("DSK", kTRUE);
  Dsk->SetGeometryFileName("dsk.root");
  Dsk->SetStoreCerenkovs(kFALSE);
  Dsk->SetStoreTrackPoints(kFALSE);
  fRun->AddModule(Dsk);
  
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kFALSE);
  fRun->AddModule(Drc);

  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  primGen->SetEventMeanTime(50.); //[ns]

  TString path1 = gSystem->Getenv("VMCWORKDIR");
  TString dedec = path1; dedec+="/pgenerators/EvtGen/DECAY.DEC";
  TString depdl = path1; depdl+="/pgenerators/EvtGen/evt.pdl";
  TString mydec = gSystem->Getenv("PWD"); mydec+="/DplusDminus.DEC";
  
  PndEvtGenDirect* evtGen = new PndEvtGenDirect("pbarpSystem",mydec.Data(),momentum,eventseed,dedec.Data(),depdl.Data());
  evtGen->SetVerbose(evtverbose); // print some stuff
  primGen->AddGenerator(evtGen);
  
  fRun->SetBeamMom(momentum);
  
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
  
  // support event display!
  fRun->SetStoreTraj(kFALSE);
    
  // Add Tasks directly to save some disk space

  // EMC Hit producer
  //-------------------------------
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd);
  
  fRun->Init();
  
  // Transport nEvents
  // -----------------
  
  fRun->Run(nEvents);
  
  rtdb->saveOutput();
  rtdb->print();
  
  delete fRun;
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}

