/*
 * PndLmdSensorAligner.cxx
 *
 *  Created on: May 6, 2015
 *      Author: Roman Klasen, klasen@kph.uni-mainz.de
 */

#include "PndLmdSensorAligner.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <icpPointToPoint.h>
#include <matrix.h>
#include <PndLmdAlignManager.h>
//using namespace std;

//do NOT delete this, or else abs() is a C style macro that casts a double to an int!!
using std::abs;

using std::cerr;
using std::cout;
using std::make_pair;


PndLmdSensorAligner::PndLmdSensorAligner() {
	_maxNoOfPairs=3e5;
	_forceInstant=true;
	_moduleID=-1;
	nonSanePairs=0;
	skippedPairs=0;
	swappedPairs=0;
	ID1=-1;
	ID2=-1;
	_pairsNormal=false;
	_pairsSimple=false;
	overlapID=-1;
	_inCentimeters=true;
	_success=false;
	_helperMatrix = Matrix::eye(3);
	//std::cout << "PndLmdSensorAligner::Init(): Initialization successful." << std::"\n";

}

PndLmdSensorAligner::~PndLmdSensorAligner() {

}

PndLmdSensorAligner::PndLmdSensorAligner(const PndLmdSensorAligner& other) {
	std::cout << "PndLmdSensorAligner::Warning! Unnecessary copy construction." << "\n";
	_maxNoOfPairs=3e5;
	_forceInstant=true;
	_moduleID=-1;
	nonSanePairs=0;
	skippedPairs=0;
	swappedPairs=0;
	ID1=-1;
	ID2=-1;
	_pairsNormal=false;
	_pairsSimple=false;
	overlapID=-1;
	_success=false;
}

