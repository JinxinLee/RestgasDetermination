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

//apparently, CINT has a problem with some boost classes
#ifndef __CINT__
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#endif

#include <PndLmdAlignStructs.h>
#include <PndLmdDim.h>
#include <PndLmdSensorAligner.h>

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
	int _i,_r,_w,_n;

	stringstream _info;

	std::map<int, PndLmdSensorAligner> aligners;
	std::map<int, bool> alignersFull;
	Matrix helperMatrix;
	PndLmdDim *dimension;
	bool _allFilesAdded, _pretend, allAlignersDone, debug;
	std::vector<std::string> fileNames;
	bool useSimpleStorage, _singleAligner, _inCentimeters, _enableHelperMatrix, _zIsTimestamp, startAlignerWhenFull;
	std::string outFilename, _matrixOutDir, _binaryPairFileDirectory;
	bool _firstInitDone;
	bool _multithreaded;

	void alignST();
	void alignMT();

	void alignOne(PndLmdSensorAligner &aligner);
	void resetMTLB(int n, int r, int w);
	void incrementMTLB();
	void checkIOpaths();

	//don't use, doesn't work with root like that
	static void readPairsFromChainMT(std::vector<std::string> files, std::map<int, PndLmdSensorAligner> &aligners, PndLmdAlignManager &manager);

	//produces matrices 0 -> 1,2,3,4,5,6,7,8,9
	Matrix combineMatrix(int id1, int id2, bool aligned);

	//produces matrices i -> ... -> i (should be almost identity matrix)
	Matrix combineCyclicMatrix(int id, bool aligned);

	//generate the file name of a matrix or pair file, so changes must only be made once
	static std::string makeBinaryPairFileName(int overlapId=0, bool incentimeters=true, bool correctionMatrix=false);
	static std::string makeBinaryPairFileName(int sensorOne=0, int sensorTwo=0, bool incentimeters=true, bool correctionMatrix=false);
	static std::string makeMatrixFileName(int overlapId=0, bool incentimeters=true, bool correctionMatrix=false);
	static std::string makeMatrixFileName(int sensorOne=0, int sensorTwo=0, bool incentimeters=true, bool correctionMatrix=false);

	void realignMatrixInLmd(Matrix &matrix, int startId, bool aligned);

