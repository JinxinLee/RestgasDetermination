/////////////////////////////////////////////////////////////
// PndDchPoint
/////////////////////////////////////////////////////////////
#include <iostream>

using std::cout;
using std::endl;

#include "PndDchPoint.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndDchPoint::PndDchPoint() : FairMCPoint() {
  fEventID = -1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndDchPoint::PndDchPoint(Int_t trackID,Int_t evtID, Int_t chamber, Int_t plane,
			 TVector3 pos, TVector3 mom, 
                         Double_t tof, Double_t length, Double_t eLoss)
  : FairMCPoint(trackID, 0, pos, mom, tof, length, eLoss) {
  SetDetectorID(chamber,plane);
  fEventID = evtID;
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDchPoint::~PndDchPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void PndDchPoint::Print(const Option_t*) const {
  cout << "-I- PndDchPoint: DCH Point for track " << fTrackID 
       << " in chamber " << GetChamber() << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndDchPoint)
