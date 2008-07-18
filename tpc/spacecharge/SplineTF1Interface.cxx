//-----------------------------------------------------------
// File and Version Information:
// Version 1.0  (Feb 27, 2008)
//
// Description:
//      Spline-Interface for TF1
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
#include "SplineTF1Interface.h"

// Collaborating Class Headers --------


SplineTF1Interface::SplineTF1Interface(CubSpline* csp) {
    _csp = csp;
    _knots = _csp->getKnots();
    _length = _knots->size();
    _coeffs = _csp->getCoeffs();
}
  

double
SplineTF1Interface::eval(double *x, double *p) {
  //have to obey TF1's interface and take double* 
  //user is responsible for proper length of parameter array!
  _csp->setCoeffsByArray(p);       // WATCH n_par of TF1 !!!
  double result = _csp->eval(x[0]);
  return result;
}


