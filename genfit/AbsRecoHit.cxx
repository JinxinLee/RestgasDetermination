#include "AbsRecoHit.h"

ClassImp(AbsRecoHit)


AbsRecoHit::AbsRecoHit(int NparHit) : _HMatrix(1,1),
  _hitCoord(NparHit,1),
  _hitCov(NparHit,NparHit),
  _time(0.),
  _hasTime(false)
{
}

AbsRecoHit::AbsRecoHit() {
}

AbsRecoHit::~AbsRecoHit(){}

