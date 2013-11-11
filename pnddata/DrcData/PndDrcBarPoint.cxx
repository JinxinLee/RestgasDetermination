// -------------------------------------------------------------------------
// -----                     PndDrcBarPoint  source file                   -----
// -----                  Created 09/04/2008  by E. Fioravanti & A. Cecchi               -----
// -------------------------------------------------------------------------


#include "PndDrcBarPoint.h"
#include <iostream>

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndDrcBarPoint::PndDrcBarPoint() 
  :FairMCPoint(), 
  fPdgCode(0),
  fBarN(0.),
  fMass(0.),
   fThetaC(0.),
   fTrackStatus(0.)
{  
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrcBarPoint::PndDrcBarPoint(Int_t trackID, Int_t detID, TVector3 pos, 
			       TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t barN, Int_t eventID, Double_t mass) 
  :FairMCPoint(trackID,detID,pos,mom, tof, length, 0.0, (UInt_t) eventID),
   fPdgCode( pdgCode), 
   fThetaC( thetaC),
   fBarN(   barN),
   fMass(   mass)
{ 
  fTrackStatus = 0;
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

