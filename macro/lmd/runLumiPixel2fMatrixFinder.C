/*
 * Finds pixel hit pairs and stores them to pair File
 */
using namespace std;

// g++ $(root-config --cflags --glibs) -lGeom -I${PANDAROOT}/geobase -L${PANDAROOT}/../buildPanda/lib -lGeoBase runLumiPixel2fMatrixFinder.C -o /tmp/find_matrices -I${PANDAROOT}/lmd/LmdMC -I${PANDAROOT}/lmd -I${PANDAROOT}/parbase -I${PANDAROOT}/dbase/dbInterface -I${PANDAROOT}/dbase/dbValidation -I${PANDAROOT}/dbase/dbInput -I${PANDAROOT}/lmd/LmdSensorAlignment/icp -I${PANDAROOT}/lmd/LmdSensorAlignment -I${PANDAROOT}/base/event

#include <string>

void runLumiPixel2fMatrixFinder(TString pairFilePath="test/boxtest-aligned-1.5/", TString binaryPairFilePath="test/boxtest-aligned-1.5/binaryFiles", bool incentimeters =true, const int verboseLevel=0)
{
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	// ---------------------- init parameters

	string pandaDir = getenv("VMCWORKDIR");

	bool simplestorage=true;
	string matrixDir=pandaDir + "/geometry/LMDmatrices";
	string pairFilesDir=pairFilePath.Data();
	string binaryFilesDir=binaryPairFilePath.Data();
	int readNoOfFiles=0;			//assuming each pair file is about 64 MB in size FIXME: maybe don't restrict at all


	// ---------------------- init Matrix Finder

	cout << "searching available files...\n";

	PndLmdAlignManager manager;
	manager.setSimpleStorage(simplestorage);
	manager.setInCentimeters(incentimeters);
	manager.setBinaryPairFileDirectory(binaryFilesDir);
	manager.setMaxPairs(300e3);

	// DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE ---
	manager.setMatrixOutDir(matrixDir);
	manager.compareCombinedMatrices();
	return;
	// DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE ---

	// ---------------------- check for binary files and sort/write, if necessary

	bool binaryPairsPresent = manager.checkForBinaryFiles();
	if(!binaryPairsPresent){
		manager.addFilesFromDirectory(pairFilesDir, readNoOfFiles);
		manager.readFiles();
		manager.writePairsToBinaryFiles();
		manager.clearPairs();				//free memory
	}

	// ---------------------- find overlap matrices and store to (overlap) matrix files
	binaryPairsPresent = manager.checkForBinaryFiles();
	if(binaryPairsPresent){
		manager.readPairsFromBinaryFiles();
		manager.setMatrixOutDir(matrixDir);
		manager.alignAllSensors();
	}
	else{
		cout << "something went wrong. still not all binary files found.\n";
	}

	manager.computeCombinedMatrices();

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
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
	cout << endl;
}
