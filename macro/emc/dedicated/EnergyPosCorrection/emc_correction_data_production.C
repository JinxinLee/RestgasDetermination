// Macro produces data for energy-position correction of EMC
// Data should be produced once for each particle type (gamma, electron)
// transport model (TGeant3, TGeant4)
// Full Panda geometry or EMC only useFullPandaGeometry=true/false
// emcModule = TS/shashlyk
void emc_correction_data_production(Int_t nEvents = 10, TString part="gamma", TString TransportModel = "TGeant3", Bool_t useFullPandaGeometry=true, TString emcModule="TS",TString energyRange="all", UInt_t seed=0)
{
	gRandom->SetSeed(seed);
	Double_t phi_min = 0; Double_t phi_max = 360;
	
	if (emcModule=="TS")
	{
		Double_t theta_min = 5; Double_t theta_max = 172;
	} else if (emcModule=="shashlyk")
	{
		Double_t theta_min = 0; Double_t theta_max = 10;
	} else 
	{
		std::cout<<"Incorrect emcModule parameter: "<<emcModule<<std::endl;
		abort();
	}
	
	// energyRange= "low", "high", "all"
	if (energyRange=="low")
	{
		Double_t momentum_min = 0.0; Double_t momentum_max = 1.5;
	} else if (energyRange=="high")
	{
		Double_t momentum_min = 1.0; Double_t momentum_max = 10.0;
	} else if (energyRange=="all")
	{
		Double_t momentum_min = 0.0; Double_t momentum_max = 10.0;
	} else
	{
		std::cout<<"Incorrect parameter energyRange: "<<energyRange<<std::endl;
		abort();
	}
	
	TString pandaGeometry;
	if (useFullPandaGeometry==0)
		pandaGeometry="emc";
	else
		pandaGeometry="full";
	
	TString s1; s1+=seed;
	
	TString OutputSimFile = "emc_complete";
	OutputSimFile = OutputSimFile+"_"+part+"_"+TransportModel+
	"_"+pandaGeometry+"_"+emcModule+"_"+s1+".root";
	TString OutputDatabaseFile = "simparams";
	OutputDatabaseFile = OutputDatabaseFile+"_"+part+"_"+TransportModel+
	"_"+pandaGeometry+"_"+emcModule+"_"+s1+".root";

	TStopwatch timer;
	timer.Start();
	gDebug=0;

	FairLogger::GetLogger()->SetLogToFile(kFALSE);		
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
  Emc->SetGeometryVersion(1);
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
	fRun->AddModule(Pipe);

	FairDetector *Stt= new PndStt("STT", kTRUE);
	Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
	fRun->AddModule(Stt);

	FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
	Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
	fRun->AddModule(Mvd);

    PndMdt *Muo = new PndMdt("MDT",kTRUE);
    Muo->SetMdtMagnet(kTRUE);
    //  Muo->SetMdtMFIron(kFALSE);
    Muo->SetMdtCoil(kTRUE);
    Muo->SetBarrel("muon_TS_barrel_strip_v1_noGeo.root");
    Muo->SetEndcap("muon_TS_endcap_strip_v1_noGeo.root");
    Muo->SetForward("muon_Forward_strip_v1_noGeo.root");
    Muo->SetMuonFilter("muon_MuonFilter_strip_v1_noGeo.root");
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
	Drc->SetGeometryFileName("dirc_l0_p0.root");
	Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
	fRun->AddModule(Drc);
	
	FairDetector *Fts= new PndFts("FTS", kTRUE);
	Fts->SetGeometryFileName("fts.geo");
	fRun->AddModule(Fts);
  }
	
  // Create and Set Event Generator
  //-------------------------------
	
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
  
  TDatabasePDG *pdg = new TDatabasePDG();
  Int_t part_pid = pdg->GetParticle(part)->PdgCode();
  
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
  emcHitProd->SetStorageOfData(kFALSE);//FALSE);
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
		
  fRun->Run(nEvents);
  timer.Stop();
	
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
	
}  

