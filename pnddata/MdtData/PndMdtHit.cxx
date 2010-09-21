#include "PndMdtHit.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMdtHit::PndMdtHit() : FairHit()
{
}


PndMdtHit::PndMdtHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index) 
  : FairHit(detID, pos, dpos, index)
{
	//SetLink(FairLink("MdtPoint", index));
}

/** Destructor **/
PndMdtHit::~PndMdtHit() 
{
} 

ClassImp(PndMdtHit)