void PndLmdSensorAligner::calculateMatrix() {

	int nPairs;

	if(_pairsNormal && !_pairsSimple){
		nPairs=pairs.size();
	}
	else if(!_pairsNormal && _pairsSimple){
		nPairs=simplePairsSensorOne.size();
	}
	else{
		cout << "Error: inconsistent storage options. This could mean no pairs loaded. \n";
		cout << "pairsNormal: " << _pairsNormal << ", pairsSimple: " << _pairsSimple << "\n";
		_success=false;
		return;
	}

	/*
	cout << "Added " << nPairs << " pairs.\n";
	cout << "Number of non sane pairs: " << nonSanePairs << "\n";
	cout << "Needed to swap " << swappedPairs << " times\n";
	cout << "Needed to skip " << skippedPairs<< " times\n";
	 */
	if(skippedPairs>0){
		cout << "=====================================================\n";
		cout << "WARNING! Invalid pairs in pair file, check your data!\n";
		cout << "=====================================================\n";
	}

	//parameters
	//TODO: set from Manager or parameter file!
	bool eventTimeCheck = true;
	double minDelta = 1e-6;
	int dim = 3;

	// only allow max Pairs!
	if(nPairs > _maxNoOfPairs){
		nPairs=_maxNoOfPairs;
	}

	//check if maxPairs > 0
	if(nPairs<5){
		cerr << "PndLmdSensrAligner::Error: Trying to use less than 5 pairs! (And that's not going to work.) Aborting.\n";
		_success=false;
		return;
	}
	else{
		//	cout << "PndLmdSensrAligner::CalculateMatrix: Using " << nPairs << " pairs.\n";
	}

	double* Model = new double[dim*nPairs];
	double* Template = new double[dim*nPairs];

	//loop over all available pairs, decide later what to use
	for(int ipair=0; ipair<nPairs; ipair++){
		if(_pairsNormal && !_pairsSimple){
			if(_inCentimeters){
				Model[ipair*3+0] = pairs[ipair].getHit1().x();
				Model[ipair*3+1] = pairs[ipair].getHit1().y();
				Model[ipair*3+2] = pairs[ipair].getHit1().z();
				Template[ipair*3+0] = pairs[ipair].getHit2().x();
				Template[ipair*3+1] = pairs[ipair].getHit2().y();
				Template[ipair*3+2] = pairs[ipair].getHit2().z();

				/*
				cout << "mx: " << Model[ipair*3+0];
				cout << ", my: " << Model[ipair*3+1];
				cout << ", mz: " << Model[ipair*3+2] << "\n";

				cout << "tx: " << Template[ipair*3+0];
				cout << ", ty: " << Template[ipair*3+1];
				cout << ", tz: " << Template[ipair*3+2] << "\n";

				if(ipair>100) exit(0);
				*/

			}
			else{
				Model[ipair*3+0] = pairs[ipair].getCol1();
				Model[ipair*3+1] = pairs[ipair].getRow1();
				Template[ipair*3+0] = pairs[ipair].getCol2();
				Template[ipair*3+1] = pairs[ipair].getRow2();
			}
		}
		else if(!_pairsNormal && _pairsSimple){
			Model[ipair*3+0] = simplePairsSensorOne[ipair].first;
			Model[ipair*3+1] = simplePairsSensorOne[ipair].second;
			Template[ipair*3+0] = simplePairsSensorTwo[ipair].first;
			Template[ipair*3+1] = simplePairsSensorTwo[ipair].second;
		}
		else{
			cout << "Fatal: inconsistent storage options in ICP model and template generation.\n This should never happen. \n";
			cout << "pairsNormal: " << _pairsNormal << ", pairsSimple: " << _pairsSimple << "\n";
			exit(1);
		}

		//if(!_inCentimeters){
			//artificial z component
			//Model[ipair*3+2] = (2.0*ipair/(double)nPairs -1.0)*12.0;
			//Template[ipair*3+2] = (2.0*ipair/(double)nPairs -1.0)*12.0;
		//}
	}

	if(_numericCorrection){
		//rotation matrix for numerical stability
		for(int ipairs=0; ipairs<nPairs; ipairs++){
			Matrix vec(4,1);
			vec.val[0][0] = Template[ipairs*3+0];
			vec.val[1][0] = Template[ipairs*3+1];
			vec.val[2][0] = Template[ipairs*3+2];
			vec.val[3][0] = 1;
			vec = _helperMatrix * vec;
			Template[ipairs*3+0] = vec.val[0][0];
			Template[ipairs*3+1] = vec.val[1][0];
			Template[ipairs*3+2] = vec.val[2][0];
		}
	}

	int zeroVals=0;
	int modxinv=0, modyinv=0, modzinv=0;
	int temxinv=0, temyinv=0, temzinv=0;

	for(int iCheck=0; iCheck<dim*nPairs; iCheck++){
		double val1 = abs(Model[iCheck]);
		double val2 = abs(Template[iCheck]);

		if(val1 < 1e-15){

			if(iCheck%3==0){
				//cout << "model xval invalid: " << val1 << "\n";
				modxinv++;
			}
			if(iCheck%3==1){
				//cout << "model yval invalid\n";
				modyinv++;
			}
			if(iCheck%3==2){
				//cout << "model zval invalid\n";
				modzinv++;
			}


			zeroVals++;
			//cout << val1 << "\n";
		}
		if(val2 < 1e-15){

			if(iCheck%3==0){
				//cout << "template xval invalid: " << val2 << "\n";
				temxinv++;
			}
			if(iCheck%3==1){
				//cout << "template yval invalid\n";
				temyinv++;
			}
			if(iCheck%3==2){
				//cout << "template zval invalid\n";
				temzinv++;
			}

			zeroVals++;
			//cout << val2 << "\n";
		}
	}
	// 3 dimension and 2 arrays = 6
	double zeroFactor = zeroVals/((double)nPairs*6.0);
	if(zeroFactor > 0.1 && zeroFactor < 0.3){
		cout << "WARNING. More than 10 % of your entries is zero. That must be a mistake. \n";
		cout << "Also, the kdtree creation could crash. Keep an eye out for that...\n";
		cout << "Zero factor: " << zeroFactor << "\n";
		cout << "model x vals invalid: " << modxinv/(double)nPairs << "\n";
		cout << "model y vals invalid: " << modyinv/(double)nPairs << "\n";
		cout << "model z vals invalid: " << modzinv/(double)nPairs << "\n";
		cout << "templ x vals invalid: " << temxinv/(double)nPairs << "\n";
		cout << "templ y vals invalid: " << temyinv/(double)nPairs << "\n";
		cout << "templ z vals invalid: " << temzinv/(double)nPairs << "\n";

	}
	if(zeroFactor > 0.3){
		cout << "ERROR. More than 30 % of your entries is zero. That must be a mistake. \n";
		cout << "Also, the kdtree creation will crash. Exiting.\n";
		cout << "Zero factor: " << zeroFactor << "\n";
		cout << "model x vals invalid: " << modxinv/(double)nPairs << "\n";
		cout << "model y vals invalid: " << modyinv/(double)nPairs << "\n";
		cout << "model z vals invalid: " << modzinv/(double)nPairs << "\n";
		cout << "templ x vals invalid: " << temxinv/(double)nPairs << "\n";
		cout << "templ y vals invalid: " << temyinv/(double)nPairs << "\n";
		cout << "templ z vals invalid: " << temzinv/(double)nPairs << "\n";
		cout << "=== additional data ===\n";
		cout << "overlap id: " << overlapID << "\n";
		cout << "no of Pairs: " << nPairs << "\n";
		exit(1);
	}

	// start with identity as initial transformation
	// in practice you might want to use some kind of prediction here
	Matrix Rotation = Matrix::eye(3);
	Matrix translation(3,1);

	//perform ICP and store quality parameters
	//attention! dim * nPairs must equal size of model!
	IcpPointToPoint icp(Model,nPairs,dim);
	icp.forceInstantResult(_forceInstant);
	//icp.setMaxIterations(75);
	//icp.setMinDeltaParam(minDelta);
	//icp.setEventTimeCheck(eventTimeCheck);
	icp.fit(Template,nPairs,Rotation,translation,-1);

	/*
	// results
	cout << endl << "Transformation results:\n";
	cout << "R:\n" << Rotation << endl << "\n";
	cout << "t:\n" << translation << endl << "\n";
	 */

	//make 4x4 matrix
	double* tempR = new double[9];
	double* tempT = new double[3];

	Rotation.getData(tempR);
	translation.getData(tempT);

	double* finalMatrix = new double[16];

	finalMatrix[0] = tempR[0];
	finalMatrix[1] = tempR[1];
	finalMatrix[2] = tempR[2];
	finalMatrix[3] = tempT[0];
	finalMatrix[4] = tempR[3];
	finalMatrix[5] = tempR[4];
	finalMatrix[6] = tempR[5];
	finalMatrix[7] = tempT[1];
	finalMatrix[8] = tempR[6];
	finalMatrix[9] = tempR[7];
	finalMatrix[10] = tempR[8];
	finalMatrix[11] = tempT[2];
	finalMatrix[12] = 0;
	finalMatrix[13] = 0;
	finalMatrix[14] = 0;
	finalMatrix[15] = 1;

	resultMatrix = Matrix(4,4);

	std::stringstream alignlog;

	if(icp.hasConverged()){

		_success=true;

		//save matrix!
		resultMatrix.setVal(4,4,finalMatrix);

		if(!_inCentimeters){
			resultMatrix = PndLmdAlignManager::transformMatrixFromPixelsToCm(resultMatrix);
		}

		//and say a few words for the log
		alignlog << "\n";
		alignlog << "====================================================\n";
		alignlog << "icp converged for area " << ID1 << " to " << ID2 << " in " << icp.getInterations() << " iterations.\n";
		alignlog << "pairs available: " << nPairs;
		if(nPairs < 100000){
			alignlog << " (WARNING! This is not enough for accurate alignment!)\n";
		}
		else{
			alignlog << "\n";
		}
		//log << "euclidean fitness score: " << icp.getFitnessScore() << " (that is " << icp.getFitnessScore()/8e-4 << " pixels)"<< "\n";
		alignlog << "euclidean fitness score: " << icp.getFitnessScore();
		if(icp.getFitnessScore() > 0.55){
			alignlog << " (WARNING! This is bad! Should be ~0.55)"; //FIXME: no it should not
		}
		alignlog << "\n";
		alignlog << "minDelta: " << minDelta << "\n";

		alignlog << "EventTimeCheck: ";
		if(eventTimeCheck){
			alignlog << "on (and passed)\n";
		}
		else{
			alignlog << "off\n";
		}
		alignlog << "Force Instant: ";
		if(_forceInstant){
			alignlog << "on\n";
		}
		else{
			alignlog << "off\n";
		}
		alignlog << "====================================================\n";
		alignlog << "grepLine: " << ID1 << "to" << ID2 << ": \t efs=" << icp.getFitnessScore() << " \t nPairs=" << nPairs << "\n";
		alignlog << "====================================================\n";
	}
	else{
		alignlog << "\n";
		alignlog << "====================================================\n";
		alignlog << "CRITICAL ERROR:\n";
		alignlog << "no convergence for sensors " << ID1 << " to " << ID2 << "."<< "\n";
		alignlog << "====================================================\n";
		alignlog << "\n";
		_success=false;
	}

	//cout << alignlog.str();

	delete[] Model;
	delete[] Template;
	delete tempR;
	delete tempT;
	delete finalMatrix;

	// Matrix result is final result. Compare with PndLmdDim
	//cout << resultMatrix << "\n";

	return;
}

