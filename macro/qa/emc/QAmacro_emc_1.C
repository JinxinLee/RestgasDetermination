// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
int QAmacro_emc_1()
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();

  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
  Int_t nEvents = 1000; 
  
  // set the MC version used
  // ------------------------
  
   fRun->SetName("TGeant3");
   //fRun->SetName("TGeant4");
  
   fRun->SetOutputFile("sim_emc.root");

   FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
   Bool_t kParameterMerged=kTRUE;
 
   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
   output->open("simparams.root");


   // Set Material file Name
  //-----------------------
   fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

   //FairModule *Magnet= new PndMagnet("MAGNET");
   //Magnet->SetGeometryFileName("magnet.geo");
   //fRun->AddModule(Magnet);

  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);

   //FairDetector *Emc = new PndEmc("EMC",kTRUE);
   PndEmc *Emc = new PndEmc("EMC",kTRUE);
   Emc->SetGeometryVersion(1);
   // See PndEmc::SetGeometryVersion() for available geometries and add there new one if necessary
   fRun->AddModule(Emc);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 
  FairBoxGenerator* boxGen = new FairBoxGenerator(22, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  boxGen->SetPRange(1.,1.); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
   boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(boxGen);  
  
  //fRun->SetStoreTraj(kTRUE); // to store particle trajectories 

  /** Set the field*/
   fRun->SetBeamMom(15);
  	//---------------------Create and Set the Field(s)---------- 
   PndMultiField *fField= new PndMultiField("FULL");
   fRun->SetField(fField);

   fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  
  
    PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
    if (fField) {  Par->SetParameters(fField); }
    Par->setInputVersion(fRun->GetRunId(),1);
    Par->setChanged();
  
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

  cout << " Test passed" << endl;
  cout << " All ok " << endl;  
  exit(0); 
}  

  
