// -------------------------------------------------------------------------
// -----                     PndDrcPDPoint  source file                   -----
// -----                  Created 09/04/2008  by E. Fioravanti & A. Cecchi               -----
// -------------------------------------------------------------------------


#include "PndDrcPDPoint.h"
#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndDrcPDPoint::PndDrcPDPoint() {
  // fTrackID    = -1;
  //fCopyNo = -1;
  //fX          = fY  = fZ =  0.;
  //fPx         = fPy = fPz = 0.;
  //fTime       =  0.;
  //fLength     =  0.;
  //fELoss      =  0.;
  fPdgCode      = 0;
  fEventID      = 0;
  //  fNBar         = 0;

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrcPDPoint::PndDrcPDPoint(Int_t trackID, Int_t detID, TVector3 pos, 
			       TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Int_t eventID) {
  fTrackID    = trackID;
  fCopyNo     = detID;
  fX          = pos.X();
  fY          = pos.Y();
  fZ          = pos.Z();
  fPx         = mom.Px();
  fPy         = mom.Py();
  fPz         = mom.Pz();
  fTime       = tof;
  fLength     = length;
  fPdgCode    = pdgCode; 
  fEventID    = eventID;
 }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDrcPDPoint::~PndDrcPDPoint() { }
// -------------------------------------------------------------------------



void PndDrcPDPoint::Print(const Option_t* opt) const {
  cout << "-I- PndDrcPDPoint: DRC Point for trackID---> " << fTrackID 
       << " in detector " << fCopyNo << endl;

  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm "<< endl;

}
ClassImp(PndDrcPDPoint)

