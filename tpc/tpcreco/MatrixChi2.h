//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      a functor returning the chi2 of two vectors with two covs
//      chi2 = d^T C^{-1} d
//      d = x_1 - x_2
//      C = cov_1 + cov_2
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef MATRIXCHI2_HH
#define MATRIXCHI2_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "TMatrixT.h"


class MatrixChi2 {
public:

  // Constructors/Destructors ---------
  MatrixChi2(){};
  ~MatrixChi2(){};

  // Operations ----------------------
  double operator ()(const TMatrixT<double>& x1, const TMatrixT<double>& cov1,
		     const TMatrixT<double>& x2, const TMatrixT<double>& cov2);
  

private:

  // Private Data Members ------------


  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
