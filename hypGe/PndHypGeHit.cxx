/////////////////////////////////////////////////////////////
//
//  PndHypGeHit
//
//  HypGe digitized hit
//

//
/////////////////////////////////////////////////////////////// 

#include "PndHypGeHit.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndHypGeHit::PndHypGeHit() {}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndHypGeHit::PndHypGeHit(Int_t trackId, Int_t detID, 
			  Double_t ener, Double_t den){
  fTrackId = trackId;   fDetectorId = detID;
  fenergy= ener;
  fden=den; }
// -------------------------------------------------------------------------

PndHypGeHit::~PndHypGeHit() {}
// -----   Public method Print   -------------------------------------------
void PndHypGeHit::Print(const Option_t* opt) const {
  cout << "HYPGE hit: , Energy=" << fenergy;
  if (fTrackId>0) cout << ", TrackID= " << fTrackId;
//  cout << ", x=" << GetX() << ", y=" << GetY() << endl << flush; 
}
// -------------------------------------------------------------------------

ClassImp(PndHypGeHit)
