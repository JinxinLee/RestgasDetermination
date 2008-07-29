// -------------------------------------------------------------------------
// -----                       PndDrcHit source file                   -----
// -----               Created 12/11/07  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndDrcHit.h"

// -----   Default constructor   -------------------------------------------
PndDrcHit::PndDrcHit(){
  fThetaC = -999.;
  fErrThetaC = -999.;
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------

PndDrcHit::PndDrcHit(Int_t detID, 
		     TVector3 pos, 
		     TVector3 dpos,
		     Double_t thetaC,
		     Double_t errThetaC,
		     Int_t index){
  fDetectorID = detID;
  fX          = pos.X();
  fY          = pos.Y();
  fZ          = pos.Z();
  fDx         = dpos.Px();
  fDy         = dpos.Py();
  fDz         = dpos.Pz();
  fThetaC     = thetaC;
  fErrThetaC  = errThetaC;
  fRefIndex   = index;
}

// -----   Destructor   ----------------------------------------------------
PndDrcHit::~PndDrcHit(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndDrcHit::Print(const Option_t* opt) const {
  cout << "DIRC hit: ThetaC = " << fThetaC << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndDrcHit)
