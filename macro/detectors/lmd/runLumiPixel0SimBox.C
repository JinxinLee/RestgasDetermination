// Panda FullSim macro

//void runLumi0SimBox(const int nEvents=10, const double mom=15, TString storePath="tmpOutput", const int verboseLevel=0, const int particle=-211)
int runLumiPixel0SimBox(const int nEvents = 10, const int startEv = 0, TString storePath = "tmpOutput",
        const int verboseLevel = 0, const int particle = -2212, double mom = 15, const int trkNum = 1,
        const int seed = 0, const double dP = 0, TString geometryFile = "") {

	// ///PROOF lite
	// TProof::Open("");

	if (geometryFile == "") {
		geometryFile = "Luminosity-Detector.root";
	}

	gRandom->SetSeed(seed);
	//gRandom->SetSeed(0);
	TStopwatch timer;
	timer.Start();
	gDebug = 0;
	mom += dP;
	cout << "We start run for beam Mom = " << mom << endl;
	//output1
	TString simOutput = storePath + "/Lumi_MC_";
	simOutput += startEv;
	simOutput += ".root";
	TString parOutput = storePath + "/Lumi_Params_";
	parOutput += startEv;
	parOutput += ".root";

	// //Load basic libraries
	// gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	// gSystem->Load("libSds");
	// gSystem->Load("libLmd");
	FairRunSim *fRun = new FairRunSim();
	cout << "All libraries succsesfully loaded!" << endl;

	//set the MC version used
	fRun->SetName("TGeant4");
	//fRun->SetName("TGeant3");  //GEANE uses GEANT3!

	fRun->SetOutputFile(simOutput);

	//set material
	fRun->SetMaterials("media_pnd.geo");

	//create and add detectors
	FairModule *Cave = new PndCave("CAVE");
	//Cave->SetGeometryFileName("../macro/lmd/pndcaveVAC.geo");
	Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave);

	FairModule *Pipe = new PndPipe("PIPE");
	//  Pipe->SetGeometryFileName("../macro/lmd/geo/beampipe_201303.root");
	Pipe->SetGeometryFileName("beampipe_201309.root");
	fRun->AddModule(Pipe);

	FairModule *Magnet = new PndMagnet("MAGNET");
	Magnet->SetGeometryFileName("FullSolenoid_V842.root");
	fRun->AddModule(Magnet);

	FairModule *Dipole = new PndMagnet("MAGNET");
	Dipole->SetGeometryFileName("dipole.geo");
	fRun->AddModule(Dipole);

	PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
	Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
	//  Lum->SetGeometryFileName("../macro/lmd/geo/Test-Dipol-Design.root"); //sensors with trap shape
	//  Lum->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design-29052013.root"); // LMD including box etc
	Lum->SetGeometryFileName(geometryFile);
	//Lum->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design-SensorsOnly.root"); // LMD, sensors only
	Lum->SetVerboseLevel(verboseLevel);

	fRun->AddModule(Lum);

	//particle generator
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

	fRun->SetGenerator(primGen);

	// Box Generator
	FairBoxGenerator *fBox = new FairBoxGenerator(particle, trkNum);
	fBox->SetPRange(mom, mom);
	//  fBox->SetThetaRange(0.52,0.63); // 9 ... 11 mrad
	//  fBox->SetThetaRange(0.12,0.7); // 2... 12 mrad
	//  fBox->SetThetaRange(1.3,1.4); // mrad outside of the detector geometry by purpose
	fBox->SetThetaRange(0.13, 0.65); // 2... 11 mrad
	//fBox->SetThetaRange(0.12,0.65); // 2... 11 mrad
	//fBox->SetThetaRange(0.229183, 0.458366); //4 ... 8 mrad
	//fBox->SetThetaRange(0.229183,0.31512);//4..5.5 mrad
	//fBox->SetThetaRange(0.229,0.229);//4..mrad
	//fBox->SetThetaRange(0.458366, 0.458366); //!!! 8 mrad
	//fBox->SetThetaRange(0., 45.);//TEST
	//fBox->SetPhiRange(90,90.);
	fBox->SetPhiRange(0, 360.);
	//fBox->SetPhiRange(0.5,359.5); //FOR missed track check
	//fBox->SetPhiRange(0.,20.);//!!! TEST
	//fBox->SetThetaRange(0.344,0.344); //!!! TEST ~ 6 mrad
	//fBox->SetPhiRange(45,45);//TEST
	primGen->AddGenerator(fBox);

	//reading the new field map in the old format
	fRun->SetBeamMom(mom);

	PndMultiField *fField = new PndMultiField("AUTO");

	fRun->SetField(fField);

	if (nEvents < 101)
		fRun->SetStoreTraj(kTRUE); // toggle this for use with EVE
	else
		fRun->SetStoreTraj(kFALSE);

	// misalign Geometery
	bool misalignedGeometry = true;
	if (misalignedGeometry) {

		string misMatricesFilePath = "misalignMatrices-SensorsOnly.root";

		// check if file exists, if true, try to read it
		TFile *misalignmentMatrixRootfile = new TFile(misMatricesFilePath.c_str(), "READ");
		if (misalignmentMatrixRootfile->IsOpen()) {

			printf("File opened successfully\n");
			std::map < std::string, TGeoHMatrix > *matrices;

			gDirectory->GetObject("PndLmdMisalignMatrices", matrices);
			misalignmentMatrixRootfile->Close();

			cout << matrices->size() << " matrices successfully read from file.";

			Lum->SetMisalignmentMatrices(*matrices);
			cout << "matrix set!\n";

			if (true) {
				// checking for good measure
				for (auto &i : *matrices) {
					cout << "\n---\n" << i.first << "\n";
					i.second.Print();
				}
			}

		}

		// if not, fail violently
		else {
			cerr << "WARNING. I was instructed to use misaligned geometry,\n";
			cerr << "but no misaligned matrices could be found in " << misMatricesFilePath << "\n";
			return 1;
		}

	}

	// fRun->SetStoreTraj(kTRUE);
	fRun->Init();

	// Fill the Parameter containers for this run
	//-------------------------------------------
	FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
	output->open(parOutput.Data(), "RECREATE");
	rtdb->setOutput(output);
	PndMultiFieldPar* Par = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");
	if (fField) {
		Par->SetParameters(fField);
	}
	Par->setInputVersion(fRun->GetRunId(), 1);
	Par->setChanged();

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