void PndLmdSensorAligner::addPair(PndLmdHitPair &pair){

	//only one kind of pairs is allowed
	if(!_pairsNormal && !_pairsSimple){
		_pairsNormal=true;
		_pairsSimple=false;
	}

	//only one kind of pairs is allowed
	else if(!_pairsNormal && _pairsSimple){
		return;
	}

	pair.check();
	if(!pair.isSane()){
		//cerr << "Warning! HitPair is not sane.\n";
		nonSanePairs++;
		return;
	}

	//use ID info from first pair
	if(pairs.size()==0){
		ID1 = pair.getId1();
		ID2 = pair.getId2();
		pairs.push_back(pair);
	}
	if(pairs.size()>0){

		//check if pair order is ok
		if(pair.getId1()==ID1 && pair.getId2()==ID2){
			//nothing, all cool
			pairs.push_back(pair);
		}
		else if(pair.getId2()==ID1 && pair.getId1()==ID2){
			pair.swapHits();
			swappedPairs++;
			pairs.push_back(pair);
		}
		else{
			cout << "invalid pair! need to skip. \n";
			skippedPairs++;
		}
	}
}

void PndLmdSensorAligner::addSimplePair(PndLmdHitPair &pair){

	//only one kind of pairs is allowed
	if(!_pairsNormal && !_pairsSimple){
		_pairsSimple=true;
		_pairsNormal=false;
	}

	//only one kind of pairs is allowed
	if(_pairsNormal && !_pairsSimple){
		return;
	}

	if(simplePairsSensorOne.size() >= _maxNoOfPairs){
		// add no more
		return;
	}

	pair.check();
	if(!pair.isSane()){
		//cerr << "Warning! HitPair is not sane.\n";
		nonSanePairs++;
		return;
	}

	//use ID info from first pair
	if(simplePairsSensorOne.size()==0){
		ID1 = pair.getId1();
		ID2 = pair.getId2();

		if(_inCentimeters){
			simplePairsSensorOne.push_back(make_pair(pair.getHit1().x(), pair.getHit1().y()));
			simplePairsSensorTwo.push_back(make_pair(pair.getHit2().x(), pair.getHit2().y()));
		}
		else{
			simplePairsSensorOne.push_back(make_pair(pair.getCol1(), pair.getRow1()));
			simplePairsSensorTwo.push_back(make_pair(pair.getCol2(), pair.getRow2()));
		}

	}
	if(simplePairsSensorOne.size()>0){

		if(pair.getId1()==ID1 && pair.getId2()==ID2){
			//nothing, all cool
			if(_inCentimeters){
				simplePairsSensorOne.push_back(make_pair(pair.getHit1().x(), pair.getHit1().y()));
				simplePairsSensorTwo.push_back(make_pair(pair.getHit2().x(), pair.getHit2().y()));
			}
			else{
				simplePairsSensorOne.push_back(make_pair(pair.getCol1(), pair.getRow1()));
				simplePairsSensorTwo.push_back(make_pair(pair.getCol2(), pair.getRow2()));
			}
		}
		else if(pair.getId2()==ID1 && pair.getId1()==ID2){
			pair.swapHits();
			swappedPairs++;

			if(_inCentimeters){
				simplePairsSensorOne.push_back(make_pair(pair.getHit1().x(), pair.getHit1().y()));
				simplePairsSensorTwo.push_back(make_pair(pair.getHit2().x(), pair.getHit2().y()));
			}
			else{
				simplePairsSensorOne.push_back(make_pair(pair.getCol1(), pair.getRow1()));
				simplePairsSensorTwo.push_back(make_pair(pair.getCol2(), pair.getRow2()));
			}
		}
		else{
			cout << "invalid pair! need to skip. \n";
			skippedPairs++;
		}
	}
}

