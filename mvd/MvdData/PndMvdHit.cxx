// -------------------------------------------------------------------------
// -----                    PndMvdHit source file                -----
// -----                                 -----
// -------------------------------------------------------------------------


#include <iostream>
#include "PndMvdHit.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"


// -----   Default constructor   -------------------------------------------
PndMvdHit::PndMvdHit() {
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMvdHit::PndMvdHit(Int_t detID, TString detName, TVector3& pos, TVector3& dpos,
		     Int_t index, Double_t charge, Int_t NDigiHits) 
  : CbmHit(detID, pos, dpos, index) {
  fDetName = detName;
  fCharge  = charge;
  fNDigiHits = NDigiHits;
  fBotIndex = -1;
}

/*PndMvdHit::PndMvdHit(PndMvdHit& c) 
: CbmHit()
{
	fDetName = c.GetDetName();
	fTrackID = c.GetTrackID();
	fCharge  =  c.GetCharge();
	fNDigiHits = c.GetNDigiHits();
	SetPosition(c.GetPosition());
	SetPositionError(c.GetPositionError());
	SetDetectorID(c.GetDetectorID());
	SetRefIndex(c.GetRefIndex());
	
	
}*/
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMvdHit::~PndMvdHit() {}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void PndMvdHit::Print(const Option_t* opt) const {
  std::cout << *this;
}


Double_t PndMvdHit::GetD(Int_t i) 
{
  if(!gGeoManager){
    std::cout<<" -E- in PndMvdHit::GetD(): No gGeoManager there. Please use "<<std::endl;
    abort();
  }
  // TODO: Caution! if there is a point reconstructed OuTSIDE its volume, 
  // then the returned stuff is WRONG!
  ((TGeoNode*)(gGeoManager->FindNode(fX,fY,fZ)))->cd();
  Double_t local[3]={fDx,fDy,fDz};
  Double_t master[3];
  gGeoManager->LocalToMasterVect(local,master);
  return master[i];
}

// -------------------------------------------------------------------------
ClassImp(PndMvdHit);
