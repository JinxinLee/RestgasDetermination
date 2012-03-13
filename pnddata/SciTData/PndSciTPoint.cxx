/////////////////////////////////////////////////////////////
//
//  CbmSciTPoint
//
//  Geant point for Forward tof  detector
//  created by A. Sanchez
//
///////////////////////////////////////////////////////////////

#include "PndSciTPoint.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndSciTPoint::PndSciTPoint() : FairMCPoint() {
  
  fEventID    = -1;
  fXin          = fYin  = fZin =  0.;
  fPxin         = fPyin = fPzin = 0.;
  fXout         = fYout  = fZout =  0.;
  fPxout        = fPyout = fPzout = 0.;
 



 
 
  fVolumeID =  0;

 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndSciTPoint::PndSciTPoint(Int_t trackID, Int_t evtID,
			 Int_t detID, TString detName,
			 TVector3 posin,
			 TVector3 momin, 
			 TVector3 posout, 
			 TVector3 momout, 
			 Double_t tof, Double_t length,
			 Double_t eLoss)
 : FairMCPoint(trackID, detID, posin, momin, tof, length, eLoss) {
 
  fEventID    = evtID;
  fVolumeID   = detID;


  fXin          = posin.X();
  fYin          = posin.Y();
  fZin          = posin.Z();
  fPxin         = momin.X();
  fPyin         = momin.Py();
  fPzin         = momin.Pz();

  
  
  fXout          = posout.X();
  fYout          = posout.Y();
  fZout          = posout.Z();
  fPxout         = momout.Px();
  fPyout         = momout.Py();
  fPzout         = momout.Pz();
  
 
  
  fDetName = detName;
  
  
 
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
