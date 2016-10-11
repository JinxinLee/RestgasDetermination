// -------------------------------------------------------------------------
// -----                    FtofHit source file                -----
// -----                  Created by A. Sanchez               -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndFtofHit.h"


// -----   Default constructor   -------------------------------------------
PndFtofHit::PndFtofHit() {
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndFtofHit::PndFtofHit(Int_t trackId, Int_t detID, TString detName,  Double_t time, Double_t dt,
		     TVector3& pos,TVector3& dpos,Int_t index, Double_t charge) 
: FairHit(detID, pos, dpos, index) {
  fDetName = detName;
  fTrackID = trackId;
  fCharge  = charge;
  ftime  = time;
  fdt  = dt;

  SetTimeStamp(time);
  //fNPixelHits = NPixelHits;
}


// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndFtofHit::~PndFtofHit() {}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndFtofHit::Print(const Option_t* opt) const {
  std::cout << "Forward tof hit in detector " << fDetName << " at ("
       << fX << ", " << fY << ", " << fZ << ") cm " 
       << ", Point " << fRefIndex << std::endl;
}
// -------------------------------------------------------------------------
ClassImp(PndFtofHit)
