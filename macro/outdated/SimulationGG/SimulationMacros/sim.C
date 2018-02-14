// Macro created 18/09/2006 by S.Spataro
// It creates a geant simulation file for emc by using an EvtGen outout file
int sim(TString simdatei, TString input, TString param, Int_t nEvents,  Char_t TransportModel[] = "TGeant4", UInt_t seed=0)
{
  gRandom->SetSeed(seed);
  TStopwatch timer;
  timer.Start();
  gDebug=0;
// Load basic libraries
      gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
      rootlogon();

  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
 // Int_t nEvents = n; 
  
  // set the MC version used
  // ------------------------
  
  fRun->SetName(TransportModel);
  
  fRun->SetOutputFile(simdatei);
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root");
  fRun->AddModule(Emc); 

  FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("FullSolenoid.root");
  fRun->AddModule(Magnet);

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);


  FairModule *Pipe= new PndPipe("PIPE");
  //Pipe->SetGeometryFileName("pipebeamtarget.geo");
  fRun->AddModule(Pipe);

 
/*
    FairDetector *Stt = new CbmStt("STT",kFALSE);
    Stt->SetGeometryFileName("stt24.geo"); // 14 = 1 solo 2layer pablo01.geo 3 layers stt24 
    fRun->AddModule(Stt);
*/
  
  /*
    FairDetector *Stt= new CbmStt("STT", kFALSE); 
    Stt->SetGeometryFileName("straws_axial.geo");
    fRun->AddModule(Stt);
  */

  FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  Tpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(Tpc);

    //  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
    //  Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
    //  fRun->AddModule(Mvd);
  
  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");
  fRun->AddModule(Tof);

  FairDetector *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetGeometryFileName("dirc.geo"); 
  fRun->AddModule(Drc); 

 FairDetector *Muo = new PndMdt("MDT",kTRUE);
  Muo->SetGeometryFileName("muopars.root");
  fRun->AddModule(Muo);

  FairDetector *Dch = new PndDchDetector("DCH", kTRUE);
  Dch->SetGeometryFileName("dch.root"); 
  fRun->AddModule(Dch);
 


  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // EvtGen Generator: 

  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(input);
  primGen->AddGenerator(evtGen);  
  
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  
  //magnetic field: no field when commented put
  //FairFieldConst *fMagField=new FairFieldConst();
  //fMagField->SetField(0.,0.,20.); // values are in kG
  //fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  //fRun->SetField(fMagField);
  
  fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(param);
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
  
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 0;
}  
 
