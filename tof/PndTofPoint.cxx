/////////////////////////////////////////////////////////////
//
//  CbmHypPoint
//
//  Geant point for Hyp detector
//
//
///////////////////////////////////////////////////////////////

#include "PndTofPoint.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndTofPoint::PndTofPoint() : CbmMCPoint() {
  fTrackID    = -1;
  fEventID    = -1;
  fXin          = fYin  = fZin =  0.;
  fPxin         = fPyin = fPzin = 0.;
  fXout         = fYout  = fZout =  0.;
  fPxout        = fPyout = fPzout = 0.;
 
  fPLin        = fPLout = 0.;

  fTime       =  0.;
  fLength     =  0.;
  fELoss      =  0.;
  fcharge = 0.0;
  fmass   = 0.0;
  fVolumeID = fpdgCode = 0;
  fdist =  0.;
 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndTofPoint::PndTofPoint(Int_t trackID, Int_t evtID,
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
 : CbmMCPoint(trackID, detID, posin, momin, tof, length, eLoss) {
  fTrackID    = trackID;
  fVolumeID = detID;  fEventID    = evtID;
  
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
  
  fTime       = tof;
  fLength     = length;
  fELoss      = eLoss;
  fcharge = charge;
  fmass   = mass;
  
  fpdgCode = pdgCode;
  fdist = dist;
  
 
}


// -----   Destructor   ----------------------------------------------------
PndTofPoint::~PndTofPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndTofPoint::Print(const Option_t* opt) const {
  cout << "-I- PndTofPoint: MUO Point for track " << fTrackID 
       << " in detector " << fVolumeID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndTofPoint)
