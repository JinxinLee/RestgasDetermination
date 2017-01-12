/*
 * PndLmdSensorAligner.cxx
 *
 *  Created on: May 6, 2015
 *      Author: Roman Klasen, klasen@kph.uni-mainz.de
 */

#include "PndLmdSensorAligner.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include <icpPointToPoint.h>
#include <matrix.h>
#include <PndLmdAlignManager.h>
#include <RunningStats.h>

//do NOT delete this, or else abs() is a C style macro that casts a double to an int!!
using std::abs;

using std::cerr;
using std::cout;
using std::make_pair;


void PndLmdSensorAligner::init(){
	_maxNoOfPairs=3e5;
	_forceInstant=true;
	_moduleID=-1;
	nonSanePairs=0;
	skippedPairs=0;
	swappedPairs=0;
	ID1=-1;
	ID2=-1;
	_verbose=0;
	_pairsNormal=false;
	_pairsSimple=false;
	overlapID=-1;
	_inCentimeters=true;
	_success=false;
	_zIsTimestamp=true;
	_numericCorrection=false;
	_helperMatrix = Matrix::eye(3);
	//cout << "PndLmdSensorAligner::Init(): Initialization successful.\n";
}

PndLmdSensorAligner::PndLmdSensorAligner(){
	init();
}

PndLmdSensorAligner::~PndLmdSensorAligner(){
	//destroy everything. leave nothing standing.
}

PndLmdSensorAligner::PndLmdSensorAligner(const PndLmdSensorAligner& other) {
	std::cout << "PndLmdSensorAligner::Warning! Unnecessary copy-construction." << "\n";
	init();
}

