// -------------------------------------------------------------------------
// -----                       PndDrcPDHit source file                   -----
// -----               Created 12/11/07  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndDrcPDHit.h"

// -----   Default constructor   -------------------------------------------
PndDrcPDHit::PndDrcPDHit(){
  fTime = -999.;
  fTimeThreshold = -999.;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------

PndDrcPDHit::PndDrcPDHit(Int_t detID, 
		     TVector3 pos, 
		     TVector3 dpos,
		     Double_t time,
		     Double_t timeThreshold,
		     Int_t index){
  fDetectorID = detID;
  fX          = pos.X();
  fY          = pos.Y();
  fZ          = pos.Z();
  fDx         = dpos.Px();
  fDy         = dpos.Py();
  fDz         = dpos.Pz();
  fTime       = time;
  fTimeThreshold  = timeThreshold;
  fRefIndex   = index;
}

// -----   Destructor   ----------------------------------------------------
PndDrcPDHit::~PndDrcPDHit(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndDrcPDHit::Print(const Option_t* opt) const {
  cout << "DIRC Photon hit: Time = " << fTime << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndDrcPDHit)
