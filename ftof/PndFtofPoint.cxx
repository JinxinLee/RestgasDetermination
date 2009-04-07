/////////////////////////////////////////////////////////////
//
//  CbmFtofPoint
//
//  Geant point for Forward tof  detector
//  created by A. Sanchez
//
///////////////////////////////////////////////////////////////

#include "PndFtofPoint.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndFtofPoint::PndFtofPoint() : FairMCPoint() {
  
  fEventID    = -1;
  fXin          = fYin  = fZin =  0.;
  fPxin         = fPyin = fPzin = 0.;
  fXout         = fYout  = fZout =  0.;
  fPxout        = fPyout = fPzout = 0.;
 
  fPLin        = fPLout = 0.;

 
  fcharge = 0.0;
  fmass   = 0.0;
  fVolumeID = fpdgCode = 0;
  fdist =  0.;
 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndFtofPoint::PndFtofPoint(Int_t trackID, Int_t evtID,
			 Int_t detID, TString detName,
			 TVector3 posin,
			 TVector3 momin, 
			 TVector3 posout, 
			 TVector3 momout, 
			 Double_t tof, Double_t length,
			 Double_t eLoss,Double_t charge,
                         Double_t mass, 
			 Int_t pdgCode,Double_t dist, 
			 Double_t PLin, Double_t PLout)
 : FairMCPoint(trackID, detID, posin, momin, tof, length, eLoss) {
 
  fEventID    = evtID;
  
  fXin          = posin.X();
  fYin          = posin.Y();
  fZin          = posin.Z();
  fPxin         = momin.X();
  fPyin         = momin.Py();
  fPzin         = momin.Pz();

  fPLin         = PLin;
  fPLout         = PLout;
  
  fXout          = posout.X();
  fYout          = posout.Y();
  fZout          = posout.Z();
  fPxout         = momout.Px();
  fPyout         = momout.Py();
  fPzout         = momout.Pz();
  
 
  
  fDetName = detName;
  fcharge = charge;
  fmass   = mass;
  
  fpdgCode = pdgCode;
  fdist = dist;
  
 
}


// -----   Destructor   ----------------------------------------------------
PndFtofPoint::~PndFtofPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndFtofPoint::Print(const Option_t* opt) const {
  cout << "-I- PndFtofPoint: forward tof Point for track " 
       <<" in detector " << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
 
}
// -------------------------------------------------------------------------



ClassImp(PndFtofPoint)