void PndLmdSensorAligner::calculateMatrix() {

	int nPairs;

	if(_pairsNormal && !_pairsSimple){
		nPairs=pairs.size();
	}
	else if(!_pairsNormal && _pairsSimple){

		//check if all vectors have the same size
		int s1 = simpleSensorOneX.size();
		int s2 = simpleSensorOneY.size();
		int s3 = simpleSensorOneZ.size();

		int s4 = simpleSensorTwoX.size();
		int s5 = simpleSensorTwoY.size();
		int s6 = simpleSensorTwoZ.size();

		if(s1==s2 && s2==s3 && s3==s4 && s4==s5 && s5==s6){
			nPairs=simpleSensorOneX.size();
		}
		else{
			cout << "FATAL. Pair sorting error, pais vectors have different sizes.\n";
			cout << "s1: " << s1 << "\n";
			cout << "s2: " << s2 << "\n";
			cout << "s3: " << s3 << "\n";
			cout << "s4: " << s4 << "\n";
			cout << "s5: " << s5 << "\n";
			cout << "s6: " << s6 << "\n";
			exit(1);
		}
	}
	else{
		cout << "Error: inconsistent storage options. This could mean no pairs loaded. \n";
		cout << "pairsNormal: " << _pairsNormal << ", pairsSimple: " << _pairsSimple << "\n";
		_success=false;
		return;
	}

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

	if(_verbose==3)
		cout << "arranging pairs...\n";

	//ToDO: change the loop layout, there should be no if conditions inside that loop
	// (branch prediction), instead make the if condition outside and the loop inside the ifs.
	//loop over all available pairs and sort them to arrays
	for(int ipair=0; ipair<nPairs; ipair++){
		if(_pairsNormal && !_pairsSimple){
			if(_inCentimeters){
				Model[ipair*3+0] = pairs[ipair].getHit1().x();
				Model[ipair*3+1] = pairs[ipair].getHit1().y();
				Model[ipair*3+2] = pairs[ipair].getHit1().z();
				Template[ipair*3+0] = pairs[ipair].getHit2().x();
				Template[ipair*3+1] = pairs[ipair].getHit2().y();
				Template[ipair*3+2] = pairs[ipair].getHit2().z();

				//FIXME: well, you know this is just wrong, don't you?
				/*
				if(Model[ipair*3+2] > 19 && Model[ipair*3+2] < 21 ){
					Model[ipair*3+2] -= 20;
					Template[ipair*3+2] -= 20;
				}
				else if(Model[ipair*3+2] > 29 && Model[ipair*3+2] < 31 ){
					Model[ipair*3+2] -= 30;
					Template[ipair*3+2] -= 30;
				}
				else if(Model[ipair*3+2] > 39 && Model[ipair*3+2] < 41 ){
					Model[ipair*3+2] -= 40;
					Template[ipair*3+2] -= 40;
				}
				 */

			}
			else{
				Model[ipair*3+0] = pairs[ipair].getCol1();
				Model[ipair*3+1] = pairs[ipair].getRow1();
				Model[ipair*3+2] = ipair;
				Template[ipair*3+0] = pairs[ipair].getCol2();
				Template[ipair*3+1] = pairs[ipair].getRow2();
				Template[ipair*3+2] = ipair;
			}
		}
		else if(!_pairsNormal && _pairsSimple){

			Model[ipair*3+0] = simpleSensorOneX[ipair];
			Model[ipair*3+1] = simpleSensorOneY[ipair];
			Model[ipair*3+2] = simpleSensorOneZ[ipair];
			Template[ipair*3+0] = simpleSensorTwoX[ipair];
			Template[ipair*3+1] = simpleSensorTwoY[ipair];
			Template[ipair*3+2] = simpleSensorTwoZ[ipair];

		}
		else{
			cout << "Fatal: inconsistent storage options in ICP model and template generation.\n This should never happen. \n";
			cout << "pairsNormal: " << _pairsNormal << ", pairsSimple: " << _pairsSimple << "\n";
			exit(1);
		}
	}

	//if we are in px coordinates, always use time stamp as z!
	if(!_inCentimeters){
		//	_zIsTimestamp=true;
	}

	//artificial z component, only really relevant if using cm coordinate system
	if(_zIsTimestamp){
		if(_verbose==3)
			cout << "applying z coordinate...\n";

		for(int ipair=0; ipair<nPairs; ipair++){
			Model[ipair*3+2] = ipair;
			Template[ipair*3+2] = ipair;
		}
	}

	if(_numericCorrection){

		if(_verbose==3)
			cout << "applying numeric correction matrix...\n";

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

	/*
	 * zero factor had to be introduced because a bug in earlier versions led to many entries
	 * being filled with zeros. it shouldn't be needed anymore, but it doesn't cost much and
	 * could still be useful.
	 */

	if(_verbose==3)
		cout << "checking for zero values...\n";

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

	if(_verbose==3)
		printAllPairs();

	if(_verbose==3)
		cout << "creating ICP...\n";

	// start with identity as initial transformation
	// in practice you might want to use some kind of prediction here
	Matrix Rotation = Matrix::eye(3);
	Matrix translation(3,1);

	//perform ICP and store quality parameters
	//attention! dim * nPairs must equal size of model!
	IcpPointToPoint icp(Model,nPairs,dim);

	if(_verbose==3)
		cout << "ICP and model created...\n";

	icp.forceInstantResult(_forceInstant);
	icp.fit(Template,nPairs,Rotation,translation,-1);

	if(_verbose==3)
		cout << "ICP fit step done.\n";

	//make 4x4 matrix
	double* tempR = new double[9];
	double* tempT = new double[3];

	Rotation.getData(tempR);
	translation.getData(tempT);

	double* finalMatrix = new double[16];

	/*
	 * TODO: matrices do NOT commute. That means applying translation THEN rotation is something different
	 * than applying rotation THEN translation. What is done here? Is this even correct? Meybe this was the
	 * bug all along?
	 */

	//okay, this is the version that FIRST rotates, THEN translates.
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

	//TODO: now, try the other way around.

	resultMatrix = Matrix(4,4);

	std::stringstream alignlog;

	if(icp.hasConverged()){

		_success=true;

		//save matrix!
		resultMatrix.setVal(4,4,finalMatrix);

		if(!_inCentimeters){
			//store matrix file already transformed to cm!
			resultMatrix = PndLmdAlignManager::transformMatrixFromPixelsToCm(resultMatrix);
		}

		//and say a few words for the log
		alignlog << "\n";
		alignlog << "====================================================\n";
		alignlog << "icp converged for area " << ID1 << " to " << ID2 << "(overlapID "<< overlapID << ")" << " in " << icp.getInterations() << " iterations.\n";
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
		alignlog << "grepLine: " << ID1 << "to" << ID2 << "(overlapID "<< overlapID << ")" << ": \t efs=" << icp.getFitnessScore() << " \t nPairs=" << nPairs << "\n";
		alignlog << "====================================================\n";
		if(_verbose==3)
			cout << "ICP convergence ok.\n";
	}
	else{
		alignlog << "\n";
		alignlog << "====================================================\n";
		alignlog << "CRITICAL ERROR:\n";
		alignlog << "no convergence for sensors " << ID1 << " to " << ID2 << "(overlapID "<< overlapID << ")" << "."<< "\n";
		alignlog << "====================================================\n";
		alignlog << "\n";
		if(_verbose==3)
			cout << "ICP did not converge!\n";
		_success=false;
	}
	if(_verbose==3)
		cout << alignlog.str();

	delete[] Model;
	delete[] Template;
	delete tempR;
	delete tempT;
	delete finalMatrix;

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

	//this is done elsewhere already
	/*
	pair.check();
	if(!pair.isSane()){
		//cerr << "Warning! HitPair is not sane.\n";
		nonSanePairs++;
		return;
	}
	*/

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

	if(simpleSensorOneX.size() >= _maxNoOfPairs){
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

	}
	//check if IDs are sorted the correct way
	else{
		if(pair.getId1()==ID1 && pair.getId2()==ID2){
			//nothing, all cool
		}
		else if(pair.getId2()==ID1 && pair.getId1()==ID2){
			pair.swapHits();
			swappedPairs++;
		}
		else{
			cout << "invalid pair! need to skip. \n";
			skippedPairs++;
			return;
		}
	}

	//finally, add pair
	if(_inCentimeters){
		simpleSensorOneX.push_back(pair.getHit1().x());
		simpleSensorOneY.push_back(pair.getHit1().y());
		simpleSensorOneZ.push_back(pair.getHit1().z());

		simpleSensorTwoX.push_back(pair.getHit2().x());
		simpleSensorTwoY.push_back(pair.getHit2().y());
		simpleSensorTwoZ.push_back(pair.getHit2().z());
	}
	else{
		simpleSensorOneX.push_back(pair.getCol1());
		simpleSensorOneY.push_back(pair.getRow1());
		simpleSensorOneZ.push_back(simpleSensorOneZ.size());

		simpleSensorTwoX.push_back(pair.getCol2());
		simpleSensorTwoY.push_back(pair.getRow2());
		simpleSensorTwoZ.push_back(simpleSensorTwoZ.size());
	}
}

void PndLmdSensorAligner::addSimplePairOld(PndLmdHitPair &pair){

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
	cout << "number of pairs normal: " << pairs.size() << ", number of simple pairs: " << simpleSensorOneX.size() << "\n";

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

		RunningStats statsX1;
		RunningStats statsY1;
		RunningStats statsZ1;
		RunningStats statsX2;
		RunningStats statsY2;
		RunningStats statsZ2;

		for(int i=0; i<simpleSensorOneX.size(); i++){
			statsX1.Push(simpleSensorOneX[i]);
			statsY1.Push(simpleSensorOneY[i]);
			statsZ1.Push(simpleSensorOneZ[i]);
			statsX2.Push(simpleSensorTwoX[i]);
			statsY2.Push(simpleSensorTwoY[i]);
			statsZ2.Push(simpleSensorTwoZ[i]);
		}
		cout << "======================== \n";
		cout << "avg. dist: " << avgDist/simpleSensorOneX.size() << "\n";
		//TODO: overlap ID
		cout << "avg ID1 : " << ID1 << "\n";
		cout << "avg ID2 : " << ID2 << "\n";
		cout << "---\n";
		cout << "avg hit 1 x : " << statsX1.Mean() << "\n";
		cout << "sig hit 1 x : " << statsX1.StandardDeviation() << "\n";
		cout << "---\n";
		cout << "---\n";
		cout << "avg hit 1 y : " << statsY1.Mean() << "\n";
		cout << "sig hit 1 y : " << statsY1.StandardDeviation() << "\n";
		cout << "---\n";
		cout << "avg hit 1 z : " << statsZ1.Mean() << "\n";
		cout << "sig hit 1 z : " << statsZ1.StandardDeviation() << "\n";
		cout << "---\n";
		cout << "avg hit 2 x : " << statsX2.Mean() << "\n";
		cout << "sig hit 2 x : " << statsX2.StandardDeviation() << "\n";
		cout << "---\n";
		cout << "---\n";
		cout << "avg hit 2 y : " << statsY2.Mean() << "\n";
		cout << "sig hit 2 y : " << statsY2.StandardDeviation() << "\n";
		cout << "---\n";
		cout << "avg hit 2 z : " << statsZ2.Mean() << "\n";
		cout << "sig hit 2 z : " << statsZ2.StandardDeviation() << "\n";
	}
}

