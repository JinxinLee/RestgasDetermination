/*
 * Finds pixel hit pairs and stores them to pair File
 *
 *	Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 *
 *
 */
using namespace std;

#include <string>

int runLumiPixel2fMatrixFinder(TString pairFilePath="test/boxtest-aligned-1.5/", TString binaryPairFilePath="test/boxtest-aligned-1.5/binaryFiles", TString LMDmatrixDir="", bool incentimeters=true, const int verboseLevel=0)
{
	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	// ---------------------- init parameters

	string pandaDir = getenv("VMCWORKDIR");

	bool simplestorage=true;
	string matrixDir;

	if(LMDmatrixDir==""){
		matrixDir=pandaDir + "/geometry/LMDmatrices";
	}
	else{
		matrixDir=LMDmatrixDir.Data();
	}
	string pairFilesDir=pairFilePath.Data();
	string binaryFilesDir=binaryPairFilePath.Data();
	int readNoOfFiles=0;			//how many files should be processed? 0 for all

	// ---------------------- init Matrix Finder

	cout << "searching available files...\n";

	PndLmdAlignManager manager;
	manager.setSimpleStorage(simplestorage);
	manager.setInCentimeters(incentimeters);
	manager.setBinaryPairFileDirectory(binaryFilesDir);
	manager.setMaxPairs(300e3);

	// DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE ---
	//manager.setMatrixOutDir(matrixDir);
	//manager.compareCombinedMatrices();
	//return;
	// DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE --- DELETE THIS LINE ---

	// ---------------------- check for binary files and sort/write, if necessary

	bool binaryPairsPresent = manager.checkForBinaryFiles();

	if(!binaryPairsPresent){
		manager.addFilesFromDirectory(pairFilesDir, readNoOfFiles);
		manager.setMatrixOutDir(matrixDir);

		//manager.setZasTimestamp(true);	//TODO: remove after testing, or leave in depending on test result!

		manager.readFilesAndAlign();
		manager.waitForCompletion();
		return 0;
	}

	//check for LMD Matrix Files
	bool LMDMatrixFilesPresent = manager.checkForLmdMatrixFiles();
	if(!LMDMatrixFilesPresent){
		cout << "reading binary pair files.\n";
		manager.setMatrixOutDir(matrixDir);
		manager.readPairsFromBinaryFiles();
		manager.alignAllSensors();
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
	return 0;
}
