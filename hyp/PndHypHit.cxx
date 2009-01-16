// -------------------------------------------------------------------------
// -----                    PndHypHit source file                -----
// -----                  Created 07/04/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndHypHit.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"


// -----   Default constructor   -------------------------------------------
PndHypHit::PndHypHit() {
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndHypHit::PndHypHit(Int_t detID, TString detName, TVector3& pos, TVector3& dpos,Int_t index, Double_t charge, Int_t NPixelHits) 
  : CbmHit(detID, pos, dpos, index) {
  fDetName = detName;
  //fTrackID = trackId;
  fCharge  = charge;
  fNPixelHits = NPixelHits;
  fBotIndex = -1;
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
  
}

Double_t PndHypHit::GetD(Int_t i)
{
  if(!gGeoManager){
    std::cout<<" -E- in PndHypHit::GetDx(): No gGeoManager there. Please use "<<std::endl;
    abort();
  }
  // TODO: Caution! if there is a point reconstructed OuTSIDE its volume,
  // then the returned stuff is WROMG!
  ((TGeoNode*)(gGeoManager->FindNode(fX,fY,fZ)))->cd();
  Double_t local[3]={fDx,fDy,fDz};
  Double_t master[3];
  gGeoManager->LocalToMasterVect(local,master);
  return master[i];
}


// -------------------------------------------------------------------------
ClassImp(PndHypHit)
