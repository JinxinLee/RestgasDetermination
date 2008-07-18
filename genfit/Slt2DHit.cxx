#include<iostream>

#include "AbsTrackRep.h"
#include "Slt2DHit.h"
#include "SL2DTrackRep.h"
#include "LSLTrackRep.h"

#include<cmath>

/*
double Slt2DHit::vecToScalar(const TMatrixT<double>& vec, double s) {
    return std::sqrt(vec[0][0]*vec[0][0] + vec[1][0]*vec[1][0]);
}
*/

double Slt2DHit::residualScalar(double s, 
								const AbsTrackRep* stateVector,
								const TMatrixT<double>& state){
  TMatrixT<double> vec = residualVector(s,stateVector,state);
  return std::sqrt(vec[0][0]*vec[0][0] + vec[1][0]*vec[1][0]);
}


Slt2DHit::~Slt2DHit() {}

Slt2DHit::Slt2DHit() : AbsRecoHit(NparHitRep) {
  HMatrix.ResizeTo(NparHitRep,4);
}

Slt2DHit::Slt2DHit(double x, double y, double z, double dx, double dy) : AbsRecoHit(NparHitRep) {
  setHit(x,y,z,dx,dy);
}


void Slt2DHit::setHit(double x, double y, double z, double dx, double dy) {
    hitCoord[0][0] = x;
    hitCoord[1][0] = y;
    hitCov[0][0] = dx*dx;
    hitCov[1][1] = dy*dy;
    sVal = z;
}

void Slt2DHit::setHMatrix(double s,  const AbsTrackRep* stateVector,
						  const TMatrixT<double>& state) {
    if(dynamic_cast<const SL2DTrackRep*>(stateVector) != NULL) { 
      HMatrix.Zero();
	//I know, since this is the same everytime, it could be done in the
	//the constructor, but I do it here anyway, to make clear that in the
	//case of several track-reps per hit, it would have to be done here
      HMatrix.ResizeTo(NparHitRep,4);
      
      HMatrix[0][0] = s;
      HMatrix[0][1] = 1.;
      HMatrix[0][2] = 0.;
      HMatrix[0][3] = 0.;
      HMatrix[1][0] = 0.;
      HMatrix[1][1] = 0.;
      HMatrix[1][2] = s;
      HMatrix[1][3] = 1.;
    }
    else if(dynamic_cast<const LSLTrackRep*>(stateVector) != NULL){
      HMatrix.Zero();  
	  HMatrix.ResizeTo(NparHitRep,5);
      HMatrix[0][0] = 1;
      HMatrix[1][1] = 1;
    }
    else {
      std::cerr << "Slt2DHit can only handle state vectors\n"
		<< " of type SL2DTrackRep or \n"
                << " of type LSLTrackRep \n -> abort" << std::endl;
      throw;
    }
}