//FIXME: doesn't work for simple storage
void PndLmdSensorAligner::printAllPairs() {

	double avgID1 =0;
	double avgID2 =0;
	double avgDist=0;
	double avgHit1x=0;
	double avgHit1y=0;
	double avgHit1z=0;
	double avgHit2x=0;
	double avgHit2y=0;
	double avgHit2z=0;

	cout << "pairs normal: " << _pairsNormal << ", pairs simple: " << _pairsSimple << "\n";
	cout << "number of pairs normal: " << pairs.size() << ", number of simple pairs: " << simplePairsSensorOne.size() << "\n";

	if(overlapID==20){
		for(int i=0; i<simplePairsSensorOne.size(); i++){
			cout << "pair " << i << " x1: " << simplePairsSensorOne[i].first << "\n";
			//cout << "pair " << i << " y1: " << simplePairsSensorOne[i].second << "\n";
			cout << "pair " << i << " x2: " << simplePairsSensorTwo[i].first << "\n";
			//cout << "pair " << i << " x2: " << simplePairsSensorTwo[i].second << "\n";
		}
	}

	if(_pairsNormal){
		for(int i=0; i<pairs.size(); i++){
			//pairs[i].Print();
			avgID1+=pairs[i].getId1();
			avgID2+=pairs[i].getId2();
			avgDist+=pairs[i].getDistance();
			avgHit1x+=pairs[i].getHit1().x();
			avgHit1y+=pairs[i].getHit1().y();
			avgHit1z+=pairs[i].getHit1().z();
			avgHit2x+=pairs[i].getHit2().x();
			avgHit2y+=pairs[i].getHit2().y();
			avgHit2z+=pairs[i].getHit2().z();
		}
		cout << "======================== \n";
		cout << "avg. dist: " << avgDist/pairs.size() << "\n";
		cout << "avg ID1 : " << avgID1/pairs.size() << "\n";
		cout << "avg ID2 : " << avgID2/pairs.size() << "\n";
		cout << "avg hit 1 x : " << avgHit1x/pairs.size() << "\n";
		cout << "avg hit 1 y : " << avgHit1y/pairs.size() << "\n";
		cout << "avg hit 1 z : " << avgHit1z/pairs.size() << "\n";
		cout << "avg hit 2 x : " << avgHit2x/pairs.size() << "\n";
		cout << "avg hit 2 y : " << avgHit2y/pairs.size() << "\n";
		cout << "avg hit 2 z : " << avgHit2z/pairs.size() << "\n";
	}
	else if(_pairsSimple){
		for(int i=0; i<simplePairsSensorOne.size(); i++){
			//pairs[i].Print();
			//avgID1+=pairs[i].getId1();
			//avgID2+=pairs[i].getId2();
			//avgDist+=pairs[i].getDistance();
			avgHit1x+=simplePairsSensorOne[i].first;
			avgHit1y+=simplePairsSensorOne[i].second;
			//avgHit1z+=pairs[i].getHit1().z();
			avgHit2x+=simplePairsSensorTwo[i].first;
			avgHit2y+=simplePairsSensorTwo[i].second;
			//avgHit2z+=pairs[i].getHit2().z();
		}
		cout << "======================== \n";
		cout << "avg. dist: " << avgDist/simplePairsSensorOne.size() << "\n";
		cout << "avg ID1 : " << avgID1/simplePairsSensorOne.size() << "\n";
		cout << "avg ID2 : " << avgID2/simplePairsSensorOne.size() << "\n";
		cout << "avg hit 1 x : " << avgHit1x/simplePairsSensorOne.size() << "\n";
		cout << "avg hit 1 y : " << avgHit1y/simplePairsSensorOne.size() << "\n";
		cout << "avg hit 1 z : " << avgHit1z/simplePairsSensorOne.size() << "\n";
		cout << "avg hit 2 x : " << avgHit2x/simplePairsSensorOne.size() << "\n";
		cout << "avg hit 2 y : " << avgHit2y/simplePairsSensorOne.size() << "\n";
		cout << "avg hit 2 z : " << avgHit2z/simplePairsSensorOne.size() << "\n";
	}


}

bool PndLmdSensorAligner::isValid(double val) {

	bool result=false;

	if(std::isinf(val)){
		cout << "fucker is invalid! is inf\n";
		return false;
	}
	if(std::isnan(val)){
		cout << "fucker is invalid! is nan\n";
		return false;
	}
	return true;
}
//Matrix PndLmdSensorAligner::getMatrix() {
//}
