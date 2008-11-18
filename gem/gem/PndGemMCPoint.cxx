// -------------------------------------------------------------------------
// -----                      PndGemPoint source file                  -----
// -----                  Created 27/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndGemMCPoint.h"


// -----   Default constructor   -------------------------------------------
PndGemMCPoint::PndGemMCPoint() : CbmMCPoint() {
  fXfOut = fYfOut = fZfOut = 0;
  fPxfOut      = fPyfOut       = fPzfOut       = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemMCPoint::PndGemMCPoint(Int_t trackID, Int_t detID, TString detName,
			 TVector3 posIn, TVector3 posOut,
			 TVector3 momIn, TVector3 momOut,
			 Double_t tof, Double_t length, Double_t eLoss) 
  : CbmMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss) 
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
PndGemMCPoint::~PndGemMCPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void PndGemMCPoint::Print(const Option_t* opt) const {
	std::cout << *this;
}
// -------------------------------------------------------------------------



ClassImp(PndGemMCPoint);
