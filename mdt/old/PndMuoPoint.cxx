#include <iostream>

using std::cout;
using std::endl;

#include "PndMuoPoint.h"

// -----   Default constructor   -------------------------------------------
PndMuoPoint::PndMuoPoint() : FairMCPoint() {
  fTrackID    = -1;
  fDetectorID = -1;
  fEventID    = -1;
  fX          = fY  = fZ =  0.;
  fPx         = fPy = fPz = 0.;
  fTime       =  0.;
  fLength     =  0.;
  fELoss      =  0.;
  kFlag       =  0;
  nSec        = -1;
  nPlane      = -1;
  nStrip      = -1;
  nBox        = -1;
  nTube       = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMuoPoint::PndMuoPoint(Int_t trackID, Int_t detID, Int_t evtID, TVector3 pos,
                        TVector3 mom, Double_t tof, Double_t length,
			 Double_t eLoss, Short_t flag, Short_t sec,
			 Short_t plane, Short_t strip, Short_t box, Short_t tube) {
  fTrackID    = trackID;
  fDetectorID = detID; 
  fEventID    = evtID;
  fX          = pos.X();
  fY          = pos.Y();
  fZ          = pos.Z();
  fPx         = mom.Px();
  fPy         = mom.Py();
  fPz         = mom.Pz();
  fTime       = tof;
  fLength     = length;
  fELoss      = eLoss;
  kFlag       = flag;
  nSec        = sec;
  nPlane      = plane;
  nStrip      = strip;
  nBox        = box;
  nTube       = tube;
}


// -----   Destructor   ----------------------------------------------------
PndMuoPoint::~PndMuoPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndMuoPoint::Print(const Option_t* opt) const {
  cout << "-I- PndMuoPoint: MUO Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV,  flag " << kFlag << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndMuoPoint)
