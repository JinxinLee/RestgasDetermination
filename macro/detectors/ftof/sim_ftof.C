// Macro created A.Sanchez
// It creates a geant simulation file for forward tof wall
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libFtof");

  FairRunSim *fRun = new FairRunSim();
  
  
  TString inFile3= "/d/panda02/urqmd_smm/pbarC_3_GeV.root";

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
  FTof->SetGeometryFileName("ftofwall.root");
  fRun->AddModule(FTof);
 // 
 
   
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

   
  PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile3);
  //primGen->SetTarget(-76.,0.);
  primGen->AddGenerator(AsciiGen);
  
  
    
    PndMultiField *fField= new PndMultiField();
    PndTransMap *map= new PndTransMap("TransMap", "R");
    PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
    PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
    fField->AddField(map);
    fField->AddField(map1);
    fField->AddField(map2);
    fRun->SetField(fField);
    
      /*
       PndConstField *fMagField=new PndConstField();
       fMagField->SetField(0, 0 ,20. ); // values are in kG
       // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
       fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
       fRun->SetField(fMagField);*/

       fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
    
       /*FairTrajFilter* trajFilter = FairTrajFilter::Instance();
	 trajFilter->SetStepSizeCut(0.001); // 1 cm
	 //  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
	 // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
	 //  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
	 trajFilter->SetStorePrimaries(kTRUE);
	 trajFilter->SetStoreSecondaries(kTRUE);*/ // not used for the others.????
   
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
  
