/*
 * PndLmdSensorAligner.cxx
 *
 *  Created on: May 6, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#include "PndLmdSensorAligner.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include "Eigen/Core"
#include "Eigen/LU"
#include "Eigen/SVD"
#include "Eigen/src/Geometry/Umeyama.h"

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

	forceInstant=true;
	_moduleID=-1;
	nonSanePairs=0;
	skippedPairs=0;
	swappedPairs=0;
	ID1=-1;
	ID2=-1;
	_verbose=0;
	_simpleStorage=true;
	overlapID=-1;
	_inCentimeters=true;
	_success=false;
	_zIsTimestamp=true;
	_numericCorrection=false;
	_helperMatrix = Matrix::eye(3);

	debug=false;

	//cout << "PndLmdSensorAligner::Init(): Initialization successful.\n";
}

PndLmdSensorAligner::PndLmdSensorAligner(){
	init();
}

PndLmdSensorAligner::~PndLmdSensorAligner(){
	//destroy everything. leave nothing standing.
}

PndLmdSensorAligner::PndLmdSensorAligner(const PndLmdSensorAligner& ) { // other //FIXME [R.K.03/2017] unused variable(s)
	std::cout << "PndLmdSensorAligner::Warning! Unnecessary copy-construction." << "\n";
	init();
}

void PndLmdSensorAligner::calculateMatrix() {

	int nPairs;

	if(false){

		const int dim = 3;
		Matrix resultUme = Matrix::eye(4);
		nPairs=simpleSensorOneX.size();

		// Convert to Eigen format
		const int npts = nPairs;

		Eigen::Matrix<double, dim, Eigen::Dynamic> cloud_src (dim, npts);
		Eigen::Matrix<double, dim, Eigen::Dynamic> cloud_tgt (dim, npts);

		if(dim==2){
			for (int i = 0; i < npts; ++i){
				cloud_src (0, i) = simpleSensorOneX[i];
				cloud_src (1, i) = simpleSensorOneY[i];

				cloud_tgt (0, i) = simpleSensorTwoX[i];
				cloud_tgt (1, i) = simpleSensorTwoY[i];
			}
		}
		else if(dim==3){
			for (int i = 0; i < npts; ++i){
				cloud_src (0, i) = simpleSensorOneX[i];
				cloud_src (1, i) = simpleSensorOneY[i];
				cloud_src (2, i) = 0.0;

				cloud_tgt (0, i) = simpleSensorTwoX[i];
				cloud_tgt (1, i) = simpleSensorTwoY[i];
				cloud_tgt (2, i) = 0.0;
			}
		}

		// Call Umeyama directly from Eigen (PCL patched version until Eigen is released)
		auto ume = Eigen::umeyama (cloud_src, cloud_tgt, true);

		//Assign to Matrix
		if(dim==2){
			resultMatrix = Matrix::eye(4);
			resultMatrix.val[0][0] =  ume(0,0);
			resultMatrix.val[0][1] =  ume(0,1);
			resultMatrix.val[0][2] =  0.0;
			resultMatrix.val[0][3] =  ume(0,2);
			resultMatrix.val[1][0] =  ume(1,0);
			resultMatrix.val[1][1] =  ume(1,1);
			resultMatrix.val[1][2] =  0.0;
			resultMatrix.val[1][3] =  ume(1,2);
			resultMatrix.val[2][0] =  ume(2,0);
			resultMatrix.val[2][1] =  ume(2,1);
			resultMatrix.val[2][2] =  0.0;
			resultMatrix.val[2][3] =  0.0;
			resultMatrix.val[3][0] =  0.0;
			resultMatrix.val[3][1] =  0.0;
			resultMatrix.val[3][2] =  0.0;
			resultMatrix.val[3][3] =  1.0;

		}
		else if(dim==3){
			resultMatrix = Matrix::eye(4);
			resultMatrix.val[0][0] =  ume(0,0);
			resultMatrix.val[0][1] =  ume(0,1);
			resultMatrix.val[0][2] =  ume(0,2);
			resultMatrix.val[0][3] =  ume(0,3);
			resultMatrix.val[1][0] =  ume(1,0);
			resultMatrix.val[1][1] =  ume(1,1);
			resultMatrix.val[1][2] =  ume(1,2);
			resultMatrix.val[1][3] =  ume(1,3);
			resultMatrix.val[2][0] =  ume(2,0);
			resultMatrix.val[2][1] =  ume(2,1);
			resultMatrix.val[2][2] =  ume(2,2);
			resultMatrix.val[2][3] =  ume(2,3);
			resultMatrix.val[3][0] =  ume(3,0);
			resultMatrix.val[3][1] =  ume(3,1);
			resultMatrix.val[3][2] =  ume(3,2);
			resultMatrix.val[3][3] =  ume(3,3);
		}

		resultMatrix = PndLmdAlignManager::transformMatrixFromPixelsToCm(resultMatrix);
		_success=true;

		if(debug){
			cout << resultMatrix << "\n";
			//exit(1);
		}

	}
	else{

		if(_simpleStorage){

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
				cout << "PndLmdSensorAligner::calculateMatrix::FATAL. Pair sorting error, pairs vectors have different sizes.\n";
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
			//nPairs=pairs.size();
			cout << "WARNING! non-simpleStorage no longer supported!\n";
			return;
		}

		if(skippedPairs>0){
			cout << "=====================================================\n";
			cout << "WARNING! Invalid pairs in pair file, check your data!\n";
			cout << "=====================================================\n";
		}

		//TODO: set from Manager or parameter file!
		/*
		 * =============== Global Parameters, from file in the future ===================
		 */
		int dim = 3;
		bool eventTimeCheck = true;
		double minDelta = 1e-6;
		reshapePointClouds = false;
		_zIsTimestamp = true;

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

		if(dim==2){
			for(int ipair=0; ipair<nPairs; ipair++){
				Model[ipair*dim+0] = simpleSensorOneX[ipair];
				Model[ipair*dim+1] = simpleSensorOneY[ipair];
				Template[ipair*dim+0] = simpleSensorTwoX[ipair];
				Template[ipair*dim+1] = simpleSensorTwoY[ipair];
			}
		}

		else if(dim==3){
			for(int ipair=0; ipair<nPairs; ipair++){
				Model[ipair*dim+0] = simpleSensorOneX[ipair];
				Model[ipair*dim+1] = simpleSensorOneY[ipair];
				Model[ipair*dim+2] = (double)ipair;
				Template[ipair*dim+0] = simpleSensorTwoX[ipair];
				Template[ipair*dim+1] = simpleSensorTwoY[ipair];
				Template[ipair*dim+2] = (double)ipair;
			}

			Mreshape = computeReshapeMatrix(Model, nPairs, dim);

			if(reshapePointClouds){
				reshapePointCloud(Model, nPairs, dim, Mreshape);
				reshapePointCloud(Template, nPairs, dim, Mreshape);

				if(_verbose==3){
					cout << "Mreshapre before reshape:\n" << Mreshape << "\n";
					cout << "Mreshapre (Model) after reshape:\n" << computeReshapeMatrix(Model, nPairs, dim) << "\n";
					cout << "Mreshapre (Templ) after reshape:\n" << computeReshapeMatrix(Model, nPairs, dim) << "\n";
				}
			}

			//artificial z component, only really relevant if using cm coordinate system
			// UPDATE: well that's not exactly true. If using CM coordinates, the z is artificial
			// as well. So to get comparable results of CM vs PX, we should use this in BOTH cases

			if(_zIsTimestamp){
				if(_verbose==3)
					cout << "applying artificial Z coordinate...\n";

				for(int ipair=0; ipair<nPairs; ipair++){
					Model[ipair*dim+2] = ((2.0*ipair / (double)nPairs - 1.0) * 1e4 );
					Template[ipair*dim+2] = ((2.0*ipair / (double)nPairs - 1.0) * 1e4 );
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
		}

		if(_verbose==3){
			cout << "creating ICP...\n";
		}

		// start with identity as initial transformation
		// in practice you might want to use some kind of prediction here
		Matrix Rotation;
		Matrix translation;

		//perform ICP and store quality parameters
		//attention! dim * nPairs must equal size of model!
		IcpPointToPoint icp(Model,nPairs,dim);

		if(_verbose==3)
			cout << "ICP and model created...\n";

		//TODO: clean this up!
		//prepare Matrices
		if(dim==2){
			Rotation = Matrix::eye(2);
			translation = Matrix(2,1);
		}
		else if(dim==3){
			Rotation = Matrix::eye(3);
			translation = Matrix(3,1);
		}

		icp.forceInstantResult(forceInstant);
		icp.fit(Template,nPairs,Rotation,translation,-1);

		if(_verbose==3)
			cout << "ICP fit step done.\n";

		if(dim==2){

			//make 4x4 matrix
			double* tempR = new double[4];
			double* tempT = new double[2];

			Rotation.getData(tempR);
			translation.getData(tempT);

			double* finalMatrix = new double[16];

			//okay, this is the version that FIRST rotates, THEN translates.
			finalMatrix[0] = tempR[0];
			finalMatrix[1] = tempR[1];
			finalMatrix[2] = 0;
			finalMatrix[3] = tempT[0];
			finalMatrix[4] = tempR[2];
			finalMatrix[5] = tempR[3];
			finalMatrix[6] = 0;
			finalMatrix[7] = tempT[1];
			finalMatrix[8] = 0;
			finalMatrix[9] = 0;
			finalMatrix[10] = 1.0;
			finalMatrix[11] = 0;
			finalMatrix[12] = 0;
			finalMatrix[13] = 0;
			finalMatrix[14] = 0;
			finalMatrix[15] = 1.0;
			resultMatrix = Matrix(4,4);

			//save matrix!
			resultMatrix.setVal(4,4,finalMatrix);

			delete tempR;
			delete tempT;
			delete finalMatrix;

		}
		else if (dim==3){

			//make 4x4 matrix
			double* tempR = new double[9];
			double* tempT = new double[3];

			Rotation.getData(tempR);
			translation.getData(tempT);

			double* finalMatrix = new double[16];

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

			resultMatrix = Matrix(4,4);

			//save matrix!
			resultMatrix.setVal(4,4,finalMatrix);

			delete tempR;
			delete tempT;
			delete finalMatrix;
		}

		std::stringstream alignlog;

		if(icp.hasConverged()){

			_success=true;

			//save matrix!
			//resultMatrix.setVal(4,4,finalMatrix);

			if(reshapePointClouds){
				resultMatrix = Matrix::inv(Mreshape) * resultMatrix * Mreshape;
			}

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
			if(forceInstant){
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
		//		delete tempR;
		//		delete tempT;
		//		delete finalMatrix;

	}
	//aligner is done, pairs can be cleared.

	return;
}

bool PndLmdSensorAligner::addSimplePair(PndLmdHitPair &pair){

	//only one kind of pairs is allowed
	if(!_simpleStorage){
		return false;
	}

	if((int)simpleSensorOneX.size() >= _maxNoOfPairs){
		// add no more
		return false;
	}

	//use ID info from first pair
	if(simpleSensorOneX.size()==0){
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
			return false;
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
		simpleSensorOneZ.push_back(simpleSensorOneZ.size());	//vecor grows, so this is okay

		simpleSensorTwoX.push_back(pair.getCol2());
		simpleSensorTwoY.push_back(pair.getRow2());
		simpleSensorTwoZ.push_back(simpleSensorTwoZ.size());	//vecor grows, so this is okay
	}
	return true;
}

void PndLmdSensorAligner::printAllPairs() {

	double avgDist=0;

	cout << "pairs simple: " << _simpleStorage << "\n";

	RunningStats statsX1;
	RunningStats statsY1;
	RunningStats statsZ1;
	RunningStats statsX2;
	RunningStats statsY2;
	RunningStats statsZ2;

	for(size_t i=0; i<simpleSensorOneX.size(); i++){
		statsX1.Push(simpleSensorOneX[i]);
		statsY1.Push(simpleSensorOneY[i]);
		statsZ1.Push(simpleSensorOneZ[i]);
		statsX2.Push(simpleSensorTwoX[i]);
		statsY2.Push(simpleSensorTwoY[i]);
		statsZ2.Push(simpleSensorTwoZ[i]);
	}
	cout << "======================== \n";
	cout << "avg. dist: " << avgDist/simpleSensorOneX.size() << "\n";

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

bool PndLmdSensorAligner::isValid(double val) {

	if(std::isinf(val)){
		cout << "value is invalid! is inf\n";
		return false;
	}
	if(std::isnan(val)){
		cout << "value is invalid! is nan\n";
		return false;
	}
	return true;
}

bool PndLmdSensorAligner::writePairsToBinary(std::string directory) {

	string filename;					//target directory
	double* pdata;						//array with pairs
	int doublesPerPair=6;				//well, doubles per Pair
	int nPairs=0;

	if(_simpleStorage){

		if(simpleSensorOneX.size() == 0){
			nPairs=0;
		}

		else if(simpleSensorOneX.size() == simpleSensorOneY.size() == simpleSensorOneZ.size() ==
				simpleSensorTwoX.size() == simpleSensorTwoY.size() == simpleSensorTwoZ.size() ){
			nPairs = simpleSensorOneX.size();
		}
		else{
			cout << "PndLmdSensorAligner::ERROR: x, y and z have different amounts of entries.\n";
		}

	}
	else{
		cout << "FATAL: non simple pair storage is no longer supported!\n";
		exit(1);
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
	header[0] = 6;					//header size in double fields
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

	//	if(_simpleStorage){
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

	/*
	 * the write part is easy, just dump everything. read part is more difficult,
	 * need to check file first
	 */

	std::ofstream os(filename.c_str(), std::ios::binary | std::ios::out);
	if ( !os.is_open() )
		return false;
	os.write(reinterpret_cast<const char*>(pdata), std::streamsize(length*sizeof(double)));
	os.close();
	delete[] pdata;
	delete[] header;
	return true;
}

bool PndLmdSensorAligner::readPairsFromBinary(std::string directory) {

	//check if this aligner is using simpleStorage
	if(!_simpleStorage){
		cout << "ERROR. reading binary pairs and storing in legacy mode is no longer supported.\n";
		exit(1);
		return false;
	}

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
	}
	else{
		cout << filename.c_str() << " could not be read!\n";
		return false;
	}

	double* headersizeD = new double[1];
	//read header size
	if(filesize >= sizeof(double)){
		//read header
		std::ifstream is(filename.c_str(), std::ios::binary | std::ios::in);
		if ( !is.is_open() )
			return false;
		is.read(reinterpret_cast<char*>(headersizeD), std::streamsize(sizeof(double)));
		is.close();
	}

	int headersize;
	if(headersizeD[0] < 6){
		//cout << "headersize is " << headersizeD[0] << ", seems to be old format. using 6 for now.";
		headersize=6;
	}
	else{
		headersize=headersizeD[0];
	}

	//read header
	if(filesize >= headersize*sizeof(double)){
		double* header = new double[headersize];

		//read header
		std::ifstream is(filename.c_str(), std::ios::binary | std::ios::in);
		if ( !is.is_open() )
			return false;
		is.read(reinterpret_cast<char*>(header), std::streamsize(headersize*sizeof(double)));
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
		noOfDoubles = nPairs * doublesPerPair + headersize;
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
	int currentIndex=headersize;				//data starts here
	bool error=false;

	bool compareBinaryToStored = false;

	if(compareBinaryToStored){
		if((int)simpleSensorOneX.size()!=nPairs){
			cout << "fatal, can't compare to empty vector.\n";
			return false;
		}
	}

	//from here on, only simpleStorage is supported
	//TODO: 4 doubles per pair (eq in px) is not supported yet
	for(int i=0; i<nPairs; i++){
		//assign pair data
		if(!compareBinaryToStored){
			simpleSensorOneX.push_back(pdata[currentIndex+0]);
			simpleSensorOneY.push_back(pdata[currentIndex+1]);
			simpleSensorOneZ.push_back(pdata[currentIndex+2]);

			simpleSensorTwoX.push_back(pdata[currentIndex+3]);
			simpleSensorTwoY.push_back(pdata[currentIndex+4]);
			simpleSensorTwoZ.push_back(pdata[currentIndex+5]);
		}
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

		//get next index, every pair has doublesPerPair doubles
		currentIndex+=doublesPerPair;
	}

	if(!error){
		if(_verbose==3){
			cout << "file check successful, everything okay!\n";
		}
		//from here on, only simpleStorage is supported
		_simpleStorage=true;
	}

	//delete array!
	delete[] pdata;

	//now, check if ID1 and ID2 can be generated from overlapID
	// wel, thy can, but only from dimension->, so this is done in manager, not here
	return true;
}

void PndLmdSensorAligner::clearPairs() {

	//	if(_simpleStorage){
	lastNoOfPairs=simpleSensorOneX.size();

	//call destructors of the member objects (well, they're doubles, so... yeah.)
	simpleSensorOneX.clear();
	simpleSensorOneY.clear();
	simpleSensorOneZ.clear();
	simpleSensorTwoX.clear();
	simpleSensorTwoY.clear();
	simpleSensorTwoZ.clear();

	//force release of allocated memory by vectors
	vector<double>().swap(simpleSensorOneX);
	vector<double>().swap(simpleSensorOneY);
	vector<double>().swap(simpleSensorOneZ);
	vector<double>().swap(simpleSensorTwoX);
	vector<double>().swap(simpleSensorTwoY);
	vector<double>().swap(simpleSensorTwoZ);

	//	}
	//	else{
	//		lastNoOfPairs=pairs.size();
	//		pairs.clear();
	//	}
}

Matrix PndLmdSensorAligner::computeReshapeMatrix(double* pointCloud, int nPairs, int dim) {

	Matrix result;

	if(_verbose==3){
		cout << "calculating reshape matrix...\n";
	}

	if(dim==2){
		//TODO: skip for now
	}

	else if(dim==3){

		//measure xmin, xmax, ymin, ymax;
		double xmin = pointCloud[0];
		double xmax = xmin;
		double ymin = pointCloud[1];
		double ymax = ymin;
		double zmin = pointCloud[2];
		double zmax = zmin;

		//TODO: dowe really need all pairs? isn't e.g. 1/10th enough?
		for(int iPair=1; iPair<nPairs; iPair++){
			xmin = min(xmin, pointCloud[iPair*dim+0]);
			xmax = max(xmax, pointCloud[iPair*dim+0]);
			ymin = min(ymin, pointCloud[iPair*dim+1]);
			ymax = max(ymax, pointCloud[iPair*dim+1]);
			zmin = min(zmin, pointCloud[iPair*dim+2]);
			zmax = max(zmax, pointCloud[iPair*dim+2]);
		}

		double xSpan = xmax - xmin;
		double ySpan = ymax - ymin;
		double zSpan = zmax - zmin;

		if(_verbose == 3){
			cout << "reshape parameters:\n";
			cout << "xmin, xmax: " << xmin << "," << xmax << "\n";
			cout << "ymin, ymax: " << ymin << "," << ymax << "\n";
			cout << "zmin, zmax: " << zmin << "," << zmax << "\n";
			cout << "xSpan, ySpan, zSpan: " << xSpan << "," << ySpan << "," << zSpan << "\n";
		}

		//create scaling matrix, translation matrix and complete reshaping matrix here

		Matrix Mshift = Matrix::eye(4);
		Matrix Mscale = Matrix::eye(4);

		Mshift.val[0][3] -= (xmin + xmax)/2.0;	//x shift
		Mshift.val[1][3] -= (ymin + ymax)/2.0;	//y shift
		Mshift.val[2][3] -= (zmin + zmax)/2.0;	//z shift

		//		Mscale.val[0][0] *= 10.0 / (xSpan*0.5);	//x scale
		//		Mscale.val[1][1] *= 10.0 / (ySpan*0.5);	//y scale
		//		Mscale.val[2][2] *= 10000.0 / (zSpan*0.5);	//z scale

		result = Mscale * Mshift;
	}

	return result;

}

//TODO: complete this for z dimension!
void PndLmdSensorAligner::reshapePointCloud(double* pointcloud, int nPairs, int dim, Matrix reshapeMatrix) {

	if(_verbose==3){
		cout << "reshaping point cloud...\n";
	}

	if(dim==2){
		//TODO: skip for now
	}

	else if(dim==3){

		//reshape point clouds
		//TODO: this can be optimized by calculating on the values directly and not creating a Matrix object.
		Matrix p1 = Matrix(4,1);

		for(int iPair=0; iPair < nPairs; iPair++){

			p1.val[0][0] =  pointcloud[iPair*dim+0];
			p1.val[1][0] =  pointcloud[iPair*dim+1];
			p1.val[2][0] =  pointcloud[iPair*dim+2];
			p1.val[3][0] =  1.0;

			p1 = reshapeMatrix * p1;

			pointcloud[iPair*dim+0] = p1.val[0][0] / p1.val[3][0];
			pointcloud[iPair*dim+1] = p1.val[1][0] / p1.val[3][0];
			pointcloud[iPair*dim+2] = p1.val[2][0] / p1.val[3][0];
		}
	}
}
