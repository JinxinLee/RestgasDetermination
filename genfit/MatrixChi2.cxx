//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class MatrixChi2
//      see MatrixChi2.hh for details
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
#include "MatrixChi2.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------

double
MatrixChi2::operator()(const TMatrixT<double>& x1, 
		       const TMatrixT<double>& cov1,
		       const TMatrixT<double>& x2,
		       const TMatrixT<double>& cov2)
{
  TMatrixT<double> d=x1-x2;
  TMatrixT<double> C=cov1+cov2;
  double det=0;
  C.Invert(&det);
  TMatrixT<double> chisq=d.T()*(C*d);
  return chisq[0][0];
}
