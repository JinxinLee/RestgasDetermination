#include "PlanarHitPolicy.h"

#include "AbsRecoHit.h"

TMatrixT<double> 
PlanarHitPolicy::hitCoord(AbsRecoHit* hit,const DetPlane&)
{
  return hit->getRawHitCoord();
}

TMatrixT<double> 
PlanarHitPolicy::hitCov(AbsRecoHit* hit,const DetPlane&)
{
  return hit->getRawHitCov();
}

const DetPlane& 
PlanarHitPolicy::detPlane(AbsRecoHit*,const AbsTrackRep*)
{
  return _physicalDetPlane;
}
