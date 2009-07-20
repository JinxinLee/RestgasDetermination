
#include "PndTrackCandHit.h"

void PndTrackCandHit::Print(){
  std::cout << "hit " << fHitId << " | det " 
	    << fDetId << " | rho " << fRho << std::endl;
}



ClassImp(PndTrackCandHit)

