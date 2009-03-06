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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PlanarHitPolicy
//      see PlanarHitPolicy.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
// Original Authors: Christian Hoeppner & Sebastian Neubert (TUM)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PlanarHitPolicy.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "AbsRecoHit.h"

// Class Member definitions -----------


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
