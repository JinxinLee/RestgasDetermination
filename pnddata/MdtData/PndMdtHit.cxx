#include "PndMdtHit.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMdtHit::PndMdtHit() : FairHit()
{
        fStripID = -1;
        fStripIndex = -1;
}


PndMdtHit::PndMdtHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index) 
  : FairHit(detID, pos, dpos, index)
{
	SetLink(FairLink("MdtPoint", index));
}

PndMdtHit::PndMdtHit(Int_t detID, Int_t stripID, TVector3& pos, TVector3& dpos, Int_t bIndex, Int_t sIndex)
  : FairHit(detID, pos, dpos, bIndex)
{
        fStripID = stripID;
        fStripIndex = sIndex;
        SetLink(FairLink("MdtDigiBox", bIndex));
        SetLink(FairLink("MdtDigiStrip", sIndex));
}


/** Destructor **/
PndMdtHit::~PndMdtHit() 
{
} 

// -----   Public method Print   -------------------------------------------
void PndMdtHit::Print(const Option_t* opt) const {
  std::cout << "Mdt hit in detector: Module " << GetModule() << " Sector " << GetSector() << " Layer " << GetLayerID() << " Box " << GetBox() << " Wire " << GetWire() << " Strip " << GetStrip() << " at ("
       << fX << ", " << fY << ", " << fZ << ") cm " 
       << ", Point " << fRefIndex << std::endl;
}

ClassImp(PndMdtHit)