public:

	//An empty AlignManager will use standard values. If you want to reuse a manager, call Init() again and set values.
	PndLmdAlignManager();
	virtual ~PndLmdAlignManager();

	// initializes Manager on construction or RESETS every value to default
	void init();

	// returns true if successful or false if aligner has enough pairs
	bool addPair(PndLmdHitPair &pair);

	//adds pairs just like the other function but starts an aligner if it is full
	bool addPairAndStartAligner(PndLmdHitPair &pair);

	//add filename, so the aligner adds the pairs itself
	bool addFile(std::string filename);
	void readFiles();
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
	int addFilesFromDirectory(std::string directory, int maxFiles=0);

	void validate();

	//perform last checks and run calculations on all aligners
	void alignAllSensors();

	void writeDebugInfoOnAllSensors();

	static void loadBar(int current, int total, int resolution, int width, std::string message="");

	void setSimpleStorage(bool val) {
		useSimpleStorage =  val;
	}
	void setMaxPairs(int maxPairs);

	//considers inactive area, guard rings, pixel size etc
	static Matrix transformMatrixFromPixelsToCm(const Matrix &input);

	//legacy function, should be deprecated and no longer used
	void transformGlobalToLmd(Matrix &matrix);

	//helper transformation, since all px matrices are local to the system of
	//the sensor they are from. they need to be transformed to lmd local so we can compare
	//them to the matrices from pndlmddim. aligned should be true, since we don't
	//actually have the misaligned matrices on the real geometry. use aligned=false
	//only when comparing matrices from misaligned geometry
	void transformFromSensorToLmdLocal(Matrix &matrix, int sensorId, bool aligned=true);

	void transformFromLmdLocalToSensor(Matrix &matrix, int sensorId, bool aligned=true);

	//returns a Matrix(4,4) from a TGeoHMatrix
	static Matrix castTGeoHMatrixToMatrix(const TGeoHMatrix &matrix);

	//returns a Matrix(4,1) to use with homogenous matrices
	static Matrix castTVector3toMatrix(const TVector3 &vec);

	//returns a Matrix(4,4) that transforms PX coordinates to CM in teh system of a sensor
	static Matrix getPixelToCentimeterTransformation();

	//returns the actual matrices from PndLmdDim in LMD Local coordinate system
	Matrix getMatrixOfficialGeometry(int fromSensor, int toSensor, bool misaligned);

	//returns the actual matrices from PndLmdDim in panda global coordinate system
	//you should not need this function anymore
	Matrix getMatrixOfficialGeometryGlobal(int fromSensor, int toSensor, bool misaligned);

	//returns the transformation matrix from sensor(aligned)->sensor(misaligned)
	Matrix getCorrectionMatrix(int id);

	//returns the transformation matrix from sensor1(misaligned)->sensor2(misaligned)
	//this matrix is essentially what the ICP finds (when operating in CM mode)
	Matrix getCorrectionMatrix(int id1, int id2);

	//helper matrix, should not be needed anymore
	Matrix getMatrixGlobalToLmd(){
		return castTGeoHMatrixToMatrix(TGeoHMatrix(*(dimension->Get_matrix(-1,-1,-1,-1,-1,-1,true))));
	}
	//helper matrix, should not be needed anymore
	Matrix getMatrixLmdToGlobal(){
		return Matrix::inv(castTGeoHMatrixToMatrix(TGeoHMatrix(*(dimension->Get_matrix(-1,-1,-1,-1,-1,-1,true)))));
	}

	static Matrix makeFourVector(double x, double y, double z);
	static TVector3 castMatrixToTVector3(const Matrix &vec);

	// read and write matrix files to and from disk
	static Matrix readMatrix(std::string filename);
	static bool writeMatrix(Matrix &mat, std::string filename);

	const Matrix& getHelperMatrix() const {
		return helperMatrix;
	}

	void setSingleAligner(bool singleAligner) {
		_singleAligner = singleAligner;
	}

	void setOutFilename(const std::string& filename) {
		outFilename = filename;
		checkIOpaths();
	}

	//don't do any real work, just check all parameters
	void setPretend(bool pretend) {
		_pretend = pretend;
	}

	static std::stringstream* readFile(std::string filename);
	static std::vector<std::vector<double> > readFromCSVFile(std::string filename);

	/*
	 * recursively searches files by extension in curr_directory and adds to list
	 * finds all extensions by default, but you can specify which extension
	 * (or part of filename) you want
	 */
	static int searchFiles(std::string curr_directory, std::vector<std::string> &list, std::string extension="", bool includeSubDirs = true);

	//clear console
	static void clearScreen();

	//searches directories in curr_directory, adds to list
	static int searchDirectories(std::string curr_directory, std::vector<std::string> &list, bool includeSubDirs = true);
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
	void enableHelperMatrix(bool enable);

	void readTrafoMatrix(std::string filename, bool aligned){
		dimension->Read_transformation_matrices(filename, aligned);
	}

	// hide all boost related things from ROOT
	#ifndef __CINT__
	//when supplied with a function object, this function executes in a new thread
	void workerThread( boost::shared_ptr< boost::asio::io_service > io_service );

	//write config file
	static bool writeConfigFile(boost::property_tree::ptree configTree, std::string filename, bool replaceExisting=true);

	//read json config file
	static boost::property_tree::ptree readConfigFile(std::string filename);
	#endif

	void setBinaryPairFileDirectory(const std::string& binaryPairFileDirectory){
		_binaryPairFileDirectory = binaryPairFileDirectory;
	}

	void setStartAlignerWhenFull(bool value) {
		startAlignerWhenFull = value;
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_ */
