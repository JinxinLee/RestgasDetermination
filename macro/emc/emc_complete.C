// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
/**
 *  Modified 17.12.08 by M. Al-Turany
 *  The hit producing is added to the simulation session, so that the EmcHits are produced on the fly 
 *  during the simulation
 */

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
	gSystem->Load("libPndBase");
	gSystem->Load("libField");
	gSystem->Load("libPassive"); // add other detector's lib if you need them
	gSystem->Load("libEmc");
	gSystem->Load("libGen");
	
	FairRunSim *fRun = new FairRunSim();
	
	TString digiFile = "emc.par";
	
	TString parFile = "simparams.root";
	
	// Set the number of events
	Int_t nEvents = 10; 
	
	// set the MC version used
	// ------------------------
	
	fRun->SetName("TGeant3");
	//fRun->SetName("TGeant4");
	
	fRun->SetOutputFile("emc_complete.root");
	
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
	
	//FairDetector *Emc = new PndEmc("EMC",kTRUE);
	PndEmc *Emc = new PndEmc("EMC",kTRUE);
	//Emc->SetGeometryFileName("emc_module12345.dat"); // if you want to use old geometry for FwEndCap
	Emc->SetGeometryFileNameDouble("emc_module1245.dat","emc_module3new.root"); // if you want to use new geometry for FwEndCap
	Emc->SetStorageOfData(kFALSE);
	fRun->AddModule(Emc);
	
	// Create and Set Event Generator
	//-------------------------------
	
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);
	
	// Box Generator: 
	FairBoxGenerator* boxGen = new FairBoxGenerator(22, 5); // 13 = muon; 1 = multipl. // 211 = pi+
	// first number: PDG particle code: 2nd number: particle multiplicity per event
	
	boxGen->SetPRange(1.,1.); // GeV/c
	// boxGen->SetPtRange(1.,1.); // GeV/c
	boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
	boxGen->SetThetaRange(0., 180.); // Polar angle in lab system range [degree]
	boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
	primGen->AddGenerator(boxGen);  
	
	// fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
	
	PndMultiField *fField= new PndMultiField();
	
	PndTransMap  *tmap= new PndTransMap("Trans1", "R");
	PndDipoleMap *dmap1= new PndDipoleMap("Dipole1", "R");
	PndDipoleMap *dmap2= new PndDipoleMap("Dipole2", "R");
    
	PndSolenoidMap *smap1= new PndSolenoidMap("Solenoid1", "R");
	PndSolenoidMap *smap2= new PndSolenoidMap("Solenoid2", "R");
	PndSolenoidMap *smap3= new PndSolenoidMap("Solenoid3", "R");
	PndSolenoidMap *smap4= new PndSolenoidMap("Solenoid4", "R");
	
	fField->AddField(tmap);
	fField->AddField(dmap1);
	fField->AddField(dmap2);
	
	fField->AddField(smap1);
	fField->AddField(smap2);
	fField->AddField(smap3);
	fField->AddField(smap4);
	
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
	emcWaveformToDigi->SetStorageOfData(kFALSE);
	fRun->AddTask(emcHitsToWaveform);  // full digitization
	fRun->AddTask(emcWaveformToDigi);  // full digitization
 
	PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(0);
	emcMakeCluster->SetStorageOfData(kFALSE);
	fRun->AddTask(emcMakeCluster);

	PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
	fRun->AddTask(emcHdrFiller); // ECM header

	PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
	emcMakeBump->SetStorageOfData(kFALSE);
	fRun->AddTask(emcMakeBump);

	PndEmcMakeRecoHit* emcMakeRecoHit= new PndEmcMakeRecoHit();
	fRun->AddTask(emcMakeRecoHit);
	
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
    output->open("simparams.root");
    rtdb->setOutput(output);
	
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
	fRun->Run(nEvents);
	
	timer.Stop();
	
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
	
}  

