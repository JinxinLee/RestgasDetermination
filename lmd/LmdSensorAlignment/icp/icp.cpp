/*
Copyright 2011. All rights reserved.
Institute of Measurement and Control Systems
Karlsruhe Institute of Technology, Germany

Authors: Andreas Geiger

libicp is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or any later version.

libicp is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
libicp; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA 
 */

#include "icp.h"

using namespace std;

Icp::Icp (double *M,const int32_t M_num,const int32_t dimension) :
						  dim(dimension), sub_step(10), max_iter(200),
						  T_num(0),iterations(0),min_delta(1e-6),
						  current_delta(1), euclidean_fitness(1e14),
						  hasConvergedBool(false), checkEventTime(false),
						  giveOutputIfNotConvergedB(false),instantForce(false)      {

	// check for correct dimensionality
	if (dimension!=2 && dimension!=3) {
		cout << "ERROR: LIBICP works only for data of dimensionality 2 or 3" << endl;
		M_tree = 0;
		return;
	}

	// check for minimum number of points
	if (M_num<5) {
		cout << "ERROR: LIBICP works only with at least 5 model points" << endl;
		M_tree = 0;
		return;
	}

	// excldue kdTree generation as long as we are fitting non-iteratively
	/*
	// copy model points to M_data
	M_data.resize(boost::extents[M_num][dimension]);
	for (int32_t m=0; m<M_num; m++)
		for (int32_t n=0; n<dimension; n++)
			M_data[m][n] = (double)M[m*dimension+n];
	// build a kd tree from the model point cloud
	M_tree = new kdtree::KDTree(M_data);
	 */
	M_ = M;

}

Icp::~Icp () {
	if (M_tree)
		delete M_tree;
}

double Icp::fit (double *T,const int32_t T_number,Matrix &R,Matrix &t,const double indist) {

	vector<int32_t> active;
	this->T_num = T_number;

	//we don't need a model if fitInstant is called
	if(instantForce){
		active.clear();
		for (int32_t i=0; i<T_number; i++){
			active.push_back(i);
		}
		//cout << "instant fit!" << endl;
		fitInstant(T,T_number,R,t,active);
		hasConvergedBool = true;
		iterations = 1;
		//euclidean_fitness = computeFitnessRMSE(T,R,t); \\that only works if kdTree is present!
		euclidean_fitness = 1.0;	//workaround for now
		return current_delta;
	}

	// make sure we have a model tree
	if (!M_tree) {
		cout << "ERROR: No model available." << endl;
		return 1;
	}

	// check for minimum number of points
	if (T_number<5) {
		cout << "ERROR: Icp works only with at least 5 template points" << endl;
		return 1;
	}

	// coarse matching
	active.clear();
	for (int32_t i=0; i<T_number; i+=sub_step)
		active.push_back(i);

	// cout << "coarse matching..." << endl;
	if(!fitIterate(T,T_number,R,t,active)){
		//cout << "coarse matching failed" << endl;
		return 0;
	}
	// fine matching
	if (indist<=0) {
		active.clear();
		for (int32_t i=0; i<T_number; i++)
			active.push_back(i);
	} else {
		active = getInliers(T,T_number,R,t,indist);
	}
	fitIterate(T,T_number,R,t,active);
	return current_delta;
}

bool Icp::fitIterate(double *T,const int32_t T_number,Matrix &R,Matrix &t,const std::vector<int32_t> &active) {

	// check if we have at least 5 active points
	if (active.size()<5)
		return false;

	// iterate until convergence
	for (int32_t iter=0; iter<max_iter; iter++){
		current_delta=fitStep(T,T_number,R,t,active);
		if (current_delta<min_delta){
			euclidean_fitness = computeFitnessRMSE(T,R,t);

			//TODO: implement for 2D!
			//			if(checkEventTime){
			//				if(abs(t.val[2][0]) > 1){
			//					cout << "false convergence, event id drift is too large!" << endl;
			//					hasConvergedBool = false;
			//					return false;
			//				}
			//			}

			hasConvergedBool=true;
			iterations = iter+1;
			return true;
		}
	}

	if(giveOutputIfNotConvergedB){
		hasConvergedBool = true;
		return true;
	}

	cout << "WARNING: icp did not converge in " << max_iter << " iterations!" << endl;
	hasConvergedBool=false;
	return false;
}

double Icp::computeFitnessRMSE(const double *T, const Matrix &R,const Matrix &t){

	//fitness has either not been computed or is unrealistically small (i.e. for identical data sets)
	//if(euclidean_fitness < 1e-18)
	//	return 0;
	if(T_num == 0){
		cout << "no points in template/model!" << endl;
		return 1e14;
	}

	if(!hasConvergedBool){
		return 1e14;
	}

	if(dim==2){
		// extract matrix and translation vector
		double r00 = R.val[0][0]; double r01 = R.val[0][1];
		double r10 = R.val[1][0]; double r11 = R.val[1][1];
		double t0  = t.val[0][0]; double t1  = t.val[1][0];

		double x0=0,y0=0,x1=0,y1=0,d=0,s=0;

		for(int32_t idx=0; idx<T_num; idx++){

			x0 = r00*T[idx*2+0] + r01*T[idx*2+1] + t0;
			y0 = r10*T[idx*2+0] + r11*T[idx*2+1] + t1;

			x1 = M_data[idx][0];
			y1 = M_data[idx][1];

			d = pow(x0-x1,2) + pow(y0-y1,2);
			s += d/T_num;
		}
		return sqrt(s);
	}

	else if(dim==3){
		// extract matrix and translation vector
		double r00 = R.val[0][0]; double r01 = R.val[0][1]; double r02 = R.val[0][2];
		double r10 = R.val[1][0]; double r11 = R.val[1][1]; double r12 = R.val[1][2];
		double r20 = R.val[2][0]; double r21 = R.val[2][1]; double r22 = R.val[2][2];
		double t0  = t.val[0][0]; double t1  = t.val[1][0]; double t2  = t.val[2][0];

		double x0=0,y0=0,z0=0,x1=0,y1=0,z1=0,d=0,s=0;

		for(int32_t idx=0; idx<T_num; idx++){

			x0 = r00*T[idx*3+0] + r01*T[idx*3+1] + r02*T[idx*3+2] + t0;
			y0 = r10*T[idx*3+0] + r11*T[idx*3+1] + r12*T[idx*3+2] + t1;
			z0 = r20*T[idx*3+0] + r21*T[idx*3+1] + r22*T[idx*3+2] + t2;

			x1 = M_data[idx][0];
			y1 = M_data[idx][1];
			z1 = M_data[idx][2];

			d = pow(x0-x1,2) + pow(y0-y1,2) + pow(z0-z1, 2);
			s += d/T_num;
		}
		return sqrt(s);
	}
	return 0;
}

double Icp::getFitnessScore(){
	if(!hasConvergedBool)
		cout << "ICP has not converged!" << endl;
	return euclidean_fitness;
}


