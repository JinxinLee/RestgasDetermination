// Macro perform simulation-digitization-reconstruction of EMC for calculation of error matrix 
error_matrix_data_production(Int_t emcGeometry=17, Int_t emc_component=1, Int_t nEvents=10){
// emc_component: 1 - barrel, 2 - fwd endcap, 3 - bwd endcap, 4 - shashlyk 
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  FairRunSim *fRun = new FairRunSim();
 	
  TString inparfile = "emc.par";
  TString outparFile = "simparams.root";
  
  TString outFile="emc_data_";
  outFile+=emcGeometry;
  outFile+="_";
  outFile+=emc_component;
  outFile+=".root";
  
  // Set energy and theta range for the given EMC component 
  Double_t e_min, e_max, theta_min, theta_max;
  switch (emc_component)
  {
	  case 1:
		  e_min = 0.1; e_max=5.0; theta_min=25.0; theta_max=135.0;
		  break;
	  case 2:
		  e_min = 0.1; e_max=7.0; theta_min=10.0; theta_max=20.0;
		  break;
	  case 3:
		  e_min = 0.1; e_max=7.0; theta_min=155.0; theta_max=165.0;
		  break;
	  case 4:
		  e_min = 0.5; e_max=7.0; theta_min=0.0; theta_max=5.0;
		  break;
	  default:
		  std::cout<<"Wrong EMC component"<<std::endl;
		  exit(0); 
  } 
  
	
  // set the MC version used
  // ------------------------
	
  fRun->SetName("TGeant3");
  //fRun->SetName("TGeant4");
	
  fRun->SetOutputFile(outFile.Data());
	
  /**Set the digitization parameters */
	
  TString emcInparfile = gSystem->Getenv("VMCWORKDIR");
  emcInparfile += "/macro/params/";
  emcInparfile += inparfile;
  /**Get the run time data base for this session and set the needed input*/
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcInparfile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  Bool_t kParameterMerged=kTRUE;
	
  /**Parameters created for this simulation goes to the out put*/
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(outparFile.Data());
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
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);
	
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("pipe.geo");
  fRun->AddModule(Pipe);
 
 //-------------------------  EMC       -----------------

  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(emcGeometry);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);
  
  // Create and Set Event Generator
  //-------------------------------
	
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
	
  // Box Generator: 
  FairBoxGenerator* boxGen = new FairBoxGenerator(22, 1);
  boxGen->SetPRange(e_min,e_max); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(theta_min, theta_max); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
  primGen->AddGenerator(boxGen);  
	
  // fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
  fRun->SetBeamMom(15);	
 	//---------------------Create and Set the Field(s)---------- 
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);
	
  /**
     Add Hit producer task to the simulation 
  */
  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  emcHitProd->SetStorageOfData(kFALSE);
  fRun->AddTask(emcHitProd);

  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(0);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(0);
  emcHitsToWaveform->SetStorageOfData(kFALSE);
  //emcWaveformToDigi->SetStorageOfData(kFALSE);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization
 
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(0);
  //emcMakeCluster->SetStorageOfData(kFALSE);
  fRun->AddTask(emcMakeCluster);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  //emcMakeBump->SetStorageOfData(kFALSE);
  fRun->AddTask(emcMakeBump);

	
  /**Initialize the session*/
  fRun->Init();
  /**After initialization now we can save the field parameters */
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();
	
  rtdb->saveOutput();
  //	rtdb->print();
		
  // Transport nEvents
  // -----------------
  fRun->Run(nEvents);
  
  std::cout<<"Simulation complete"<<std::endl;	
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}

