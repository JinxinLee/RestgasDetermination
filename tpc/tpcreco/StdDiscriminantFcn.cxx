//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class StdDiscriminantFcn
//      see StdDiscriminantFcn.hh for details
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
#include "StdDiscriminantFcn.h"
#include "TMath.h"
// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------


StdDiscriminantFcn::StdDiscriminantFcn(const TVectorT<double>& mean, 
		   const TMatrixT<double>& cov)
  : _mean(mean),_invcov(cov)
{
  double det;
  _invcov.Invert(&det);
  _lndet=TMath::Log(det);
}


double
StdDiscriminantFcn::operator()(const TVectorT<double>& x)
{
  TVectorT<double> res(x-_mean);
  double term1=-0.5*res*(_invcov*res);
  double term2=-0.5*_lndet;
  return term1+term2;
}
