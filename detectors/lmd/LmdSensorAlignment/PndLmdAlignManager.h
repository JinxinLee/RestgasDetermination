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

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <PndLmdAlignStructs.h>
#include <PndLmdSensorAligner.h>

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
	int _i, _r, _w, _n;

	int _verboseLevel;

	std::stringstream _info;

	std::map<int, PndLmdSensorAligner> aligners;
	std::map<int, bool> alignersFull;

	bool _allFilesAdded, allAlignersDone;
	std::vector<std::string> fileNames;
	bool _singleAligner, _inCentimeters, _enableHelperMatrix, _zIsTimestamp, startAlignerWhenFull;
	std::string outFilename, _matrixOutDir, _binaryPairFileDirectory;
	bool _firstInitDone;
	bool _multithreaded;

	vector<int> overlapIDs;

	//FIXME: remove
	// we don't want that here anymore
	//PndLmdGeometryHelper *helper;

	void alignST();
	void alignMT();

	void alignOne(PndLmdSensorAligner &aligner);
	void resetMTLB(int n, int r, int w);
	void incrementMTLB();
	void checkIOpaths();

	//don't use, doesn't work with root like that
	static void readPairsFromChainMT(std::vector<std::string> files,
	    std::map<int, PndLmdSensorAligner> &aligners, PndLmdAlignManager &manager);

	//produces matrices 0 -> 1,2,3,4,5,6,7,8,9
	//Matrix combineMatrix(int id1, int id2);

	//produces matrices i -> ... -> i (should be almost identity matrix)
	//Matrix combineCyclicMatrix(int id);

	//generate the file name of a matrix or pair file, so changes must only be made once
	static std::string makeBinaryPairFileName(int overlapId = 0, bool incentimeters = true);
	static std::string makeMatrixFileName(int overlapId = 0, bool incentimeters = true);

	void verbosePrint(std::string input, int level=3);

public:

	//An empty AlignManager will use standard values. If you want to reuse a manager, call Init() again and set values.
	PndLmdAlignManager();
	virtual ~PndLmdAlignManager();

	// initializes Manager on construction or RESETS every value to default
	void init();

	// returns true if successful or false if aligner has enough pairs
	//bool addPair(PndLmdHitPair &pair);

	//adds pairs just like the other function but starts an aligner if it is full
	bool addPairAndStartAligner(PndLmdHitPair &pair);

	//add filename, so the aligner adds the pairs itself
	bool addFile(std::string filename);
	//void readFiles();		// deprecated
	void readFilesAndAlign();
	void waitForCompletion();

	//don't use, doesn't work with root like this
	//void readFilesMT();

	bool writePairsToBinaryFiles();
	bool readPairsFromBinaryFiles();
	bool checkForBinaryFiles();
	bool checkForLmdMatrixFiles();
	void clearPairs();

	//add all pair files that can be found in directory, up to a maximum of maxFiles
	//returns number of files found (including 0 for no files) or -1 if "pretend" option is set
	//set maxFiles=0 for all available files
	int addFilesFromDirectory(std::string directory, int maxFiles = 0);

	void validate();

	//perform last checks and run calculations on all aligners
	void alignAllSensors();

	static void loadBar(int current, int total, int resolution, int width, std::string message = "");

	void setMaxPairs(int maxPairs);

	//considers inactive area, guard rings, pixel size etc
	//TODO: remove, replace, deprecate, return
	//static Matrix transformMatrixFromPixelsToCm(const Matrix &input);

	static Matrix castTGeoHMatrixToMatrix(const TGeoHMatrix &matrix);

	//static Matrix getPixelToCentimeterTransformation();
	//Matrix getMatrixSensorToSensor(int fromSensor, int toSensor);

	// read and write matrix files to and from disk
	static Matrix readMatrix(std::string filename);
	static bool writeMatrix(Matrix &mat, std::string filename);

	void setSingleAligner(bool singleAligner) {
		_singleAligner = singleAligner;
	}

	void setOutFilename(const std::string& filename) {
		outFilename = filename;
		checkIOpaths();
	}

	static std::stringstream* readFile(std::string filename);
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

	void setMatrixOutDir(std::string matrixOutDir) {
		_matrixOutDir = matrixOutDir;
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

	//when supplied with a function object, this function executes in a new thread
	void workerThread(boost::shared_ptr<boost::asio::io_service> io_service);

	//write config file
	static bool writeConfigFile(boost::property_tree::ptree configTree, std::string filename,
	    bool replaceExisting = true);

	//read json config file
	static boost::property_tree::ptree readConfigFile(std::string filename);

	void setBinaryPairFileDirectory(const std::string& binaryPairFileDirectory) {
		_binaryPairFileDirectory = binaryPairFileDirectory;
	}

	void setStartAlignerWhenFull(bool value) {
		startAlignerWhenFull = value;
	}

	void verboseLevel(int verbose){
		_verboseLevel = verbose;
		for(auto &a:aligners){
			a.second.verboseLevel(_verboseLevel);
		}
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_ */