bool PndLmdSensorAligner::isValid(double val) {

	//bool result=false; //[R.K. 01/2017] unused variable

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

bool PndLmdSensorAligner::writePairsToBinary(std::string directory) {

	//std::stringstream file_path;		//target directory	DO NOT USE ANYMORE
	string filename;					//target directory
	double* pdata;						//array with pairs
	int doublesPerPair=6;				//well, doubles per Pair
	int nPairs=0;

	if(_pairsSimple){
		nPairs = simpleSensorOneX.size();				//number of pairs, TODO: check if all vectors have same size!
	}
	else{
		nPairs = pairs.size();
	}

	if(nPairs==0){
		cout << "warning: attempting to write empty binary pair file! (no pairs in buffer for overlapID " << overlapID << ")\n";
		return false;
	}

	size_t length = nPairs*doublesPerPair + 6;		//number of raw doubles (including header), remember pairs have 6 doubles

	filename = directory;
	filename += PndLmdAlignManager::makeBinaryPairFileName(overlapID, _inCentimeters);

	//construct header
	double* header = new double[6];
	header[0] = 0.2;				//version
	header[1] = doublesPerPair;		//doubles per pair
	header[2] = nPairs;				//nPairs, read the correct vector!
	header[3] = sizeof(double);		//sizeof(double), check when reading files!
	header[4] = overlapID;			//overlapID
	header[5] = -1;					//value not assigned yet

	pdata = new double[length];

	//save header
	for(int i=0;i<6;i++){
		pdata[i]=header[i];
	}
	//save data
	int currentIndex=6;				//data starts here

	if(_pairsSimple){
		for(int i=0; i<nPairs; i++){
			//first, only assume simple storage
			pdata[currentIndex+0]=simpleSensorOneX[i];
			pdata[currentIndex+1]=simpleSensorOneY[i];
			pdata[currentIndex+2]=simpleSensorOneZ[i];
			pdata[currentIndex+3]=simpleSensorTwoX[i];
			pdata[currentIndex+4]=simpleSensorTwoY[i];
			pdata[currentIndex+5]=simpleSensorTwoZ[i];
			currentIndex+=6;
		}
	}
	else{
		for(int i=0; i<nPairs; i++){
			//first, only assume simple storage
			pdata[currentIndex+0]=pairs[i].getCol1();
			pdata[currentIndex+1]=pairs[i].getRow1();
			pdata[currentIndex+2]=-1;
			pdata[currentIndex+3]=pairs[i].getCol2();
			pdata[currentIndex+4]=pairs[i].getRow2();
			pdata[currentIndex+5]=-1;
			currentIndex+=6;
		}
	}

	/*
	 * the write part is easy, just dump everything. read part is more difficult,
	 * need to check file first
	 */

	std::ofstream os(filename.c_str(), std::ios::binary | std::ios::out);
	if ( !os.is_open() )
		return false;
	os.write(reinterpret_cast<const char*>(pdata), std::streamsize(length*sizeof(double)));
	os.close();
	delete[] pdata, header;
	return true;
}

bool PndLmdSensorAligner::readPairsFromBinary(std::string directory) {

	string filename;				//binary pair file
	//size_t length;				//number of raw doubles, remember pairs have 6 doubles
	double* pdata;					//array with pairs
	size_t filesize;
	size_t doublesize = sizeof(double);
	int nPairs;
	int doublesPerPair;
	int noOfDoubles=0;

	/*
	 * read goes as follows:
	 *
	 * first, check file size.
	 * file size is larger than 6 doubles (header)? -> read header only! else return false;
	 * read header and interpret data from header.
	 * is nPairs*6*sizeof(double) + 6*sizeof(double) (header) == filezise?
	 * is nPairs in header == 6 * sizeof(double) (filesize - header)
	 * if so, file seems okay, read header + file. else return false;
	 * when entire file is read, copy data without header to arrays for ICP	 *
	 */

	filename = directory;
	filename += PndLmdAlignManager::makeBinaryPairFileName(overlapID, _inCentimeters);

	//check if file exists and file size
	std::fstream inStream(filename.c_str(), std::ios::binary|std::ios::in|std::ios::ate);
	if(inStream) {
		std::fstream::pos_type size = inStream.tellg();
		filesize = size;
	} else {
		cout << filename.c_str() << " could not be read!\n";
		return false;
	}

	//read header
	if(filesize >= 6*sizeof(double)){
		double* header = new double[6];

		//read header
		std::ifstream is(filename.c_str(), std::ios::binary | std::ios::in);
		if ( !is.is_open() )
			return false;
		is.read(reinterpret_cast<char*>(header), std::streamsize(6*sizeof(double)));
		is.close();

		//check header
		if(_verbose==3){
			cout << "file version: " << header[0] <<"\n";
			cout << "doubles / pair: " << header[1] << "\n";
			cout << "no of pairs: " << header[2] << "\n";
		}

		//check header
		doublesPerPair = header[1];
		nPairs = header[2];
		noOfDoubles = nPairs * doublesPerPair + 6;
		size_t filesizeMust = sizeof(double) * (noOfDoubles);

		if(doublesize != header[3]){
			cout << "warning! sizeof(double) on this system is different than on system that made this binary!\n";
			//TODO: decide what to do in this case
			exit(1);
			doublesize = header[3];
			return false;
		}

		if(overlapID!=header[4]){
			cout << "error! file name and overlapID do not match! did you rename the file?\n";

			//FIXME: allow this, for now...
			//return false;
		}

		if(filesizeMust == filesize){
			if(_verbose==3)
				cout << "file seems okay!\n";
		}
		else{
			cout << "file is corrupt!\n";
			return false;
		}

		//free allocated space!
		delete[] header;

	}
	else{
		cout << filename.c_str() << " is too small, file corrupt!\n";
		return false;
	}

	pdata = new double[noOfDoubles];

	//actually read file
	std::ifstream is(filename.c_str(), std::ios::binary | std::ios::in);
	if ( !is.is_open() )
		return false;
	is.read(reinterpret_cast<char*>(pdata), std::streamsize(noOfDoubles*sizeof(double)));
	is.close();

	//store data correctly or whatever
	//save data
	int currentIndex=6;				//data starts here
	bool error=false;

	bool compareBinaryToStored = false;

	if(compareBinaryToStored){
		if(simpleSensorOneX.size()!=nPairs){
			cout << "fatal, can't compare to empty vector.\n";
			return false;
		}
	}

	for(int i=0; i<nPairs; i++){
		//assign pair data
		if(!compareBinaryToStored){
			simpleSensorOneX.push_back(pdata[currentIndex+0]);
			simpleSensorOneY.push_back(pdata[currentIndex+1]);
			simpleSensorOneZ.push_back(pdata[currentIndex+2]);

			simpleSensorTwoX.push_back(pdata[currentIndex+3]);
			simpleSensorTwoY.push_back(pdata[currentIndex+4]);
			simpleSensorTwoZ.push_back(pdata[currentIndex+5]);
		}\
		//check read data
		else{
			if(pdata[currentIndex+0]!=simpleSensorOneX[i])
				error=true;
			if(pdata[currentIndex+1]!=simpleSensorOneY[i])
				error=true;
			if(pdata[currentIndex+2]!=simpleSensorOneZ[i])
				error=true;
			if(pdata[currentIndex+3]!=simpleSensorTwoX[i])
				error=true;
			if(pdata[currentIndex+4]!=simpleSensorTwoY[i])
				error=true;
			if(pdata[currentIndex+5]!=simpleSensorTwoZ[i])
				error=true;

			if(error){
				cout << pdata[currentIndex+0] << "|" << simpleSensorOneX[i] << "\n";
				cout << pdata[currentIndex+1] << "|" << simpleSensorOneY[i] << "\n";
				cout << pdata[currentIndex+2] << "|" << simpleSensorOneZ[i] << "\n";
				cout << pdata[currentIndex+3] << "|" << simpleSensorTwoX[i] << "\n";
				cout << pdata[currentIndex+4] << "|" << simpleSensorTwoY[i] << "\n";
				cout << pdata[currentIndex+5] << "|" << simpleSensorTwoZ[i] << "\n";

				cout << "error!\n";
				return false;
			}
		}

		//get next index, every pair has 6 doubles
		currentIndex+=6;
	}

	if(!error){
		if(_verbose==3){
			cout << "file check successful, everything okay!\n";
		}
		_pairsSimple=true;
	}

	//delete array!
	delete[] pdata;

	//now, check if ID1 and ID2 can be generated from overlapID
	return true;
}

void PndLmdSensorAligner::clearPairs() {
	if(_pairsSimple){
		simpleSensorOneX.clear();
		simpleSensorOneY.clear();
		simpleSensorOneZ.clear();
		simpleSensorTwoX.clear();
		simpleSensorTwoY.clear();
		simpleSensorTwoZ.clear();
	}
	else{
		pairs.clear();
	}
}
