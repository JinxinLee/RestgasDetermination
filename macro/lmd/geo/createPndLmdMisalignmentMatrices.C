// for now, we want deterministic pseudo random numbers.
// we can't use 0, because 0 is replaced by root by another
// pseudo random number which completely misses the point of
// a user defined seed value.
unsigned int seed = 128;

// use Mersenne Twister
TRandom3 *PRNG = new TRandom3(seed);

// creates random translation and rotation matrices with the supplied values as sigma values
TGeoHMatrix createRandomMatrix(double angleSigma, double shiftSigma) {

	double mean = 0;

	double angles[3];
	double shift[3];

	// can only rotate about z
	//angles[0] = PRNG->Gaus(mean, angleSigma);
	//angles[1] = PRNG->Gaus(mean, angleSigma);
	angles[0] = 0;
	angles[1] = 0;
	angles[2] = PRNG->Gaus(mean, angleSigma);

	// can't move in z
	shift[0] = PRNG->Gaus(mean, shiftSigma);
	shift[1] = PRNG->Gaus(mean, shiftSigma);
	//shift[2] = PRNG->Gaus(mean, shiftSigma);
	shift[2] = 0;

//	cout << "rand val: " << angles[0] << "\n";
//	cout << "rand val: " << angles[1] << "\n";
//	cout << "rand val: " << angles[2] << "\n";
//	cout << "rand val: " << shift[0] << "\n";
//	cout << "rand val: " << shift[1] << "\n";
//	cout << "rand val: " << shift[2] << "\n";

	TGeoHMatrix result;
	result.SetRotation(angles);
	result.SetTranslation(shift);

//	result.Print();

	return result;
}

int createPndLmdMisalignmentMatrices(bool debug = false) {

	string misMatricesFilePath = "misalignMatrices-SensorsOnly.root";

	cout << "creating dummy geometry...\n";

	TString geometryFile = "Luminosity-Detector.root";

	TString simOutput = "./dummy.root";
	FairRunSim *fRun = new FairRunSim();

	//set the MC version used
	fRun->SetName("TGeant4");

	fRun->SetOutputFile(simOutput);
	fRun->SetMaterials("media_pnd.geo");
	FairModule *Cave = new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave);
	FairModule *Pipe = new PndPipe("PIPE");
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
	Lum->SetGeometryFileName(geometryFile);
	fRun->AddModule(Lum);

	fRun->Init();

	cout << "PndLmd: creating misalignment matrices.\n";

	PndLmdGeometryHelper &helper = PndLmdGeometryHelper::getInstance();
	vector < string > paths = helper.getAllAlignPaths(true, false, false, false, false);

	cout << "got paths.\n";

	std::map < std::string, TGeoHMatrix > matrices;

	// for testing if ONE matrix works
	if (debug) {
		// Misalign geometry
		std::string examplePath = "/cave_1/lmd_root_0/half_0/plane_0/module_0/sensor_0/";

		TGeoHMatrix misalignTestMat;
		double translation[3] = { 0.5, 0.5, 0.0 };
		misalignTestMat.SetTranslation(translation);
		matrices[examplePath] = misalignTestMat;

	} else {

		for (auto &i : paths) {
			// TODO: cerate sigma parameters another way
			TGeoHMatrix tempMat = createRandomMatrix(1000e-6, 100e-4);
			matrices[i] = tempMat;
		}
	}

	if (true) {
		// checking for good measure
		for (auto &i : matrices) {
			cout << "\n---\n" << i.first << "\n";
			i.second.Print();
		}
	}

	TFile *misalignmentMatrixRootfile = new TFile(misMatricesFilePath.c_str(), "NEW");
	if (misalignmentMatrixRootfile->IsOpen()) {
		printf("File opened successfully\n");

		gDirectory->WriteObject(&matrices, "PndLmdMisalignMatrices");
		misalignmentMatrixRootfile->Write();
		misalignmentMatrixRootfile->Close();

		cout << "All matrices written to file!\n";
	} else {
		cerr << "WARNING! Could not write to " << misMatricesFilePath << "\n";
	}

	return 0;
}
