// -------------------------------------------------------------------------
// -----                      PndGemPoint source file                  -----
// -----                  Created 27/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndGemMCPoint.h"

#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndGemMCPoint::PndGemMCPoint() : FairMCPoint() ,
  fXfOut(0.), fYfOut(0.), fZfOut(0.),
  fPxfOut(0.), fPyfOut(0.), fPzfOut(0.),
  fSensorId(0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemMCPoint::PndGemMCPoint(Int_t trackID, Int_t detID, Int_t sensID,
			     TVector3 posIn, TVector3 posOut,
			     TVector3 momIn, TVector3 momOut,
			     Double_t tof, Double_t length, Double_t eLoss) 
  : FairMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss) ,
  fXfOut(posOut.X()),
  fYfOut(posOut.Y()),
  fZfOut(posOut.Z()),
  fPxfOut(momOut.Px()),
  fPyfOut(momOut.Py()),
  fPzfOut(momOut.Pz()),
  fSensorId(sensID)
{
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemMCPoint::~PndGemMCPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void PndGemMCPoint::Print(const Option_t* opt) const {
	std::cout << *this << " opt = " << opt << std::endl;
}
// -------------------------------------------------------------------------



ClassImp(PndGemMCPoint);
