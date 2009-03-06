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

#ifndef VIRTSPACEPOINTRECOHIT_H
#define VIRTSPACEPOINTRECOHIT_H

#include "RecoHitIfc.h"
#include "SpacepointHitPolicy.h"

typedef RecoHitIfc<SpacepointHitPolicy> SpacepointRecoHit;

class VirtSpacePointRecoHit : public SpacepointRecoHit {
public:
 
  // Constructors/Destructors ---------
  VirtSpacePointRecoHit();
  VirtSpacePointRecoHit(double x, double y, double z);
  VirtSpacePointRecoHit(const TVector3& pos);

  virtual ~VirtSpacePointRecoHit();

  virtual AbsRecoHit* clone();

  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);




private:

  // Private Data Members ------------
  static const int NparHitRep = 3;

  // Private Methods -----------------

public:
  ClassDef(VirtSpacePointRecoHit,1)


};

#endif

