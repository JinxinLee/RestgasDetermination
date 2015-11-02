/*
 * PndLmdSensorAligner.h
 *
 *  Created on: May 6, 2015
 *      Author: Roman Klasen, klasen@kph.uni-mainz.de
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

class PndLmdSensorAligner{

private:

	bool _forceInstant;
	int _maxNoOfPairs;
	std::string _inputFilename;
	int _moduleID, overlapID;
	int nonSanePairs, skippedPairs, swappedPairs;
	int ID1, ID2;

	std::vector<PndLmdHitPair> pairs;
	std::vector<std::pair<double, double> > simplePairsSensorOne;
	std::vector<std::pair<double, double> > simplePairsSensorTwo;
	bool _pairsNormal, _pairsSimple, _inCentimeters, _success, _numericCorrection;

	Matrix resultMatrix, _helperMatrix;

public:

	PndLmdSensorAligner();
	PndLmdSensorAligner(const PndLmdSensorAligner &other);
	virtual ~PndLmdSensorAligner();

	// add pair, make to vector
	void addPair(PndLmdHitPair &pair);

	// add simplified pair, for size and perfomance reasons
	void addSimplePair(PndLmdHitPair &pair);

	//set how many pairs the aligner should use, if higher than available in file, it will use all available
	void setMaximumNumberOfHitPairs(Int_t maxPais){

		if(maxPais > 0){
			_maxNoOfPairs=maxPais;
			//check vector capacity to avoid constant re-allocation, use max pairs
		}
	}

	//deactivate iterative part, use for debug only
	void forceInstant(Bool_t instant){
		_forceInstant=instant;
	}

	void setModuleID(Int_t ID){
		_moduleID=ID;
	}
	int getModuleID(){
		return _moduleID;
	}
	int getNoOfPairs(){
		if(_pairsSimple && !_pairsNormal){
			return simplePairsSensorOne.size();
		}
		else if(!_pairsSimple && _pairsNormal){
			return pairs.size();
		}
		else{
			return -1;
		}
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

	//true in cm, false in pixels
	//FIXME: only works with non-simplified pairs, fix this!
	void setInCentimeters(bool inCentimeters) {
		_inCentimeters = inCentimeters;
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

	void setHelperMatrix(const Matrix& helperMatrix) {
		_helperMatrix = helperMatrix;
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDSENSORALIGNER_H_ */
