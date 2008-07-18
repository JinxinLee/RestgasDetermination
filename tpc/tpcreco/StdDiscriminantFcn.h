//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Discriminant function object
//      for a gaussian shaped, n-dimensional distribution
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef STDDISCRIMINANTFCN_HH
#define STDDISCRIMINANTFCN_HH

// Base Class Headers ----------------
#include "TMatrixT.h"
#include "TVectorT.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --



class StdDiscriminantFcn {
public:

  // Constructors/Destructors ---------
  StdDiscriminantFcn(const TVectorT<double>& mean, 
		     const TMatrixT<double>& cov);
  ~StdDiscriminantFcn(){}

  // Operators
  double operator() (const TVectorT<double>& x);


private:

  // Private Data Members ------------
  TVectorT<double> _mean;
  TMatrixT<double> _invcov;
  double _lndet;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
