#include "iostream.h"
#include "CbmPlanePoint.h"

// -----   Default constructor   -------------------------------------------
CbmPlanePoint::CbmPlanePoint() : CbmMCPoint() {
  fX_out      = fY_out  = fZ_out  = 0.;
  fPx_out     = fPy_out = fPz_out = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
// CbmPlanePoint::CbmPlanePoint(Int_t trackID, Int_t detID, TVector3 posIn, 
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
CbmPlanePoint::CbmPlanePoint(Int_t trackID, Int_t detID, TVector3 posIn, 
			 TVector3 posOut, TVector3 momIn, TVector3 momOut, 
                         Double_t tof, Double_t length, Double_t eLoss, Double_t charge,
                         Double_t mass, Bool_t preflag, Bool_t postflag,TString nam,Int_t eventID) 
  : CbmMCPoint(trackID, detID, posIn, momIn, tof, length, eLoss) {
    fX_out  = posOut.X();//cm
    fY_out  = posOut.Y();//cm
    fZ_out  = posOut.Z();
    fX_in  = posIn.X();
    fY_in  = posIn.Y();
    fZ_in  = posIn.Z();
    fPx_out = momOut.Px();
    fPy_out = momOut.Py();
    fPz_out = momOut.Pz();
    fPx_in = momIn.Px();
    fPy_in = momIn.Py();
    fPz_in = momIn.Pz();
    fcharge = charge;
    fmass = mass;
    fpreflag = preflag;
    fpostflag = postflag;
    fCopyNo = detID;
    fPlanenam =nam;
    ftrackID = trackID;
    feventID = eventID;
}


// -----   Destructor   ----------------------------------------------------
CbmPlanePoint::~CbmPlanePoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void CbmPlanePoint::Print(const Option_t* opt) const {
  cout << "-I- CbmPlanePoint: Plane Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(CbmPlanePoint)
