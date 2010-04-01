// -------------------------------------------------------------------------
// -----                      CbmStsPoint source file                  -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndMvdMCPoint.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMvdMCPoint::PndMvdMCPoint() : FairMCPoint(),
	fXfOut(0.), fYfOut(0.), fZfOut(0.),
	fPxfOut(0.), fPyfOut(0.), fPzfOut(0)
{}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMvdMCPoint::PndMvdMCPoint(Int_t trackID, Int_t detID, TString detName,
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
  SetLink(FairLink(kMCTrack, trackID));
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMvdMCPoint::~PndMvdMCPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void PndMvdMCPoint::Print(const Option_t* opt) const {
	std::cout << *this;
}
// -------------------------------------------------------------------------



ClassImp(PndMvdMCPoint);
