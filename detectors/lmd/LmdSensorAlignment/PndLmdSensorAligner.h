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

//forward declarations so we can use a reference to these objects here
class PndLmdAlignManager;

class PndLmdSensorAligner {

private:

	friend class PndLmdAlignQA;
	friend class PndLmdAlignManager;

	bool forceInstant, debug;
	unsigned int maxNoOfPairs, numberOfPairs, lastNoOfPairs, dim;
	std::string inputFilename;
	int _moduleID, overlapID;
	unsigned int nonSanePairs, skippedPairs, swappedPairs, verbose;

	std::vector<std::vector<double> > simplePairs;

	bool inCentimeters, success, zIsTimestamp;

	void printPairSpread(int what = 0);
	void transformPair(Matrix &trafoMatrix, std::vector<double> &pair);
	std::vector<double> getPairSpread(int what = 0);

	bool zeroValCheck();

	//the manager which contains this SensorAligner
	PndLmdAlignManager *superManager;

	//FIXME: in the future, return a TGeoHMatrix
	Matrix resultMatrix;

public:

	PndLmdSensorAligner();
	PndLmdSensorAligner(const PndLmdSensorAligner &other);
	virtual ~PndLmdSensorAligner();

	//every constructor should call this, also resets aligner (even though that never happens in normal use)
	void init();

	// add simplified pair, for size and performance reasons, return false if aligner has enough pairs, return true if successful
	bool addSimplePair(const PndLmdHitPair &pair);

	//TODO: these two functions belong in the manager, not the aligner
	bool writePairsToBinary(const std::string directory);
	bool readPairsFromBinary(const std::string directory);

	//set how many pairs the aligner should use, if higher than available in file, it will use all available
	//set 0 for all
	void setMaximumNumberOfHitPairs(Int_t maxPairs) {
		if (maxPairs > 0) {
			maxNoOfPairs = maxPairs;
			simplePairs.reserve(maxPairs);
		}
	}

	Matrix transformToLmdLocal();
	Matrix transformToSensorOne();

	bool check();

	void clearPairs();

	//deactivate iterative part
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
		// usually, simplePairs contains all pairs, but if it gets cleared,
		// the info is lost, so we save the number of pairs somewhere else.
		int sPairs = simplePairs.size();
		if (sPairs > 0) {
			return sPairs;
		}
		else {
			return lastNoOfPairs;
		}
	}

	void applyDynamicCut(double percent=5.0);

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
	void setInCentimeters(bool value) {
		inCentimeters = value;
	}

	bool successful() {
		return success;
	}

	void setZasTimetamp(bool value) {
		zIsTimestamp = value;
	}

	void verboseLevel(int level) {
		verbose = level;
	}

	void setManager(PndLmdAlignManager* manager) {
		superManager = manager;
	}

	unsigned int getDim() const {
		return dim;
	}

	void setDim(unsigned int value) {
		dim = value;
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDSENSORALIGNER_H_ */
