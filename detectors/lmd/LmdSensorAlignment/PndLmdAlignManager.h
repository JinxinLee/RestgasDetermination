/*
 * PndLmdAlignManager.h
 *
 *  Created on: May 26, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 *
 *      This is a manager for all SensorAligner objects. It gathers all PndLmdHitPairs indiscriminately,
 *      and sorts them to 40 (depends on sensor geometry) SensorAligner Objects. It also stores the
 *      resultant matrices and writes them to disk (appropriate format is yet to come, for now just ascii).
 */

#ifndef LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_
#define LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_

#include "PndLmdHitPair.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <PndLmdAlignStructs.h>
#include <PndLmdGeometryHelper.h>
#include <PndLmdSensorAligner.h>
#include "PndLmdThreadPool.h"

#include <TGeoMatrix.h>

#include <matrix.h>
#include <string>
#include <sstream>

class PndLmdAlignManager {

	typedef std::map<int, PndLmdSensorAligner>::iterator mapIt;

private:

	// PndLmdSensorAligner needs private functions from here
	friend class PndLmdSensorAligner;
	friend class PndLmdAlignQA;

	//loadBarMultiThreaded
	std::mutex MTLBmutex;

	int _verboseLevel;
	unsigned int maxThreads;

	std::stringstream _info;

	std::map<int, PndLmdSensorAligner> aligners;
	std::map<int, bool> alignersFull;

	bool _allFilesAdded;
	std::vector<std::string> fileNames;
	bool _singleAligner, _inCentimeters, enableHelperMatrix, _zIsTimestamp;
	std::string outFilename, matrixOutDir, binaryPairFileDirectory;
	bool _firstInitDone;
	bool _multithreaded;

	vector<int> overlapIDs;

	bool allAlignersFull();

	//adds pairs just like the other function but starts an aligner if it is full
	bool addPairAndStartAligner(PndLmdHitPair &pair, PndLmdThreadPool &threadPool);

	void runSensorAligner(PndLmdSensorAligner &aligner);
	void checkIOpaths();

	//let all threads finish their work
	void waitForCompletion(PndLmdThreadPool &threadPool);

	//generate the file name of a matrix or pair file, so changes must only be made once
	static std::string makeBinaryPairFileName(int overlapId = 0, bool incentimeters = true);
	static std::string makeMatrixFileName(int overlapId = 0, bool incentimeters = true);

	void verbosePrint(std::string input, int level = 3);

public:

	//An empty AlignManager will use standard values. If you want to reuse a manager, call Init() again and set values.
	PndLmdAlignManager();
	virtual ~PndLmdAlignManager();

	// initializes Manager on construction or resets every value to default
	void init();

	//add filename, so the aligner adds the pairs itself
	bool addFile(std::string filename);
	void readFilesAndAlign();

	bool writePairsToBinaryFiles();
	bool readPairsFromBinaryFilesAndAlign();
	bool checkForBinaryFiles();
	bool checkForLmdMatrixFiles();
	void clearPairs();

	//add all pair files that can be found in directory, up to a maximum of maxFiles
	//returns number of files found (including 0 for no files) or -1 if "pretend" option is set
	//set maxFiles=0 for all available files
	int addFilesFromDirectory(std::string directory, int maxFiles = 0);

	void validate();

	//perform last checks and run calculations on all aligners
	//void alignAllSensors();

	void loadBar(int current, int total, int resolution, int width, std::string message = "");

	void setMaxPairs(int maxPairs);

	static Matrix castTGeoHMatrixToMatrix(const TGeoHMatrix &matrix);

	// read and write matrix files to and from disk
	static Matrix readMatrix(std::string filename);
	static TGeoHMatrix readTGeoHMatrix(std::string filename);

	static bool writeMatrix(Matrix &mat, std::string filename);

	void setSingleAligner(bool singleAligner) {
		_singleAligner = singleAligner;
	}

	void setOutFilename(const std::string& filename) {
		outFilename = filename;
		checkIOpaths();
	}

	//static std::stringstream readFile(std::string filename);
	static std::vector<std::vector<double> > readFromCSVFile(std::string filename);

	/*
	 * recursively searches files by extension in curr_directory and adds to list
	 * finds all extensions by default, but you can specify which extension
	 * (or part of filename) you want
	 */
	static int searchFiles(std::string curr_directory, std::vector<std::string> &list,
	    std::string extension = "", bool includeSubDirs = true);

	//clear console
	static void clearScreen();

	//searches directories in curr_directory, adds to list
	static int searchDirectories(std::string curr_directory, std::vector<std::string> &list,
	    bool includeSubDirs = true);
	static bool mkdir(std::string path);
	static bool exists(std::string file);
	static std::vector<std::string> findRegex(std::string source, std::string regex);

	void setMatrixOutDir(std::string directory) {
		matrixOutDir = directory;
	}

	/*
	 * set this BEFORE reading pair files. if inCentimeters==true, then hits will be decoded by PndLmdDim to centimeters
	 * and all calculations will be done in cm. Misalginment matrices are close to identity matrix, which could introduce
	 * numerical errors.
	 * If inCentimeters==false, then the ICP will work directly on row/col values and matrix will be in pixels. AlignManager
	 * converts matrix to cm, but the resultant matrix is the product of misalignment*ideal!
	 */
	void setInCentimeters(bool inCentimeters);
	void setZasTimestamp(bool timestamp);

	//write config file
	static bool writeConfigFile(boost::property_tree::ptree configTree, std::string filename,
	    bool replaceExisting = true);

	//read json config file
	static boost::property_tree::ptree readConfigFile(std::string filename);

	void setBinaryPairFileDirectory(const std::string& directory) {
		binaryPairFileDirectory = directory;
	}

	void verboseLevel(int verbose) {
		_verboseLevel = verbose;
		for (auto &a : aligners) {
			a.second.verboseLevel(_verboseLevel);
		}
	}

	void setMaxThreads(int NmaxThreads = 0) {
		if (NmaxThreads < 0 || NmaxThreads > 256) {
			//cerr << "Invalid number of threasds!\n";
		}
		else {
			maxThreads = NmaxThreads;
		}
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_ */
