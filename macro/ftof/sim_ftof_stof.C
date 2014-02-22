// Macro created A.Sanchez
// It creates a geant simulation file for forward tof wall and side tof plates
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libFtof");

  FairRunSim *fRun = new FairRunSim();
  
  
  TString inFile3= "pbarC_3_GeV.root";

  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant4");
  
  
 
  fRun->SetOutputFile("test.root");
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave); 
  
   FairModule *Magnet= new PndMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);
  

  FairModule *Dipole= new PndMagnet("MAGNET");
  Dipole->SetGeometryFileName("dipole.geo");
  fRun->AddModule(Dipole);

  
  FairDetector *FTof = new PndFtof("FTOF",kTRUE);
  FTof->SetGeometryFileName("ftofwall+stof.root");
  fRun->AddModule(FTof);
 // 
 
 //---------  FTS  --------
 FairDetector *Fts= new PndFts("FTS", kTRUE);
 Fts->SetGeometryFileName("fts.geo");
 fRun->AddModule(Fts);    


  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

/*   
  PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile3);
  //primGen->SetTarget(-76.,0.);
  primGen->AddGenerator(AsciiGen);
  
  */

// Box Generator:
 PndBoxGenerator* boxGen = new PndBoxGenerator(3312, 10); // 13 = muon; 1 = multipl. // 211 = pi+
 // first number: PDG particle code: 2nd number: particle multiplicity per event
  boxGen->SetPRange(.2,2.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 15.); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen);


    
    PndMultiField *fField= new PndMultiField("FULL");
    fRun->SetField(fField);
    

       fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
    
   
    fRun->Init();
  
    
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();

  PndMultiFieldPar* fieldPar = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if ( fField ) { fieldPar->SetParameters(fField); }
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  fieldPar->setChanged();

  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("simparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
   
  // Set the number of events
  Int_t nEvents = 100; 
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  delete fRun;
  exit(0);

}  
  
