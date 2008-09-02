// Macro created 18/09/2006 by S.Spataro
// It creates a geant simulation file for emc by using an EvtGen outout file
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive"); // add other detector's lib if you need them
  gSystem->Load("libEmc"); 
  gSystem->Load("libGen");
  
  CbmRunSim *fRun = new CbmRunSim();
  
  // Set the number of events
  Int_t nEvents = 1000; 
  
  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant3");
  
  fRun->SetOutputFile("sim_emc.root");
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
  
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryFileNameDouble("emc_module124.dat","emc_module3new.root");
  fRun->AddModule(Emc); 

  // Create and Set Event Generator
  //-------------------------------
  
  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // EvtGen Generator: 

  CbmEvtGenGenerator* evtGen = new CbmEvtGenGenerator("output.evt");
  primGen->AddGenerator(evtGen);  
  
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  
  //magnetic field: no field when commented put
  //CbmFieldConst *fMagField=new CbmFieldConst();
  //fMagField->SetField(0.,0.,20.); // values are in kG
  //fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);// values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  //fRun->SetField(fMagField);
  
  fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open("simparams.root");
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
  
}  
  
