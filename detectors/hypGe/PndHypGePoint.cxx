/////////////////////////////////////////////////////////////
//
//  PndHypGePoint
//
//  Geant point for HypGe detector
//

//
///////////////////////////////////////////////////////////////

#include <iostream>
#include "PndHypGePoint.h"

// -----   Default constructor   -------------------------------------------
PndHypGePoint::PndHypGePoint() : FairMCPoint() {
  fTrackID    = -1;
  //fDetectorID = -1;
  fEventID    = -1;
  fX          = fY  = fZ =  0.;
  fPx         = fPy = fPz = 0.;
  fTime       = fcharge = 0.;
  fLength     =  0.;
  fELoss      =  0.;
  fpdgCode = 0;
  fnCopy = -1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndHypGePoint::PndHypGePoint(Int_t trackID, Int_t evtID, Int_t pdgCode,Double_t charge,
			     TVector3 pos,TVector3 mom, Double_t tof, Double_t length,
			 Double_t eLoss, Short_t copy) {
  fTrackID    = trackID;
  //fDetectorID = detID; '
  fcharge = charge;
  fpdgCode = pdgCode;
  
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

  fnCopy       = copy;
}


// -----   Destructor   ----------------------------------------------------
PndHypGePoint::~PndHypGePoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndHypGePoint::Print(const Option_t* opt) const {
  cout << "-I- PndHypGePoint: HYPGE Point for track " << fTrackID 
       << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndHypGePoint)
