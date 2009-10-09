//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcConfMapFit
//      see PndTpcConfMapFit.hh for details
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
#include "PndTpcConfMapFit.h"
 #include "TMath.h"
// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------



PndTpcConfMapFit::PndTpcConfMapFit()
  : GFAbsTrackRep(2),_isRotated(false)
{
  fCov[0][0]=10;
  fCov[1][1]=10;
}

GFAbsTrackRep*
PndTpcConfMapFit::clone()const
{
  return new PndTpcConfMapFit(*this);
}

GFAbsTrackRep* 
PndTpcConfMapFit::prototype()const
{
  return new PndTpcConfMapFit();
}


double 
PndTpcConfMapFit::extrapolate(const GFDetPlane& plane, TMatrixT<double>& statePred)
{
  statePred=fState;
  return 0;
}

double
PndTpcConfMapFit::extrapolate(const GFDetPlane& plane,
			      TMatrixT<double>& statePred,
			      TMatrixT<double>& covPred)

{
  statePred=fState;
  covPred=fCov;
  return 0;
}


TVector3 
PndTpcConfMapFit::getPos(const GFDetPlane& pl)
{
  throw;
}

TVector3 
PndTpcConfMapFit::getMom(const GFDetPlane& pl)
{
  throw;
}
  
void 
PndTpcConfMapFit::setReferencePlane(const GFDetPlane& pl)
{
  //fRefPlane=pl;
}

double 
PndTpcConfMapFit::getR2()
{
  // (a,b) is center of circle in real space
  
  double b;
  if(fState[1][0]!=0){
    b=0.5/fState[1][0];
  }
  else b=1E4;
  double a=-fState[0][0]*b;
  return a*a+b*b;
}

double 
PndTpcConfMapFit::getR()
{
  return TMath::Sqrt(getR2());
}

void
PndTpcConfMapFit::setRotated(bool flag)
{
  _isRotated=flag;
  double x=_origin.X();
  double y=_origin.Y();
  _origin.Set(y,-x);
}
