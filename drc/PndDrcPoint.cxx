#include <iostream>

using std::cout;
using std::endl;

#include "PndDrcPoint.h"

// -----   Default constructor   -------------------------------------------
PndDrcPoint::PndDrcPoint() : CbmMCPoint() {
 }
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
// PndDrcPoint::PndDrcPoint(Int_t trackID, Int_t detID, TVector3 posIn, 
// 			 TVector3 posOut, TVector3 momIn, TVector3 momOut,
// 			 Double_t tof, Double_t length, Double_t eLoss) 
//   : CbmMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss) {
//   fX_out  = posOut.X();
//   fY_out  = posOut.Y();
//   fZ_out  = posOut.Z();
//   fPx_out = momOut.Px();
//   fPy_out = momOut.Py();
//   fPz_out = momOut.Pz();
// }
// -------------------------------------------------------------------------
PndDrcPoint::PndDrcPoint(Int_t trackID, 
			 Int_t copyNo, 
			 TVector3 pos, 
			 TVector3 mom, 
                         Double_t time, 
			 Double_t length, 
			 Double_t eLoss, 
			 Double_t charge,
                         Double_t mass, 
			 Bool_t flag,
			 TString nam,
			 Int_t pdgCode,
			 Int_t eventID) 
  : CbmMCPoint(trackID, copyNo, pos, mom, time, length, eLoss) {
  fX  = pos.X();//cm
  fY  = pos.Y();//cm
  fZ  = pos.Z();
  fPx  = mom.Px();
  fPy  = mom.Py();
  fPz  = mom.Pz();
  fcharge = charge;
  fmass   = mass;
  fflag = flag;
  fELoss = eLoss;
  fCopyNo = copyNo;
  fnam = nam;
  fTrackID  = trackID;
  fPdgCode = pdgCode;
  fEventID  = eventID;
}


// -----   Destructor   ----------------------------------------------------
PndDrcPoint::~PndDrcPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void PndDrcPoint::Print(const Option_t* opt) const {
  cout << "-I- PndDrcPoint: DRC Point for trackID---> " << fTrackID 
       << " in detector " << fCopyNo << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndDrcPoint)
