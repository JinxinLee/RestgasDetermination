// for now, we want deterministic pseudo random numbers.
// we can't use 0, because 0 is replaced by root by another
// pseudo random number which completely misses the point of
// a user defined seed value.
unsigned int seed = 128;

#include <TRandom3.h>

// use Mersenne Twister
TRandom3 *PRNG = new TRandom3(seed);

// creates random translation and rotation matrices with the supplied values as sigma values
TGeoHMatrix createRandomMatrix(double angleSigma, double shiftSigma) {

	double mean = 0;

	double sigmaX, sigmaY, sigmaZ;
	double shift[3];

	// can only rotate about z
	//sigmaX = PRNG->Gaus(mean, angleSigma);
	//sigmaY = PRNG->Gaus(mean, angleSigma);
	sigmaX = 0;
	sigmaY = 0;
	sigmaZ = PRNG->Gaus(mean, angleSigma);

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
	result.RotateZ(sigmaZ);
	result.SetTranslation(shift);

//	result.Print();

	return result;
}

int createPndLmdMisalignmentMatrices() {

	string misMatFileName = "misalignMatrices-SensorsOnly-";
	string ext = ".root";

	cout << "creating dummy geometry...\n";

	// the Geometry we want to use
	TString geometryFile = "Luminosity-Detector.root";

	// we have to setup a dummy simulation so the gGeoManager indexes the Geometry
	TString simOutput = "./dummy.root";
	FairRunSim *fRun = new FairRunSim();
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

	// after init, the geometry can't be changed anymore, but that's okay.
	// we only want to create a matrix file that can be used in a later step.

	PndLmdGeometryHelper &helper = PndLmdGeometryHelper::getInstance();
	vector < string > paths = helper.getAllAlignPaths(true, false, false, false, false);

	cout << "got paths.\n";

	std::map<std::string, TGeoHMatrix> matrices;

	double misAlignParam;

	cout << "enter misalign parameter in um (example: 100 for 100um)\n";
	cout << "don't go higher than 500u. Please enter now:\nmisalign: ";

	cin >> misAlignParam;

	cout << "using shift: " << misAlignParam << "um and rot: " << misAlignParam/40 << "mrad \n";
	cout << "PndLmd: creating misalignment matrices.\n";

	for (auto &i : paths) {

		double shift = misAlignParam * 1e-4;	//convert to cm for root
		// use small angle approximation sin x = x
		double rot = TMath::RadToDeg() * (shift / 4);  // see report

		TGeoHMatrix tempMat = createRandomMatrix(rot, shift);
		matrices[i] = tempMat;
	}

	if (false) {
		// checking for good measure
		for (auto &i : matrices) {
			cout << "\n---\n" << i.first << "\n";
			i.second.Print();
		}
	}

	string paramStr = std::to_string(int(misAlignParam));

	TFile *misalignmentMatrixRootfile = new TFile((misMatFileName + paramStr + ext).c_str(), "NEW");
	if (misalignmentMatrixRootfile->IsOpen()) {
		printf("File opened successfully\n");

		gDirectory->WriteObject(&matrices, "PndLmdMisalignMatrices");
		misalignmentMatrixRootfile->Write();
		misalignmentMatrixRootfile->Close();

		cout << "All matrices written to file!\n";
	}
	else {
		cerr << "WARNING! Could not write to " << misMatFileName << "\n";
	}
	return 0;
}
