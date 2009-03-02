// Original Authors: Christian Hoeppner & Sebastian Neubert (TUM)
#include "AbsRecoHit.h"

ClassImp(AbsRecoHit)


AbsRecoHit::AbsRecoHit(int NparHit) : _HMatrix(1,1),
				      _hitCoord(NparHit,1),
				      _hitCov(NparHit,NparHit) {
}

AbsRecoHit::AbsRecoHit() {
}

AbsRecoHit::~AbsRecoHit(){}

