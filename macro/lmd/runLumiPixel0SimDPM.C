// Lmd DPM Sim macro
int runLumiPixel0SimDPM(const int nEvents = 10, const int startEvent = 0,
		const double mom = 15, TString input, TString storePath = "tmpOutputDPM",
		const double beam_X0 = 0.0, const double beam_Y0 = 0.0,
		const double target_Z0 = 0.0, const double beam_width_sigma_X = 0.0,
		const double beam_width_sigma_Y = 0.0,
		const double target_width_Z = 0.0, // beam offset and smearing parameters
		const double beam_grad_X = 0.0, const double beam_grad_Y = 0.0,
		const double beam_grad_sigma_X = 0.0, const double beam_grad_sigma_Y = 0.0, // beam gradiant parameters
		const TString lmd_geometry_filename = "Luminosity-Detector.root",
		const int verboseLevel = 0) {
	// gRandom->SetSeed(seed);
	Int_t mode = 1;
	TStopwatch timer;
	timer.Start();
	gDebug = 0;

	//output1
	TString simOutput = storePath + "/Lumi_MC_";
	simOutput += startEvent;
	simOutput += ".root";
	TString parOutput = storePath + "/Lumi_Params_";
	parOutput += startEvent;
	parOutput += ".root";
	//Load basic libraries
	gSystem->Load("libSds");
	gSystem->Load("libLmd");
	FairRunSim *fRun = new FairRunSim();
	cout << "All libraries succsesfully loaded!" << endl;

	//set the MC version used
	fRun->SetName("TGeant4");

	fRun->SetOutputFile(simOutput);

	//set material
	fRun->SetMaterials("media_pnd.geo");

	//  //create and add detectors
// //-------------------------  CAVE      -----------------

	FairModule *Cave = new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave);
	//-------------------------  Magnet   -----------------
	FairModule *Magnet = new PndMagnet("MAGNET");
	Magnet->SetGeometryFileName("FullSuperconductingSolenoid_v831.root");
	fRun->AddModule(Magnet);
	FairModule *Dipole = new PndMagnet("MAGNET");
	Dipole->SetGeometryFileName("dipole.geo");
	fRun->AddModule(Dipole);
	//-------------------------  Pipe     -----------------
	FairModule *Pipe = new PndPipe("PIPE");
	Pipe->SetGeometryFileName("beampipe_201309.root");
	fRun->AddModule(Pipe);
