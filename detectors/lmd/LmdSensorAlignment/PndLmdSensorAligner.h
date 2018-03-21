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

class PndLmdSensorAligner {

private:

	friend class PndLmdAlignQA;

	bool forceInstant, debug;
	int _maxNoOfPairs, numberOfPairs, lastNoOfPairs;
	std::string _inputFilename;
	int _moduleID, overlapID;
	int nonSanePairs, skippedPairs, swappedPairs, verbose;

	std::vector<double> simpleSensorOneX, simpleSensorOneY, simpleSensorOneZ;
	std::vector<double> simpleSensorTwoX, simpleSensorTwoY, simpleSensorTwoZ;

	bool _inCentimeters, _success, _zIsTimestamp;

	Matrix resultMatrix;

public:

	PndLmdSensorAligner();
	PndLmdSensorAligner(const PndLmdSensorAligner &other);
	virtual ~PndLmdSensorAligner();

	//every constructor should call this, also resets aligner (even though that never happens in normal use)
	void init();

	// add simplified pair, for size and performance reasons, return false if aligner has enough pairs, return true if successful
	bool addSimplePair(const PndLmdHitPair &pair);

	bool writePairsToBinary(const std::string directory);
	bool readPairsFromBinary(const std::string directory);

	//set how many pairs the aligner should use, if higher than available in file, it will use all available
	void setMaximumNumberOfHitPairs(Int_t maxPais) {
		if (maxPais > 0) {
			_maxNoOfPairs = maxPais;

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
	void setForceInstant(Bool_t instant) {
		forceInstant = instant;
	}

	void setModuleID(Int_t ID) {
		_moduleID = ID;
	}
	int getModuleID() {
		return _moduleID;
	}
	int getNoOfPairs() {
		return std::max(simpleSensorOneX.size(), (size_t) lastNoOfPairs);
	}

	void calculateMatrix();

	int getOverlapId() const {
		return overlapID;
	}

	void setOverlapId(Int_t overlapId) {
		overlapID = overlapId;
	}

	const Matrix& getResultMatrix() const {
		return resultMatrix;
	}

	//true in cm, false in pixels
	void setInCentimeters(bool inCentimeters) {
		_inCentimeters = inCentimeters;
	}

	bool successful() {
		return _success;
	}

	void setZasTimetamp(bool value) {
		_zIsTimestamp = value;
	}

	void verboseLevel(int level){
		verbose = level;
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDSENSORALIGNER_H_ */
