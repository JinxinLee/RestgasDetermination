/*
 * SensorAligner.cpp
 *
 *  Created on: Jul 29, 2013
 *      Author: roman
 */

#include "SensorAligner.h"
#include <matrix.h>
#include <icp.h>
#include <icpPointToPoint.h>

#include <iostream>
#include <sstream>
#include <PndLmdDim.h>

//ClassImp(SensorAligner);

using namespace std;

SensorAligner::SensorAligner() {
	dimension = PndLmdDim::Instance();

	//FIXME: take special care of these matrices! read them from file
	//This is only for old geometry that has NO inactive areas.
	//float pixelsX = 250;
	//float pixelsY = 250;

	float pixelsX = 247.5;
	float pixelsY = 242.5;

	matrixpixelEdgeToCenter = Matrix::eye(4);
	//matrixpixelEdgeToCenter.val[0][3] += 0.5;
	//matrixpixelEdgeToCenter.val[1][3] += 0.5;

	//KEEP IN MIND! This matrix also takes inactive area into account!!
	matrixactiveEdgeToCenter = Matrix::eye(4);
	matrixactiveEdgeToCenter.val[0][3] -= pixelsX/2;
	matrixactiveEdgeToCenter.val[1][3] -= pixelsY/2;

	matrixscaleActiveToSensor = Matrix::eye(4);
	matrixscaleActiveToSensor.val[0][0]=80e-4;
	matrixscaleActiveToSensor.val[1][1]=80e-4;

	newPixelsToSensorInCm = matrixscaleActiveToSensor * matrixactiveEdgeToCenter * matrixpixelEdgeToCenter;
	newSensorInCmToPixels = Matrix(newPixelsToSensorInCm);
	newSensorInCmToPixels.inv();

}

SensorAligner::SensorAligner(PndLmdDim* tdimension) {
	dimension = tdimension;
	//FIXME: take special care of these matrices! read them from file
	//This is only for old geometry that has NO inactive areas.
	//float pixelsX = 250;
	//float pixelsY = 250;

	float pixelsX = 247.5;
	float pixelsY = 242.5;

	matrixpixelEdgeToCenter = Matrix::eye(4);
	matrixpixelEdgeToCenter.val[0][3] += 0.5;
	matrixpixelEdgeToCenter.val[1][3] += 0.5;

	//KEEP IN MIND! This matrix also takes inactive area into account!!
	matrixactiveEdgeToCenter = Matrix::eye(4);
	matrixactiveEdgeToCenter.val[0][3] -= pixelsX/2;
	matrixactiveEdgeToCenter.val[1][3] -= pixelsY/2;

	matrixscaleActiveToSensor = Matrix::eye(4);
	matrixscaleActiveToSensor.val[0][0]=80e-4;
	matrixscaleActiveToSensor.val[1][1]=80e-4;

	newPixelsToSensorInCm = matrixscaleActiveToSensor * matrixactiveEdgeToCenter * matrixpixelEdgeToCenter;
	newSensorInCmToPixels = Matrix(newPixelsToSensorInCm);
	newSensorInCmToPixels.inv();
}


Matrix SensorAligner::getMatrixFromHitPairs(vector<PndLmdHitPair> &pairs) {

	int nPairs = pairs.size();

	if(nPairs < 5){
		cout << "Error in SensorAligner: Trying to use less than 5 pairs! (And that's not going to work well.) Aborting." << endl;
		exit(1);
	}

	int front=pairs[0].getId1();
	int back=pairs[0].getId2();

	bool forceInstant = true;
	double* set1 = new double[3*nPairs];
	double* set2 = new double[3*nPairs];

	//FIXME: the HitPairs from the root file don't have an event id. fix that!
	for(int j=0;j<nPairs;j++){	//loop over batch

		/*
		set1[3*j+0] = pairs.at(j).getX1();
		set1[3*j+1] = pairs.at(j).getY1();
		set1[3*j+2] = j;
		set2[3*j+0] = pairs.at(j).getX2();
		set2[3*j+1] = pairs.at(j).getY2();
		set2[3*j+2] = j;
		*/
	}

	//this is important, do NOT remove that! The sensor hits are rotated by 180 degrees,
	//and the icp won't find a match if you don't help it a little.
	Matrix R;
	Matrix t;

	R = Matrix::eye(3);
	t = Matrix(3,1);

	double minDelta = 1e-6;
	bool eventTimeCheck = true;

	IcpPointToPoint icp(set1,nPairs,3);
	icp.forceInstantResult(forceInstant);
	icp.setMaxIterations(75);
	icp.setMinDeltaParam(minDelta);
	icp.setEventTimeCheck(eventTimeCheck);
	icp.fit(set2,nPairs,R,t,-1);

	double* tempR = new double[9];
	double* tempT = new double[3];

	R.getData(tempR);
	t.getData(tempT);

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

	Matrix result(4,4);

	stringstream log;

	if(icp.hasConverged()){

		//save matrix!
		result.setVal(4,4,finalMatrix);

		//and say a few words for the log
		log << endl;
		log << "====================================================" << endl;
		log << "icp converged for area " << front << " to " << back << " in " << icp.getInterations() << " iterations." << endl;
		log << "pairs available: " << nPairs;
		if(nPairs < 100000){
			log << " (WARNING! This is not enough for accurate alignment!)" << endl;
		}
		else{
			log << endl;
		}
		//log << "euclidean fitness score: " << icp.getFitnessScore() << " (that is " << icp.getFitnessScore()/8e-4 << " pixels)"<< endl;
		log << "euclidean fitness score: " << icp.getFitnessScore();
		if(icp.getFitnessScore() > 0.55){
			log << " (WARNING! This is bad! Should be ~0.55)";
		}
		log << endl;
		log << "minDelta: " << minDelta << endl;

		log << "EventTimeCheck: ";
		if(eventTimeCheck){
			log << "on (and passed)" << endl;
		}
		else{
			log << "off" << endl;
		}
		log << "Force Instant: ";
		if(forceInstant){
			log << "on" << endl;
		}
		else{
			log << "off" << endl;
		}
		log << "====================================================" << endl;
		log << "grepLine: " << front << "to" << back << ": \t efs=" << icp.getFitnessScore() << " \t nPairs=" << nPairs << endl;
		log << "====================================================" << endl;
	}
	else{
		log << endl;
		log << "====================================================" << endl;
		log << "CRITICAL ERROR:" << endl;
		log << "no convergence for sensors " << front << " to " << back << "."<< endl;
		log << "====================================================" << endl;
		log << endl;
	}

	delete tempR;
	delete tempT;
	delete set1;
	delete set2;
	delete finalMatrix;

	cout << log.str();

	//icp matrices are edge-to-edge. we want center-to-center:
	Matrix result2 = newPixelsToSensorInCm * result * newSensorInCmToPixels;

	return result2;
}

