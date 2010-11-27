// Macro produces data for energy-position correction of EMC
// Data shiuld be produced once for each particle type (gamma, electron) and transport model (TGeant3, TGeant4)
void emc_correction_data_production(Int_t nEvents = 10, TString part="gamma", Bool_t useFullPandaGeometry=false, Double_t momentum_min = 0.0, Double_t momentum_max = 10.0, Double_t theta_min = 0, Double_t theta_max = 6, Double_t phi_min = 0, Double_t phi_max = 360, TString OutputSimFile = "emc_complete_shashlyk.root", TString OutputDatabaseFile = "simparams.root", TString TransportModel = "TGeant3", UInt_t seed=0)
{
  //emc_complete(Int_t nEvents = 100, Float_t mom = 1.){
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

		
  FairRunSim *fRun = new FairRunSim();
	
  TString digiFile = "emc.par";
	
  // set the MC version used
  // ------------------------
	
   fRun->SetName(TransportModel);
	
  
  fRun->SetOutputFile(OutputSimFile);
	

 /**Set the digitization parameters */
	
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;

  /**Get the run time data base for this session and set the needed input*/
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);        
  Bool_t kParameterMerged=kTRUE;

  /**Parameters created for this simulation goes to the out put*/
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(OutputDatabaseFile);
  rtdb->setOutput(output);



  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");
	
  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 
	
  PndEmc *Emc = new PndEmc("EMC",kTRUE);
  Emc->SetGeometryVersion(17);
  Emc->SetStorageOfData(kFALSE);
  fRun->AddModule(Emc);
  
  if (useFullPandaGeometry)
  {
	  FairModule *Magnet= new PndMagnet("MAGNET");
	  Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
	  fRun->AddModule(Magnet);

	  FairModule *Dipole= new PndMagnet("MAGNET");
	  Dipole->SetGeometryFileName("dipole.geo");
	  fRun->AddModule(Dipole);

	  FairModule *Pipe= new PndPipe("PIPE");
     //fRun->AddModule(Pipe);

	  PndTpcDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
	  Tpc->SetGeometryFileName("tpc.geo");
	  if(TransportModel=="TGeant3")  Tpc->SetAliMC();
	  fRun->AddModule(Tpc);

	  FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
	  Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
	  fRun->AddModule(Mvd);

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
	  Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
	  fRun->AddModule(Drc);
  }
	
  // Create and Set Event Generator
  //-------------------------------
	
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  cout <<" part = "<< part<<endl;
  TDatabasePDG *pdg = new TDatabasePDG();
  Int_t part_pid = pdg->GetParticle(part)->PdgCode();
  cout <<"pid == "<< part_pid<<endl;
  
  // Box Generator: 
  FairBoxGenerator* boxGen = new FairBoxGenerator(part_pid, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
	
  boxGen->SetPRange(momentum_min,momentum_max); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(phi_min, phi_max); // Azimuth angle range [degree]
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
  emcHitProd->SetStorageOfData(kTRUE);//FALSE);
  fRun->AddTask(emcHitProd);

  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(0);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(0);
  emcHitsToWaveform->SetStorageOfData(kFALSE);
  emcWaveformToDigi->SetStorageOfData(kTRUE);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization
 
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(0);
  emcMakeCluster->SetStorageOfData(kTRUE);
  fRun->AddTask(emcMakeCluster);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  emcMakeBump->SetStorageOfData(kTRUE);
  fRun->AddTask(emcMakeBump);
   
  cout<< "emc_complete.C: transportModel--> "<<TransportModel<<endl;

   /**Initialize the session*/
  fRun->Init();
  /**After initialization now we can save the field parameters */
  PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
  if (fField) {  Par->SetParameters(fField); }
  Par->setInputVersion(fRun->GetRunId(),1);
  Par->setChanged();
	
  /**All parameters are initialized and ready to be saved*/
  rtdb->saveOutput();
  //	rtdb->print();
		
  // Transport nEvents
  // -----------------
  cout <<" nEvents == "<<nEvents<<endl;
  fRun->Run(nEvents);
  cout <<" ***done == "<<endl;
  timer.Stop();
	
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
	
}  

