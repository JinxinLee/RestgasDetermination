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
  : GFAbsTrackRep(2)
{
  fCov[0][0]=10;
  fCov[1][1]=10;
}

GFAbsTrackRep*
PndTpcZSFit::clone()const 
{
  return new PndTpcZSFit(*this);
}

GFAbsTrackRep* 
PndTpcZSFit::prototype()const
{
  return new PndTpcZSFit();
}


double
PndTpcZSFit::extrapolate(const GFDetPlane& plane, TMatrixT<double>& statePred)
{
  statePred=fState;
  return 0;
}

double
PndTpcZSFit::extrapolate(const GFDetPlane& plane,
			 TMatrixT<double>& statePred,
			 TMatrixT<double>& covPred)
{
  statePred=fState;
  covPred=fCov;
  return 0;
}

TVector3 
PndTpcZSFit::getPos(const GFDetPlane& pl)
{
  throw;
}

TVector3 
PndTpcZSFit::getMom(const GFDetPlane& pl)
{
  throw;
}
  
void 
PndTpcZSFit::setReferencePlane(const GFDetPlane& pl)
{
  //fRefPlane=pl;
}
