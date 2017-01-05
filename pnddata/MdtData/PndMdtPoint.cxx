
#include <iostream>

using std::cout;
using std::endl;

#include "PndMdtPoint.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMdtPoint::PndMdtPoint() : FairMCPoint(), fPosIn(), fMomIn()
{
  
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMdtPoint::PndMdtPoint( Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
			  Double_t tof, Double_t length, Double_t eLoss, 
			  TVector3 pos_in, TVector3 mom_in) 
  :  FairMCPoint(trackID, detID, pos, mom,tof, length, eLoss ), fPosIn(pos_in),  fMomIn(mom_in)
{
  SetLink(FairLink("MCTrack", trackID));
}


// -----   Destructor   ----------------------------------------------------
PndMdtPoint::~PndMdtPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndMdtPoint::Print(const Option_t* opt) const {
  cout << "-I- PndMdtPoint: MDT Point for track " << fTrackID << " in the detector " << fDetectorID << endl;
  cout << " in event " << fEventId << " track length " << fLength << " time " << fTime << " eloss " << fELoss << endl;
  cout << "    Position In (" << fPosIn.X() << ", " << fPosIn.Y() << ", " << fPosIn.Z() << ") cm^3*s" << endl;
  cout << "    Momentum In (" << fMomIn.X() << ", " << fMomIn.Y() << ", " << fMomIn.Z() << ") GeV^4" << endl; 
  cout << "    Position Out (" << fX << ", " << fY << ", " << fZ << ") cm^3*s" << endl;
  cout << "    Momentum Out(" << fPx << ", " << fPy << ", " << fPz << ") GeV^4" << endl;
  cout << " opt = " << opt << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndMdtPoint)
