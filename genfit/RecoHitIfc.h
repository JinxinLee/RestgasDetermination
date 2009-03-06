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


#ifndef RECOHITIFC_HH
#define RECOHITIFC_HH

// Base Class Headers ----------------
#include "AbsRecoHit.h"

// Collaborating Class Headers -------
#include "DetPlane.h"
#include "TMatrixT.h"

// Collaborating Class Declarations --


template<class detplane_policy>
class RecoHitIfc : public AbsRecoHit, public detplane_policy{
 public:
  RecoHitIfc(int dim) : AbsRecoHit(dim){;}
  virtual ~RecoHitIfc(){;}

  virtual const DetPlane& getDetPlane(AbsTrackRep* rep){return detPlane(this,rep);}
  virtual TMatrixT<double> getHitCoord(const DetPlane& plane)
  {return hitCoord(this,plane);}
  virtual TMatrixT<double> getHitCov(const DetPlane& plane)
  {return hitCov(this,plane);}
};

#endif



//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
