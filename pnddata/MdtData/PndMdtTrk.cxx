#include "PndMdtTrk.h"

// -----   Default constructor   -------------------------------------------
PndMdtTrk::PndMdtTrk()
{
  Clear();
}

/** Destructor **/
PndMdtTrk::~PndMdtTrk() 
{
}

// -----   Clear function   -------------------------------------------
void PndMdtTrk::Clear()
{
  for (Int_t ii=0; ii<15; ii++)
    { fHitList[ii]  = -1;
      fHitMult[ii]  =  0;
      fHitDist[ii] = -1.;
      fLayDist[ii] = 0.;
    }
  fHitCount = 0;
  fLayerCount = 0;
  fMaxLayer = 0;
  fHitBit = 0;
  fModule = 0;
  fChi2 = -1;
}

ClassImp(PndMdtTrk)
