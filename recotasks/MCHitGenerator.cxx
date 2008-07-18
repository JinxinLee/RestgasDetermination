#include "MCHitGenerator.h"
#include "AliceHelixTrackRep.h"
#include "SltHit.h"
#include "Slt2DHit.h"

#include<cmath>

#include<ctime>


int MCHitGenerator::getSeed() {
    std::time_t myTime = std::time(NULL);
    return (int)myTime;
}

MCHitGenerator::MCHitGenerator() {
    rand = new TRandom( getSeed() );
    aHelix = NULL;
}

MCHitGenerator::~MCHitGenerator() {
    delete rand;
}

SltHit* MCHitGenerator::generateSL1D(double z, double a, double b, double dy) {
    double y_mean = a*z + b;
    SltHit* theHit = new SltHit( rand->Gaus(y_mean,dy) , z, dy);
    return theHit;
}

Slt2DHit* MCHitGenerator::generateSL2D(double z, double ax, double bx, double ay, double by, double dx, double dy) {
     double x_mean = ax*z + bx;
     double y_mean = ay*z + by;
     Slt2DHit* theHit = new Slt2DHit( rand->Gaus(x_mean,dx),
                                                     rand->Gaus(y_mean,dy),
                                                     z, dx, dy);
     return theHit;
}

void MCHitGenerator::initHelix(double initialS,
				       const TMatrixT<double>& initialState,
				       const TMatrixT<double>& initialCov) {
  
  if(aHelix != NULL) {
    delete aHelix;
  }
  aHelix = new AliceHelixTrackRep(initialS,initialState,initialCov);
}


PndTpcRecoHit* MCHitGenerator::generateTPC(double r,
						    double dr, double dphi, double dz) {

  TMatrixT<double> newState(5,1);
  aHelix->extrapolate(r,newState);

  PndTpcRecoHit *aHit = new PndTpcRecoHit();

  aHit->setHMatrix(r,aHelix,newState);

  TMatrixT<double> HMatrix(2,5);
  HMatrix = aHit->getHMatrix();

  TMatrixT<double> hitCoord(2,1);
  hitCoord = HMatrix * newState;

  aHit->setHit(rand->Gaus(r,dr),rand->Gaus(hitCoord[0][0]/r,dphi),rand->Gaus(hitCoord[1][0],dz),sqrt(dr*dr+dphi*dphi),dz);
  return aHit;
}

