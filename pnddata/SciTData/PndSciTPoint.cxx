/////////////////////////////////////////////////////////////
//
//  PndSciTPoint
//
//  MC point for Barrel tof  detector
//  created by A. Sanchez
//  modified by D. Steinschaden
//  last update  04.2015/
///////////////////////////////////////////////////////////////

#include "PndSciTPoint.h"
#include "FairEventHeader.h"
#include "FairRun.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndSciTPoint::PndSciTPoint() : FairMCPoint() {

  fXout         = fYout  = fZout =  0.;
  fPxout        = fPyout = fPzout = 0.;
 
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSciTPoint::PndSciTPoint( Int_t eventID, Int_t trackID,
			 Int_t detectorID, TString detName,
			 TVector3 posin,
			 TVector3 momin, 
			 TVector3 posout, 
			 TVector3 momout, 
			 Double_t tof, Double_t length,
			 Double_t eLoss)
 : FairMCPoint(trackID, detectorID, posin, momin, tof, length, eLoss, (UInt_t) eventID) {
 
  fDetName = detName;
 
  fXout          = posout.X();
  fYout          = posout.Y();
  fZout          = posout.Z();
  fPxout         = momout.Px();
  fPyout         = momout.Py();
  fPzout         = momout.Pz();

  FairEventHeader* evtHeader = FairRun::Instance()->GetEventHeader();
  SetLink(FairLink(-1, eventID, "MCTrack", trackID));
}


// -----   Destructor   ----------------------------------------------------
PndSciTPoint::~PndSciTPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndSciTPoint::Print(const Option_t* opt) const {
  cout << "-I- PndSciTPoint: forward tof Point for track " 
       <<" in detector " << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
 
}
// -------------------------------------------------------------------------



ClassImp(PndSciTPoint)
