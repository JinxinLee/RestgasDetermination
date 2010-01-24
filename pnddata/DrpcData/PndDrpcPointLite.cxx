/////////////////////////////////////////////////////////////
//
//  CbmHypPoint
//
//  Geant point for Hyp detector
//
//
///////////////////////////////////////////////////////////////

#include "PndDrpcPointLite.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrpcPointLite::PndDrpcPointLite()
  : TObject()
{
  fTrackId=-1111;
  fDetId=-1111;
  fPDGCode=-1111;
  fELoss=-1111;
  fX=-1111; fY=-1111; fZ=-1111; fT=-1111;
  fPx=-1111; fPy=-1111; fPz=-1111; fE=-1111;
  fLen=-1111;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrpcPointLite::PndDrpcPointLite(Int_t trackID,
	      Int_t detID,
	      TLorentzVector& pos,
	      TLorentzVector& mom,
	      Double_t eLoss,
	      Double_t len,
	      Int_t pdgCode)
 : TObject()
{
  fTrackId=trackID;
  fDetId=detID;
  fELoss=eLoss;
  fLen=len;
  fPDGCode=pdgCode;
  SetPos(pos);
  SetMom(mom);
}


// -----   Destructor   ----------------------------------------------------
PndDrpcPointLite::~PndDrpcPointLite() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndDrpcPointLite::Print(const Option_t* opt) const {
  cout << "-I- PndDrpcPointLite: MUO Point for track " << fTrackId 
       << " in detector " << fDetId << endl;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ
       << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
  cout << "    Time " << fT << " ns,  Length " << fLen
       << " cm,  Energy loss " << fELoss << " GeV" << endl;
}
// -------------------------------------------------------------------------



ClassImp(PndDrpcPointLite)
