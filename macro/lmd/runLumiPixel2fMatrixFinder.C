/*
 * Finds pixel hit pairs and stores them to pair File
 *
 *	Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 *
 *
 */

//#include <string>
//#include <PndLmdAlignManager.h>
//
//int main() {
//
//	std::string pairFilePath = "/home/arbeit/RedPro3TB/simulationData/newGeometry";
//	std::string binaryPairFilePath = pairFilePath + "/binaryPairs/";
//	std::string LMDmatrixDir = pairFilePath + "/LMDmatrices/";
//
//	PndLmdAlignManager manager;
//	manager.setInCentimeters(true);
//	manager.setBinaryPairFileDirectory(binaryPairFilePath);
//	manager.setMaxPairs(300e3);
//	manager.verboseLevel(3);
//
//	cout << "reading binary pair files.\n";
//	manager.setMatrixOutDir(LMDmatrixDir);
//	manager.readPairsFromBinaryFiles();
//	manager.alignAllSensors();
//
//	return 0;
//}

int runLumiPixel2fMatrixFinder(TString pairFilePath = "test/boxtest-aligned-1.5/",
    TString binaryPairFilePath = "", TString LMDmatrixDir = "", bool incentimeters = true,
    const int verboseLevel = 2) {
	// -----   Timer   --------------------------------------------------------

	//do we really need those?
	gSystem->Load("libLmd");
	gSystem->Load("libLmdSensorAligner");

	// ---------------------- init parameters

	string pandaDir = getenv("VMCWORKDIR");

	// don't forget trailing slashes!!
	if (LMDmatrixDir == "") {
		LMDmatrixDir = pairFilePath + "/LMDmatrices/";
	}
	if (binaryPairFilePath == "") {
		binaryPairFilePath = pairFilePath + "/binaryPairs/";
	}

	int readNoOfFiles = 0;			//how many files should be processed? 0 for all

	// ---------------------- init Matrix Finder

	cout << "searching available files...\n";

	PndLmdAlignManager manager;
	manager.setInCentimeters(incentimeters);
	manager.setBinaryPairFileDirectory(binaryPairFilePath.Data());
	manager.setMaxPairs(300e3);
	manager.verboseLevel(verboseLevel);

	// ---------------------- check for binary files and sort/write, if necessary
	bool binaryPairsPresent = manager.checkForBinaryFiles();

	if (!binaryPairsPresent) {
		manager.addFilesFromDirectory(pairFilePath.Data(), readNoOfFiles);
		manager.setMatrixOutDir(LMDmatrixDir.Data());

		manager.readFilesAndAlign();
		manager.waitForCompletion();
		return 0;
	}

	//check for LMD Matrix Files
	bool LMDMatrixFilesPresent = manager.checkForLmdMatrixFiles();
	if (!LMDMatrixFilesPresent) {
		cout << "reading binary pair files.\n";
		manager.setMatrixOutDir(LMDmatrixDir.Data());
		manager.readPairsFromBinaryFiles();
		manager.alignAllSensors();
	}

	//manager.computeCombinedMatrices();

	// ---------------------- compute absolute correction matrices from overlap matrices and store to PndLmdDim format, compare to pndLmdDim

	//TODO: well, this ^

	// steps:
	/*
	 * gather all modules
	 * gather all overlapping areas for each module
	 * compute all combined matrices for each module (this can be done multi-threadedly)
	 * store them in a good way (how?)
	 *
	 */

	// -----   Finish   -------------------------------------------------------
	return 0;
}

