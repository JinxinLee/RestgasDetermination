// -------------------------------------------------------------------------
// -----                    PndHypHit source file                -----
// -----                Modified for hyp purpose by A. Sanchez   -----
// -------------------------------------------------------------------------

#include "stdlib.h"
#include <iostream>
#include <algorithm>
#include "PndHypHit.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"


// -----   Default constructor   -------------------------------------------
PndHypHit::PndHypHit() :
   fDetName(""),
   fCharge(0.),
   fNDigiHits(0),
   fBotIndex(-1)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndHypHit::PndHypHit(Int_t detID, TString detName, TVector3& pos, TVector3& dpos,Int_t index, Double_t charge, Int_t NDigiHits) 
  : FairHit(detID, pos, dpos, index), 
  fDetName(detName),
  fCharge(charge),
  fNDigiHits(NDigiHits),
  fBotIndex(-1)
{}


// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndHypHit::~PndHypHit() {}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndHypHit::Print(const Option_t* opt) const {
  std::cout << *this;
  
}




// -------------------------------------------------------------------------
ClassImp(PndHypHit)
