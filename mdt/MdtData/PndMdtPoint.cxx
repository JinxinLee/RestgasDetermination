
#include <iostream>

using std::cout;
using std::endl;

#include "PndMdtPoint.h"

// -----   Default constructor   -------------------------------------------
PndMdtPoint::PndMdtPoint() : CbmMCPoint()
{
  fEventID       = -999;
  fTrackID       = -999;
  fTrackParentID = -999;
  fDetectorID       = -999;
  fPDG           = -999;
  fPos.SetXYZT(0.0,0.0,0.0,0.0);
  fMom.SetXYZT(0.0,0.0,0.0,0.0);
  fX             = 0.0;
  fY             = 0.0;
  fZ             = 0.0;
  fPx            = 0.0;
  fPy            = 0.0;
  fPz            = 0.0;
  fELoss	 = 0.0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMdtPoint::PndMdtPoint(Int_t eventID, Int_t trackID, Int_t trackParentID, Int_t detID, Int_t pdg,  
	    		 TLorentzVector pos, TLorentzVector mom)
{
  fEventID       = eventID;
  fTrackID       = trackID;
  fTrackParentID = trackParentID;
  fDetectorID    = detID;
  fPDG           = pdg;
  fPos.SetXYZT(pos.X(),pos.Y(),pos.Z(),pos.T());
  fMom.SetXYZT(mom.X(),mom.Y(),mom.Z(),mom.T());
  fX             = fPos.X();
  fY             = fPos.Y();
  fZ             = fPos.Z();
  fPx            = fMom.X();
  fPy            = fMom.Y();
  fPz            = fMom.Z();
  fELoss	 = fMom.T();
}


// -----   Destructor   ----------------------------------------------------
PndMdtPoint::~PndMdtPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndMdtPoint::Print(const Option_t* opt) const {
  cout << "-I- PndMdtPoint: MDT Point for track " << fTrackID << " in the detector " << fDetectorID << endl;
  cout << " in event " << fEventID+1 << " with parent track " << fTrackParentID 
       << " and its particle PDG " << fPDG << endl;
  cout << "    Position (" << fPos.X() << ", " << fPos.Y() << ", " << fPos.Z() << ", " << fPos.T()
       << ") cm^3*s" << endl;
  cout << "    Momentum (" << fMom.X() << ", " << fMom.Y() << ", " << fMom.Z() << ", " << fMom.T()
       << ") GeV^4" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndMdtPoint)
