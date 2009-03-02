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
