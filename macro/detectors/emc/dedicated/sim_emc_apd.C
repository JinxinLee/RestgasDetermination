// Macro created 20/09/2006 by S.Spataro
// It creates a geant simulation file for emc
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

	fRun->SetOutputFile("sim_emc_apd.root");

  // Set Material file Name
  //-----------------------

	fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------
	FairModule *Cave= new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave); 

	FairModule *Magnet= new PndMagnet("MAGNET");
	Magnet->SetGeometryFileName("FullSolenoid.root");
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
	Mvd->SetGeometryFileName("MVD_v1.0_woPassiveTraps.root");
	fRun->AddModule(Mvd);

	PndEmc *Emc = new PndEmc("EMC",kTRUE);
	Emc->SetGeometryVersion(15);
	fRun->AddModule(Emc);
  
	FairDetector *EmcApd = new PndEmcApd("APD",kTRUE);
	EmcApd->SetGeometryFileName("apd_module12.dat");
	fRun->AddModule(EmcApd);

	FairDetector *Tof = new PndTof("TOF",kTRUE);
	Tof->SetGeometryFileName("tofbarrel.geo");
	fRun->AddModule(Tof);

	PndMdt *Muo = new PndMdt("MDT",kTRUE);
	Muo->SetBarrel("torino");
	Muo->SetEndcap("torino");
	Muo->SetMuonFilter("torino");
	Muo->SetMdtMagnet(kTRUE);
	Muo->SetMdtMFIron(kTRUE);
	fRun->AddModule(Muo);

	PndDrc *Drc = new PndDrc("DIRC", kTRUE);
	Drc->SetRunCherenkov(kFALSE); // for fast sim Cherenkov -> kFALSE
	fRun->AddModule(Drc);

  // Create and Set Event Generator
  //-------------------------------
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);

  // Box Generator: 
	FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event

	boxGen->SetPRange(1.,1.); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
	boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
	boxGen->SetThetaRange(30., 130.); // Polar angle in lab system range [degree]
	boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
	primGen->AddGenerator(boxGen);  

  //fRun->SetStoreTraj(kTRUE); // to store particle trajectories 

	PndMultiField *fField= new PndMultiField();

	PndTransMap *map= new PndTransMap("TransMap", "R");
	PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
	PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
	fField->AddField(map);
	fField->AddField(map1);
	fField->AddField(map2);

	fRun->SetField(fField);

	fRun->Init();

  // Fill the Parameter containers for this run
  //-------------------------------------------
	FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
	Bool_t kParameterMerged=kTRUE;

	PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
	if (fField) {  Par->SetParameters(fField); }
	Par->setInputVersion(fRun->GetRunId(),1);
	Par->setChanged();

	FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
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

