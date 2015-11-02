/*
 * PndLmdAlignManager.h
 *
 *  Created on: May 26, 2015
 *      Author: Roman Klasen, klasen@kph.uni-mainz.de
 *
 *      This is a manager for all SensorAligner objects. It gathers all PndLmdHitPairs indiscriminately,
 *      and sorts them to 40 (depends on sensor geometry) SensorAligner Objects. It also stores the
 *      resultant matrices and writes them to disk (appropriate format is yet to come, for now just ascii).
 */

#ifndef LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_
#define LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_

#include "PndLmdHitPair.h"

#include <string>

#include <PndLmdSensorAligner.h>
#include <PndLmdDim.h>

class PndLmdAlignManager {

	typedef std::map<int, PndLmdSensorAligner>::iterator mapIt;

private:
	std::map<int, PndLmdSensorAligner> _aligners;
	PndLmdDim *dimension;
	bool _allFilesAdded, _pretend;
	std::vector<std::string> _fileNames;
	bool _useSimpleStorage, _singleAligner, _inCentimeters, _enableHelperMatrix;
	std::string outFilename, _matrixOutDir;

	Matrix helperMatrix;

public:

	PndLmdAlignManager();
	virtual ~PndLmdAlignManager();

	//check ID and sort pair to appropriate SensorAligner
	/* FIXME: observe memory, maybe 40 Aligners with 300k Pairs is too large for memory
	 * sizeof(PndLmdHitPair) is 152, so 300k Pairs are 45.6 MB, so 40 of that are 1.9 GB.
	 * should work... Note: just tested it, ROOT consumes (resident) 1.9 GB, so this is spot-on.
	 * That is a problem, because it's 400 Aligners in total, so 19 GB... that won't work.
	 *
	 * I decreased memory using smaller pair storage mechanisms, maybe it works now
	 */
	bool addPair(PndLmdHitPair &pair);

	//add filename, so the aligner adds the pair itself
	bool addFile(std::string filename);
	void readFiles();

	void validate();

	//perform last checks and run calculations on all aligners
	void alignAllSensors();

	void writeDebugInfoOnAllSensors();

	static void loadBar(int current, int total, int resolution, int width, std::string message="");

	void setSimpleStorage(bool useSimpleStorage) {
		_useSimpleStorage = useSimpleStorage;
	}

	static Matrix transformMatrixFromPixelsToCm(const Matrix &input);
	void transformGlobalToLmd(Matrix &matrix);

	//returns a Matrix(4,4)
	static Matrix castTGeoHMatrixToMatrix(const TGeoHMatrix &matrix);

	//returns a Matrix(4,1) to use with homogenous matrices
	static Matrix castTVector3toMatrix(const TVector3 &vec);

	//returns a TVector3 from a Matrix(3,1) or Matrix(4,1)
	static TVector3 castMatrixToTVector3(const Matrix &vec);

	Matrix getMatrixOfficialGeometry(int fromSensor, int toSensor, bool misaligned);
	Matrix getMatrixOfficialGeometryGlobal(int fromSensor, int toSensor, bool misaligned);
	Matrix getCorrectionMatrix(int id);
	Matrix getCorrectionMatrix(int id1, int id2);

	TVector3 transformMeasuredToTrue(const TVector3 &hit, int sensorID);

	Matrix getMatrixGlobalToLmd(){
		return castTGeoHMatrixToMatrix(TGeoHMatrix(*(dimension->Get_matrix(-1,-1,-1,-1,-1,-1,true))));
	}
	Matrix getMatrixLmdToGlobal(){
		return Matrix::inv(castTGeoHMatrixToMatrix(TGeoHMatrix(*(dimension->Get_matrix(-1,-1,-1,-1,-1,-1,true)))));
	}

	void setSingleAligner(bool singleAligner) {
		_singleAligner = singleAligner;
	}

	void setOutFilename(const std::string& filename) {
		outFilename = filename;
		checkIOpaths();
	}

	void reinitialize();

	void checkIOpaths();

	//don't do any real work, just check all parameters
	void setPretend(bool pretend) {
		_pretend = pretend;
	}

	static std::stringstream* readFile(std::string filename);
	static std::vector<std::vector<double> > readFromCSVFile(std::string filename);
	static Matrix readMatrix(std::string filename);
	bool writeMatrix(Matrix &mat, std::string filename);

	/*
	 * recursively searches files by extension in curr_directory and adds to list
	 * finds all extensions by default, but you can specify which extension
	 * (or part of filename) you want
	 */
	static int searchFiles(std::string curr_directory, std::vector<std::string> &list, std::string extension="", bool includeSubDirs = true);

	//searches directories in curr_directory, adds to list
	static int searchDirectories(std::string curr_directory, std::vector<std::string> &list, bool includeSubDirs = true);
	static bool mkdir(std::string path);
	static std::vector<std::string> findRegex(std::string source, std::string regex);

	void setMatrixOutDir(std::string matrixOutDir) {
		_matrixOutDir = matrixOutDir;
	}

	/*
	 * set this BEFORE reading pair files. if inCentimeters==true, then hits will be decoded by PndLmdDim to centimeters
	 * and all calculations will be done in cm. Misalginment matrices are close to identity matrix, which could introduce
	 * numerical errors.
	 * If inCentimeters==false, then the ICP will work directly on row/col values and matrix will be in pixels. AlignManager
	 * converts matrix to cm, but the resultant matrix is the product of musalignment*ideal!
	 */
	void setInCentimeters(bool inCentimeters);

	void enableHelperMatrix(bool enable);

	static Matrix homogenizeMatrix(const Matrix &input);

	const Matrix& getHelperMatrix() const {
		return helperMatrix;
	}

	void readTrafoMatrix(std::string filename, bool aligned){
		dimension->Read_transformation_matrices(filename, aligned);
	}

	void xOption(int option);

};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNMANAGER_H_ */
