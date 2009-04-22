//-----------------------------------------------------------
// File and Version Information:
// Version 1.0  (Apr 7, 2008)
//
// Description:
//      Spline-Interface for TF2
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


// Panda Headers ----------------------

// This Class' Header ------------------
#include "SplineTF2Interface.h"

// Collaborating Class Headers --------


//TODO: _read_only should be true by default, but need to fix my macros first

SplineTF2Interface::SplineTF2Interface() {
  _bcsp = NULL;
  _kx = NULL;
  _ky = NULL;
  _xl = 0;
  _yl = 0;
  _coeffs = NULL;
  _read_only=false;
}


SplineTF2Interface::SplineTF2Interface(BiCubSpline* bcsp) {
  _bcsp = bcsp;
  _kx = _bcsp->getKx();
  _ky = _bcsp->getKy();
  _xl = _kx->size();
  _yl = _ky->size();
  _coeffs = _bcsp->getCoeffs();
  _read_only=false;
}


double   
SplineTF2Interface::eval(double *x, double *p) {
  //have to obey TF2's interface and take double* 
  //user is responsible for proper length of parameter array!
  if(!_read_only)
    _bcsp->setCoeffsByArray(p);
  double result = _bcsp->eval(x[0], x[1]);
  return result;
}


