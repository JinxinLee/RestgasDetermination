#include "PndMdtHit.h"

// -----   Default constructor   -------------------------------------------
PndMdtHit::PndMdtHit() : CbmHit()
{
}


PndMdtHit::PndMdtHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index) 
  : CbmHit(detID, pos, dpos, index)
{
}

/** Destructor **/
PndMdtHit::~PndMdtHit() 
{
} 

ClassImp(PndMdtHit)