//   //-------------------------  STT       -----------------
//   FairDetector *Stt= new PndStt("STT", kTRUE);
//   Stt->SetGeometryFileName("straws_skewed_blocks_35cm_pipe.geo");
//   fRun->AddModule(Stt);
//   //-------------------------  MVD       -----------------
//   FairDetector *Mvd = new PndMvdDetector("MVD", kTRUE);
//   Mvd->SetGeometryFileName("Mvd-2.1_FullVersion.root");
//   fRun->AddModule(Mvd);
//   //-------------------------  GEM       -----------------
//   FairDetector *Gem = new PndGemDetector("GEM", kTRUE);
//   Gem->SetGeometryFileName("gem_3Stations.root");
//   fRun->AddModule(Gem);
//   //-------------------------  EMC       -----------------
//   PndEmc *Emc = new PndEmc("EMC",kTRUE);
//   Emc->SetGeometryVersion(1);
//   Emc->SetStorageOfData(kFALSE);
//   fRun->AddModule(Emc);
//   //-------------------------  DRC       -----------------
//   PndDrc *Drc = new PndDrc("DIRC", kTRUE);
//   Drc->SetGeometryFileName("dirc_l0_p0_updated.root"); 
//   Drc->SetRunCherenkov(kFALSE);
//   fRun->AddModule(Drc); 
//   //-------------------------  DISC      -----------------
//   PndDsk* Dsk = new PndDsk("DSK", kTRUE);
//   Dsk->SetStoreCerenkovs(kFALSE);
//   Dsk->SetStoreTrackPoints(kFALSE);
//   fRun->AddModule(Dsk);
//   //-------------------------  MDT       -----------------
//   PndMdt *Muo = new PndMdt("MDT",kTRUE);
//   Muo->SetBarrel("fast");
//   Muo->SetEndcap("fast");
//   Muo->SetMuonFilter("fast");
//   Muo->SetForward("fast");
//   Muo->SetMdtMagnet(kTRUE);
//   Muo->SetMdtMFIron(kTRUE);
//   fRun->AddModule(Muo);
//   //-------------------------  FTS       -----------------
//   FairDetector *Fts= new PndFts("FTS", kTRUE);
//   Fts->SetGeometryFileName("fts.geo");
//   fRun->AddModule(Fts); 
//   //-------------------------  FTOF      -----------------
//   FairDetector *FTof = new PndFtof("FTOF",kTRUE);
//   FTof->SetGeometryFileName("ftofwall.root");
//   fRun->AddModule(FTof);
//   //-------------------------  RICH       ----------------
//   FairDetector *Rich= new PndRich("RICH",kFALSE);
//   Rich->SetGeometryFileName("rich_v2.geo");
//   fRun->AddModule(Rich);

	PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
	Lum->SetExclusiveSensorType("LumActive"); //ignore MVD
	Lum->SetGeometryFileName(lmd_geometry_filename); // new sensors
	Lum->SetVerboseLevel(verboseLevel);
	fRun->AddModule(Lum);

	//particle generator
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	if (beam_X0 != 0.0 || beam_Y0 != 0.0 || beam_width_sigma_X > 0.0 || beam_width_sigma_Y > 0.0) {
		primGen->SmearGausVertexXY(true);
		primGen->SetBeam(beam_X0, beam_Y0, beam_width_sigma_X, beam_width_sigma_Y);
	}
	if(target_Z0 != 0.0 || target_width_Z > 0.0) {
		primGen->SmearGausVertexZ(true);
		primGen->SetTarget(target_Z0, target_width_Z);
	}
	if (beam_grad_X != 0.0 || beam_grad_Y != 0.0 || beam_grad_sigma_X > 0.0 || beam_grad_sigma_Y > 0.0) {
		primGen->SetBeamAngle(beam_grad_X, beam_grad_Y, beam_grad_sigma_X,
				beam_grad_sigma_Y);
	}

	fRun->SetGenerator(primGen);

// DPM Generator
	PndDpmGenerator* dpmGen = new PndDpmGenerator(input);
	primGen->AddGenerator(dpmGen);
// PndDpmDirect *dpmGen = new PndDpmDirect(mom, mode, gRandom->GetSeed(), 0.1);
// primGen->AddGenerator(dpmGen);

//reading the new field map in the old format
	fRun->SetBeamMom(mom);
 PndMultiField *fField= new PndMultiField("AUTO");
// fRun->SetField(fField);
	/*PndMultiField *fField = new PndMultiField();
	PndTransMap *map_t = new PndTransMap("TransMap", "R");
	PndDipoleMap *map_d1 = new PndDipoleMap("DipoleMap1", "R");
	PndDipoleMap *map_d2 = new PndDipoleMap("DipoleMap2", "R");
	PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
	PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
	PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
	PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

	fField->AddField(map_t);
	fField->AddField(map_d1);
	fField->AddField(map_d2);
	fField->AddField(map_s1);
	fField->AddField(map_s2);
	fField->AddField(map_s3);
	fField->AddField(map_s4);*/

	fRun->SetField(fField);

	if (nEvents < 450)
		fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
	else
		fRun->SetStoreTraj(kFALSE);

//FairLogger
// get handle
	FairLogger *logger = FairLogger::GetLogger();
//
// log to screen and to file
	logger->SetLogToScreen(kTRUE);
	logger->SetLogToFile(kFALSE);

	logger->SetLogVerbosityLevel("LOW");

// Set different levels of verbosity. In the example everything >=INFO goes to the
// file and everything >= ERROR is printed on the screen
// LogLevels are (FATAL, ERROR, WARNING, INFO, DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG4)
	logger->SetLogScreenLevel("ERROR"); //Only FATAL and ERROR to screen

	fRun->Init();
	((TGeant4*)gMC)->ProcessGeantCommand("/mcVerbose/eventAction 0");

// // Fill the Parameter containers for this run
// //-------------------------------------------
	FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
	output->open(parOutput.Data(), "RECREATE");
	rtdb->setOutput(output);

// PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
// if (fField) {  Par->SetParameters(fField); }
// Par->setInputVersion(fRun->GetRunId(),1);
// Par->setChanged();

// Transport nEvents
// -----------------

	fRun->Run(nEvents);

	rtdb->saveOutput();
	rtdb->print();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
  return 0;
}
