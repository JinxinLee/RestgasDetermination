//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcZSFit
//      see PndTpcZSFit.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcZSFit.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------



PndTpcZSFit::PndTpcZSFit()
  : AbsTrackRep(2)
{
  cov[0][0]=10;
  cov[1][1]=10;
}

AbsTrackRep*
PndTpcZSFit::clone()const 
{
  return new PndTpcZSFit(*this);
}

AbsTrackRep* 
PndTpcZSFit::prototype()const
{
  return new PndTpcZSFit();
}


double
PndTpcZSFit::extrapolate(const DetPlane& plane, TMatrixT<double>& statePred)
{
  statePred=state;
  return 0;
}

double
PndTpcZSFit::extrapolate(const DetPlane& plane,
			 TMatrixT<double>& statePred,
			 TMatrixT<double>& covPred)
{
  statePred=state;
  covPred=cov;
  return 0;
}

TVector3 
PndTpcZSFit::getPos(const DetPlane& pl)
{
  throw;
}

TVector3 
PndTpcZSFit::getMom(const DetPlane& pl)
{
  throw;
}
  
void 
PndTpcZSFit::setReferencePlane(const DetPlane& pl)
{
  //_refPlane=pl;
}
