/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

#include "VirtSpacePointRecoHit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "LSLTrackRep.h"

// Class Member definitions -----------

ClassImp(VirtSpacePointRecoHit)

VirtSpacePointRecoHit::~VirtSpacePointRecoHit()
{}

VirtSpacePointRecoHit::VirtSpacePointRecoHit()
: SpacepointRecoHit(NparHitRep)
{}

VirtSpacePointRecoHit::VirtSpacePointRecoHit(double x, double y, double z)
  : SpacepointRecoHit(NparHitRep)
{
  setPos(TVector3(x,y,z));
  _hitCoord[0][0] = x;
  _hitCoord[1][0] = y;
  _hitCoord[2][0] = z;
  _hitCov.UnitMatrix();
}

VirtSpacePointRecoHit::VirtSpacePointRecoHit(const TVector3& pos)
  : SpacepointRecoHit(NparHitRep)
{
 setPos(pos);
  _hitCoord[0][0] = pos.X();
  _hitCoord[1][0] = pos.Y();
  _hitCoord[2][0] = pos.Z();
  _hitCov.UnitMatrix();
}

AbsRecoHit*
VirtSpacePointRecoHit::clone()
{ 
  return new VirtSpacePointRecoHit(*this);
}


void 
VirtSpacePointRecoHit::setHMatrix(const AbsTrackRep* stateVector,
	   const TMatrixT<double>& state)
{
  if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    _HMatrix.ResizeTo(NparHitRep,5);

    _HMatrix[0][0] = 1.;
    _HMatrix[0][1] = 0.;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 0.;
    _HMatrix[0][4] = 0.;

    _HMatrix[1][0] = 0.;
    _HMatrix[1][1] = 1.;
    _HMatrix[1][2] = 0.;
    _HMatrix[1][3] = 0.;
    _HMatrix[1][4] = 0.;

    _HMatrix[2][0] = 0.;
    _HMatrix[2][1] = 0.;
    _HMatrix[2][2] = 0.;
    _HMatrix[2][3] = 0.;
    _HMatrix[2][4] = 0.;
  }
  else {
    std::cerr << "VirtSpacePointRecoHit can only handle state"
              << " vectors of type LSLTrackRep -> abort" << std::endl;
    throw;
  }

}


