// -------------------------------------------------------------------------
// -----                    PndTofHit source file                -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndTofHit.h"


// -----   Default constructor   -------------------------------------------
PndTofHit::PndTofHit() {
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndTofHit::PndTofHit(Int_t trackId, Int_t detID, TString detName,  Double_t time, Double_t dt,
		     TVector3& pos,TVector3& dpos,Int_t index, Double_t charge) 
: FairHit(detID, pos, dpos, index) {
  fDetName = detName;
  fTrackID = trackId;
  fCharge  = charge;
  ftime  = time;
  fdt  = dt;
  //fNPixelHits = NPixelHits;
}

/*PndTofHit::PndTofHit(PndTofHit& c) 
: FairHit()
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
PndTofHit::~PndTofHit() {}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndTofHit::Print(const Option_t* opt) const {
  std::cout << "Hybrid pixel hit in detector " << fDetName << " at ("
       << fX << ", " << fY << ", " << fZ << ") cm " 
       << ", Point " << fRefIndex << std::endl;
}
// -------------------------------------------------------------------------
ClassImp(PndTofHit)
