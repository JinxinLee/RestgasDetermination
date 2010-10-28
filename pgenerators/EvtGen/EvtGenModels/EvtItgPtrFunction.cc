//--------------------------------------------------------------------------
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Module: EvtItgPtrFunction.hh
//
// Description:
//      Class describing a function with one vector of coefficients. (Stolen and 
//      modified from the BaBar IntegrationUtils package - author: Phil Strother).
//
// Modification history:
//
//    Jane Tinslay                March 21, 2001       Module adapted for use in 
//                                                     EvtGen
//
//------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"

#include "EvtGenModels/EvtItgPtrFunction.hh"

//-------------
// C Headers --
//-------------
extern "C" {
}

//----------------
// Constructors --
//----------------
EvtItgPtrFunction::EvtItgPtrFunction( double (*theFunction)(double, const std::vector<double> &), double flowerRange, double fupperRange, const std::vector<double> &coeffs1):
  EvtItgAbsFunction(flowerRange, fupperRange),
  _myFunction(theFunction),
  _coeffs1(coeffs1)
{}


//--------------
// Destructor --
//--------------

EvtItgPtrFunction::~EvtItgPtrFunction( )
{}


double
EvtItgPtrFunction::myFunction(double x) const{
  return _myFunction(x, _coeffs1);
}

void
EvtItgPtrFunction::setCoeff(int vect, int which, double fvalue)
{
  if (vect == 1) _coeffs1[which] = fvalue;
}

double
EvtItgPtrFunction::getCoeff(int vect, int which)
{
  if (vect == 1) return _coeffs1[which];
  else {return 0;}
}
