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


#ifndef PLANARHITPOLICY_H
#define PLANARHITPOLICY_H


#include "TMatrixT.h"

#include "DetPlane.h"

class AbsRecoHit;
class AbsTrackRep;


class PlanarHitPolicy {
public:

  // Constructors/Destructors ---------
  PlanarHitPolicy(){;}
  

  // Accessors -----------------------
  const DetPlane& detPlane(AbsRecoHit*,const AbsTrackRep*);
  

  // Modifiers -----------------------
  void setDetPlane(const DetPlane& p){_physicalDetPlane=p;}

  // Operations ----------------------
  TMatrixT<double> hitCoord(AbsRecoHit*,const DetPlane&);
  TMatrixT<double> hitCov(AbsRecoHit*,const DetPlane&);

protected:
  // policy destructors have to be protected
  // see Alexandrescu
  virtual ~PlanarHitPolicy(){;}


private:

  // Private Data Members ------------
  DetPlane _physicalDetPlane;

  // Private Methods -----------------

};

#endif

