#include "PndFtsPoint.h"
#include "PndDetectorList.h"

#include <iostream>
#include "math.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndFtsPoint::PndFtsPoint() : FairMCPoint() {
  fX_in_local  = fY_in_local   = fZ_in_local   = 0.;
  fX_out_local = fY_out_local  = fZ_out_local  = 0.;
  fPx_in       = fPy_in        = fPz_in        = 0.;
  fPx_out      = fPy_out       = fPz_out       = 0.;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndFtsPoint::PndFtsPoint(Int_t trackID, Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, 
			 TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, 
			 TVector3 momIn, TVector3 momOut,
			 Double_t tof, Double_t length, Double_t eLoss, Double_t mass) 
  : FairMCPoint(trackID, detID, pos, momIn, tof, length, eLoss) 
{
  fX_out_local  = posOutLocal.X();
  fY_out_local  = posOutLocal.Y();
  fZ_out_local  = posOutLocal.Z();

  fX_in_local  = posInLocal.X();
  fY_in_local  = posInLocal.Y();
  fZ_in_local  = posInLocal.Z();

  fPx_in = momIn.Px();
  fPy_in = momIn.Py();
  fPz_in = momIn.Pz();

  fPx_out = momOut.Px();
  fPy_out = momOut.Py();
  fPz_out = momOut.Pz();

  // reset MC monentum
  fPx = (momIn.Px() + momOut.Px()) / 2.;
  fPy = (momIn.Py() + momOut.Py()) / 2.;
  fPz = (momIn.Pz() + momOut.Pz()) / 2.;

  fMass = mass; // stt1
  SetLink(FairLink("MCTrack", trackID));

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndFtsPoint::~PndFtsPoint() { }
// -------------------------------------------------------------------------




// -----   Public method Print   -------------------------------------------
void PndFtsPoint::Print(const Option_t* opt) const {
  cout << "-I- PndFtsPoint: FTS Point for track " << fTrackID 
       << " in detector " << fDetectorID << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength 
       << " cm,  Energy loss " << fELoss*1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndFtsPoint)
