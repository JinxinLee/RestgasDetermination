/*
 * PndLmdSensorAligner.h
 *
 *  Created on: May 6, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 *
 *      This class gets HitPairs from PndLmdAlignManager and generates transformation matrices for ONE overlapping area.
 *      It works in cm and gives the matrix between sensors, so the hit pairs MUST have valid x, y and z coordinates
 *      in cm.
 *
 *      One SensorAligner is responsible for one overlapping area alone, so 360 aligners are needed for the entire detector.
 *      Therefore, only the PndLmdAlignManager should interact with this class.
 */

#ifndef LMD_LMDSENSORALIGNMENT_PNDLMDSENSORALIGNER_H_
#define LMD_LMDSENSORALIGNMENT_PNDLMDSENSORALIGNER_H_

#include "PndLmdHitPair.h"

#include <string>
#include <vector>

#include <matrix.h>

struct allPairs{

};

class PndLmdSensorAligner{

private:

	friend class PndLmdAlignQA;

	bool forceInstant, reshapePointClouds, debug;
	int _maxNoOfPairs, numberOfPairs, lastNoOfPairs;
	std::string _inputFilename;
	int _moduleID, overlapID;
	int nonSanePairs, skippedPairs, swappedPairs, _verbose;
	int ID1, ID2;

	//complete Pairs, DEPRECATED
	//std::vector<PndLmdHitPair> pairs;

	//simple pairs old UPDATE: this can go, refactoring
	//std::vector<std::pair<double, double> > simplePairsSensorOne;
	//std::vector<std::pair<double, double> > simplePairsSensorTwo;

	//simple pairs new
	std::vector<double> simpleSensorOneX;
	std::vector<double> simpleSensorOneY;
	std::vector<double> simpleSensorOneZ;
	std::vector<double> simpleSensorTwoX;
	std::vector<double> simpleSensorTwoY;
	std::vector<double> simpleSensorTwoZ;

	bool _simpleStorage, _inCentimeters, _success, _numericCorrection, _zIsTimestamp;

	Matrix resultMatrix, _helperMatrix, Mreshape;

public:

	PndLmdSensorAligner();
	PndLmdSensorAligner(const PndLmdSensorAligner &other);
	virtual ~PndLmdSensorAligner();

	//every constructor should call this, also resets aligner (even though that never happens in normal use)
	void init();


	// add simplified pair, for size and performance reasons, return false if aligner has enough pairs, return true if successful
	bool addSimplePair(PndLmdHitPair &pair);

	bool writePairsToBinary(std::string directory);
	bool readPairsFromBinary(std::string directory);

	//set how many pairs the aligner should use, if higher than available in file, it will use all available
	void setMaximumNumberOfHitPairs(Int_t maxPais){
		if(maxPais > 0){
			_maxNoOfPairs=maxPais;

			//check vector capacity to avoid constant re-allocation, use max pairs
			simpleSensorOneX.reserve(maxPais);
			simpleSensorOneY.reserve(maxPais);
			simpleSensorOneZ.reserve(maxPais);
			simpleSensorTwoX.reserve(maxPais);
			simpleSensorTwoY.reserve(maxPais);
			simpleSensorTwoZ.reserve(maxPais);
		}
	}

	void clearPairs();

	//deactivate iterative part, use for debug only
	void setForceInstant(Bool_t instant){
		forceInstant=instant;
	}

	void setModuleID(Int_t ID){
		_moduleID=ID;
	}
	int getModuleID(){
		return _moduleID;
	}
	int getNoOfPairs(){
			return std::max(simpleSensorOneX.size(),(size_t)lastNoOfPairs);
	}

	void calculateMatrix();

	int getOverlapId() const {
		return overlapID;
	}

	void setOverlapId(Int_t overlapId) {
		overlapID = overlapId;
	}

	void printAllPairs();

	const Matrix& getResultMatrix() const {
		return resultMatrix;
	}
	void setHelperMatrix(const Matrix& helperMatrix) {
		_helperMatrix = helperMatrix;
	}

	//true in cm, false in pixels
	void setInCentimeters(bool inCentimeters) {
		_inCentimeters = inCentimeters;
	}

	void setId1(int id1) {
		ID1 = id1;
	}

	void setId2(int id2) {
		ID2 = id2;
	}

	int getId1() const {
		return ID1;
	}

	int getId2() const {
		return ID2;
	}

	bool isValid(double val);

	bool successful(){
		return _success;
	}

	void setNumericCorrection(bool numericCorrection) {
		_numericCorrection = numericCorrection;
	}

	void setZasTimetamp(bool value){
		_zIsTimestamp=value;
	}

	void setReshapePointClouds(bool value) {
		reshapePointClouds = value;
	}

	Matrix computeReshapeMatrix(double* pointCloud, int nPairs, int dim);

	void reshapePointCloud(double* pointcloud, int nPairs, int dim, Matrix reshapeMatrix);

};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDSENSORALIGNER_H_ */
