#include "PndTpcRiemannMCResidual.h"
#include "McIdCollection.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"

ClassImp(PndTpcRiemannMCResidual)

Bool_t
PndTpcRiemannMCResidual::isTruth(){
  return hit()->cluster()->mcId().DominantID()==_mcIdTrack.DominantID();
}
