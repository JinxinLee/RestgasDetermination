// -------------------------------------------------------------------------
// -----                      PndGemPoint source file                  -----
// -----                  Created 27/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndTpcTBMCPoint.h"


// -----   Default constructor   -------------------------------------------
PndTpcTBMCPoint::PndTpcTBMCPoint() : FairMCPoint() {
  fXfOut = fYfOut = fZfOut = 0;
  fPxfOut      = fPyfOut       = fPzfOut       = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndTpcTBMCPoint::PndTpcTBMCPoint(Int_t trackID, Int_t detID, TString detName,
			     TVector3 posIn, TVector3 posOut,
			     TVector3 momIn, TVector3 momOut,
			     Double_t tof, Double_t length, Double_t eLoss) 
  : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss) 
{
  fXfOut = posOut.X();
  fYfOut = posOut.Y();
  fZfOut = posOut.Z();

  fPxfOut = momOut.Px();
  fPyfOut = momOut.Py();
  fPzfOut = momOut.Pz();

  fDetName = detName;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndTpcTBMCPoint::~PndTpcTBMCPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void PndTpcTBMCPoint::Print(const Option_t* opt) const {
	std::cout << *this;
}
// -------------------------------------------------------------------------



ClassImp(PndTpcTBMCPoint);
