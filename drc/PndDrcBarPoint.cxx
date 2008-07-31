// -------------------------------------------------------------------------
// -----                     PndDrcBarPoint  source file                   -----
// -----                  Created 09/04/2008  by E. Fioravanti & A. Cecchi               -----
// -------------------------------------------------------------------------


#include "PndDrcBarPoint.h"
#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndDrcBarPoint::PndDrcBarPoint() {
  fPdgCode      = 0;
  fEventID      = 0;
  fAngIn        = 0;
  fNBar         = 0;
  fMass         = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrcBarPoint::PndDrcBarPoint(Int_t trackID, Int_t detID, TVector3 pos, 
			       TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Double_t angIn, Double_t thetaC, Int_t nBar, Int_t eventID, Double_t mass) {
  fTrackID    = trackID;
  fDetectorID = detID;
  fX          = pos.X();
  fY          = pos.Y();
  fZ          = pos.Z();
  fPx         = mom.Px();
  fPy         = mom.Py();
  fPz         = mom.Pz();
  fTime       = tof;
  fLength     = length;
  fPdgCode    = pdgCode; 
  fAngIn      = angIn;
  fThetaC     = thetaC;
  fNBar       = nBar;
  fEventID    = eventID;
  fMass       = mass;
 }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDrcBarPoint::~PndDrcBarPoint() { }
// -------------------------------------------------------------------------



void PndDrcBarPoint::Print(const Option_t* opt) const {
  cout << "-I- PndDrcPoint: DRC Point for trackID---> " << fTrackID 
       << " in detector " << fDetectorID << endl;

  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm "<< endl;

}
ClassImp(PndDrcBarPoint)

