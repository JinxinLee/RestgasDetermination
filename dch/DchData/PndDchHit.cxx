// -------------------------------------------------------------------------
// PndDchHit class definition
// -------------------------------------------------------------------------


#include <iostream>
#include "PndDchHit.h"


// -----   Default constructor   -------------------------------------------
PndDchHit::PndDchHit() {
}

// -----   Standard constructor   ------------------------------------------
PndDchHit::PndDchHit(Int_t trackID, Int_t detID, 
		     TVector3 pos, TVector3 dpos,
		     Int_t index) 
  : CbmHit(detID, pos, dpos, index) {
  fTrackID = trackID;
}

// -----   Destructor   ----------------------------------------------------
PndDchHit::~PndDchHit() {}

// -----   Public method Print   -------------------------------------------
void PndDchHit::Print(const Option_t* opt) const {
  std::cout << "dch hit in detector " << fDetectorID << " at ("
       << fX << ", " << fY << ", " << fZ << ") cm " 
       << ", Point " << fRefIndex << std::endl;
}
// -------------------------------------------------------------------------
ClassImp(PndDchHit)
