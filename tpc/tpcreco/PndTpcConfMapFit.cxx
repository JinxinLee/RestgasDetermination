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
  : AbsTrackRep(2),_isRotated(false)
{
  cov[0][0]=10;
  cov[1][1]=10;
}

AbsTrackRep*
PndTpcConfMapFit::clone()const
{
  return new PndTpcConfMapFit(*this);
}

AbsTrackRep* 
PndTpcConfMapFit::prototype()const
{
  return new PndTpcConfMapFit();
}


double 
PndTpcConfMapFit::extrapolate(const DetPlane& plane, TMatrixT<double>& statePred)
{
  statePred=state;
  return 0;
}

double
PndTpcConfMapFit::extrapolate(const DetPlane& plane,
			      TMatrixT<double>& statePred,
			      TMatrixT<double>& covPred)

{
  statePred=state;
  covPred=cov;
  return 0;
}

void 
PndTpcConfMapFit::stepalong(double h){
  throw;
}


TVector3 
PndTpcConfMapFit::getPos(const DetPlane& pl)
{
  throw;
}

TVector3 
PndTpcConfMapFit::getMom(const DetPlane& pl)
{
  throw;
}
  
void 
PndTpcConfMapFit::setReferencePlane(const DetPlane& pl)
{
  //_refPlane=pl;
}

double 
PndTpcConfMapFit::getR2()
{
  // (a,b) is center of circle in real space
  
  double b;
  if(state[1][0]!=0){
    b=0.5/state[1][0];
  }
  else b=1E4;
  double a=-state[0][0]*b;
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