//get transformation matrix of fromSensor -> toSensor
Matrix SensorAligner::getIdealMatrixOfficialGeometry(Int_t fromSensor, Int_t toSensor, bool aligned){

	if(!dimension){
		cout << "Error: PndLmdDim::Instance() did not load matrices!" << endl;
	}

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(fromSensor, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(toSensor, bhalf, bplane, bmodule, bside, bdie, bsensor);

	const TGeoHMatrix& matrix_from = dimension->Get_transformation_sensor_to_lmd_local(fhalf, fplane, fmodule, fside, fdie, fsensor, aligned);
	const TGeoHMatrix& matrix_to   = dimension->Get_transformation_lmd_local_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, aligned);

	TGeoMatrix* matrix = &(matrix_from*matrix_to);

	double* homogenousMatrix = new double[16];
	double* finalMatrix = new double[16];
	matrix->GetHomogenousMatrix(homogenousMatrix);

	//switch root translation position with normal
	//fuckheads of root put their translation at the wrong position because fuck me is why.
	finalMatrix[0] = homogenousMatrix[0];
	finalMatrix[1] = homogenousMatrix[1];
	finalMatrix[2] = homogenousMatrix[2];
	finalMatrix[3] = homogenousMatrix[12];
	finalMatrix[4] = homogenousMatrix[4];
	finalMatrix[5] = homogenousMatrix[5];
	finalMatrix[6] = homogenousMatrix[6];
	finalMatrix[7] = homogenousMatrix[13];
	finalMatrix[8] = homogenousMatrix[8];
	finalMatrix[9] = homogenousMatrix[9];
	finalMatrix[10] = homogenousMatrix[10];
	finalMatrix[11] = homogenousMatrix[14];
	finalMatrix[12] = homogenousMatrix[3];
	finalMatrix[13] = homogenousMatrix[7];
	finalMatrix[14] = homogenousMatrix[11];
	finalMatrix[15] = homogenousMatrix[15];

	//and convert to meters, like any sane person would do
	//finalMatrix[3] /= 100;
	//finalMatrix[7] /= 100;
	//finalMatrix[11] /= 100;

	//now convert to pixels, which is non-final and will go later
	//TODO: make it go
	//finalMatrix[3] /= 80e-6;
	//finalMatrix[7] /= 80e-6;
	//finalMatrix[11] /= 80e-6;

	Matrix result(4,4,finalMatrix);
	delete homogenousMatrix, finalMatrix;
	return result;
}

void SensorAligner::demoICP() {

	cout << "======== AND NOW: DEMO ICP ========" << endl;

	// define a 3 dim problem with 10000 model points
	// and 10000 template points:
	int32_t dim = 3;
	int32_t num = 10000;

	// allocate model and template memory
	double* M = (double*)calloc(3*num,sizeof(double));
	double* T = (double*)calloc(3*num,sizeof(double));

	// set model and template points
	cout << endl << "Creating model with 10000 points ..." << endl;
	cout << "Creating template by shifting model by (1,1,1) ..." << endl;
	int32_t k=0;
	for (double x=-2; x<2; x+=0.04) {
		for (double y=-2; y<2; y+=0.04) {
			double z=5*x*exp(-x*x-y*y);
			M[k*3+0] = x;
			M[k*3+1] = y;
			M[k*3+2] = z;
			T[k*3+0] = x-0.001;
			T[k*3+1] = y+2000;
			T[k*3+2] = z+3e-6;
			k++;
		}
	}

	// start with identity as initial transformation
	// in practice you might want to use some kind of prediction here
	Matrix R = Matrix::eye(3);
	Matrix t(3,1);

	// run point-to-plane ICP (-1 = no outlier threshold)
	cout << endl << "Running ICP (point-to-point, no outliers)" << endl;
	IcpPointToPoint icp(M,num,dim);
	icp.forceInstantResult(true);
	icp.setMaxIterations(200);
	icp.fit(T,num,R,t,-1);

	if(icp.hasConverged()){
		cout << endl << "Transformation results:" << endl;
		cout << "R:" << endl << R << endl << endl;
		cout << "t:" << endl << t << endl << endl;
		cout << "fitness score: " << icp.getFitnessScore() << endl;
		cout << "iterations needed to converge: " << icp.getInterations() << endl;
	}

	// free memory
	free(M);
	free(T);

}


SensorAligner::~SensorAligner() {
}
