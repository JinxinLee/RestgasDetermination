#include<iostream>

#include"AbsTrackRep.h"
#include"SltHit.h"
#include"SLTrackRep.h"
#include"SL2DTrackRep.h"

/*
double SltHit::vecToScalar(const TMatrixT<double>& vec, double s) {
    return vec[0][0];
}
*/

double SltHit::residualScalar(double s, 
							  const AbsTrackRep* stateVector,
							  const TMatrixT<double>& state) {

  TMatrixT<double> vec = residualVector(s,stateVector,state);
  return vec[0][0];
}

SltHit::~SltHit() {}

SltHit::SltHit() : AbsRecoHit(NparHitRep) {
}

SltHit::SltHit(double y, double z, double dy) : AbsRecoHit(NparHitRep) {
    setHit(y,z,dy);
}


void SltHit::setHit(double y, double z, double dy) {
    hitCoord[0][0] = y;
    hitCov[0][0] = dy*dy;
    sVal = z;
}

void SltHit::setHMatrix(double s,  const AbsTrackRep* stateVector,
						const TMatrixT<double>& state) {
    if(dynamic_cast<const SLTrackRep*>(stateVector) != NULL) {
      HMatrix.ResizeTo(NparHitRep,2);
      HMatrix[0][0] = s;
      HMatrix[0][1] = 1.;
    }
    else {
      if(dynamic_cast<const SL2DTrackRep*>(stateVector) != NULL) {
		HMatrix.ResizeTo(NparHitRep,4);
		HMatrix[0][0] = s;
		HMatrix[0][1] = 1.;
		HMatrix[0][2] = 0.;
		HMatrix[0][3] = 0.;
      }
      else {
        std::cerr << "SltHit can only handle state vectors" 
				  << " of type SLTrackRep or SL2DTrackRep-> abort" 
				  << std::endl;
        throw;
		
      }
    }



}
