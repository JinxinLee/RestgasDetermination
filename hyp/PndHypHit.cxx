// -------------------------------------------------------------------------
// -----                    PndHypHit source file                -----
// -----                  Created 07/04/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndHypHit.h"


// -----   Default constructor   -------------------------------------------
PndHypHit::PndHypHit() {
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndHypHit::PndHypHit(Int_t detID, TString detName, TVector3& pos, TVector3& dpos,
		     Int_t index, Double_t charge, Int_t NPixelHits) 
  : CbmHit(detID, pos, dpos, index) {
  fDetName = detName;
  //fTrackID = trackId;
  fCharge  = charge;
  fNPixelHits = NPixelHits;
}

/*PndHypHit::PndHypHit(PndHypHit& c) 
: CbmHit()
{
	fDetName = c.GetDetName();
	fTrackID = c.GetTrackID();
	fCharge  =  c.GetCharge();
	fNPixelHits = c.GetNPixelHits();
	SetPosition(c.GetPosition());
	SetPositionError(c.GetPositionError());
	SetDetectorID(c.GetDetectorID());
	SetRefIndex(c.GetRefIndex());
	
	
}*/
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndHypHit::~PndHypHit() {}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndHypHit::Print(const Option_t* opt) const {
  std::cout << *this;
  // std::cout << "Hybrid pixel hit in detector " << fDetName << " at ("
  //        << fX << ", " << fY << ", " << fZ << ") cm " 
  //        << ", Point " << fRefIndex << std::endl;
}
// -------------------------------------------------------------------------
ClassImp(PndHypHit)
