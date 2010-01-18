// -------------------------------------------------------------------------
// -----                    PndMvdHit source file                -----
// -----                                 -----
// -------------------------------------------------------------------------

#include "stdlib.h"
#include <iostream>
#include "PndMvdHit.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"


// -----   Default constructor   -------------------------------------------
PndMvdHit::PndMvdHit() {
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndMvdHit::PndMvdHit(Int_t detID, TString detName, TVector3& pos, TVector3& dpos,
		     Int_t clindex, Double_t charge, Int_t NDigiHits, Int_t mcindex)
  : FairHit(detID, pos, dpos, mcindex) {
  fDetName = detName;
  fCharge  = charge;
  fNDigiHits = NDigiHits;
  fClusterIndex = clindex;
  fBotIndex = -1;
}

/*PndMvdHit::PndMvdHit(PndMvdHit& c)
: FairHit()
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


// Double_t PndMvdHit::GetD(Int_t i)
// {
//   if(!gGeoManager){
//     std::cout<<" -E- in PndMvdHit::GetD(): No gGeoManager there. Please use "<<std::endl;
//     abort();
//   }
//
//   // Caution! if there is a point reconstructed OUTSIDE its volume,
//   // then the returned stuff is WRONG!
//   ((TGeoNode*)(gGeoManager->FindNode(fX,fY,fZ)))->cd();
//   TGeoHMatrix* currMatrix = gGeoManager->GetCurrentMatrix();
//
//   const Double_t* rotation = (currMatrix->Inverse()).GetRotationMatrix();
//   Double_t local[3]={fDx,fDy,fDz};
//
//   Double_t result = fabs(rotation[3*i+0]*local[0]) + fabs(rotation[3*i+1]*local[1]) + fabs(rotation[3*i+2]*local[2]);
// std::cout"PndMvdHit::GetD("<<i<<"): dLocal={"<<fDx<<","<<fDy<<","<<fDz<<"} dLab("<<i<<"="<<result<<std::endl;
// return result;
// //   Double_t master[3];
// //   gGeoManager->LocalToMasterVect(local,master);
// //   return master[i];
//
// }

// -------------------------------------------------------------------------
ClassImp(PndMvdHit);
