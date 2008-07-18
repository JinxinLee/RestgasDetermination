/////////////////////////////////////////////////////////////
//
//  PndHypPoint
//
//  Geant point for Hyp detector
//
//
///////////////////////////////////////////////////////////////

#include <iostream>
using std::cout;
using std::endl;
#include "PndHypPoint.h"

// -----   Default constructor   -------------------------------------------
PndHypPoint::PndHypPoint() : CbmMCPoint() {
  fTrackID    = -1;
  fEventID    = -1;
  fXin          = fYin  = fZin =  0.;
  fPxin         = fPyin = fPzin = 0.;
  fXout         = fYout  = fZout =  0.;
  fPxout        = fPyout = fPzout = 0.;
  // fXin_local  = fYin_local   = fZin_local   = 0.;
//   fXout_local = fYout_local  = fZout_local  = 0.;
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
PndHypPoint::PndHypPoint(Int_t trackID, 
			 Int_t detID, TString detName,
			 TVector3 posin,
			 TVector3 momin, 
			 TVector3 posout, 
			 TVector3 momout, 
			 //TVector3 posInLocal, TVector3 posOutLocal, 
			 Double_t tof, Double_t length,
			 Double_t eLoss,Double_t charge,
                         Double_t mass, 
			 Int_t pdgCode,Double_t dist, 
			 Double_t PLin, Double_t PLout)
  {
  fTrackID    = trackID;
  fVolumeID = detID; 
  
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
  
 //  fXout_local  = posOutLocal.X();
//   fYout_local  = posOutLocal.Y();
//   fZout_local  = posOutLocal.Z();

//   fXin_local  = posInLocal.X();
//   fYin_local  = posInLocal.Y();
//   fZin_local  = posInLocal.Z();
  
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
PndHypPoint::~PndHypPoint() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndHypPoint::Print(const Option_t* opt) const {
  cout << "-I- PndHypPoint: HYP Point for track " << fTrackID 
       << " in detector " << fVolumeID << endl;
  cout << "    Position (" << fXin << ", " << fYin << ", " << fZin
       << ") cm" << endl;
  cout << "    Momentum (" << fPxin << ", " << fPyin << ", " << fPzin
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndHypPoint)
