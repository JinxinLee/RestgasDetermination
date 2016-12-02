//--------------------------------------------------------------------------
//
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Module: EvtItgTwoCoeffFcn.cc
//
// Description:
//      Class describing a function with two vectors of coefficients. 
//
// Modification history:
//
//    Jane Tinslay                March 21, 2001       Module created
//
//------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"

#include "EvtGenModels/EvtItgTwoCoeffFcn.hh"

//-------------
// C Headers --
//-------------
extern "C" {
}

EvtItgTwoCoeffFcn::EvtItgTwoCoeffFcn( double (*theFunction)(double, const std::vector<double> &, const std::vector<double> &), double the_lowerRange, double the_upperRange, const std::vector<double> &coeffs1, const std::vector<double> &coeffs2):
  EvtItgAbsFunction(the_lowerRange, the_upperRange),
  _myFunction(theFunction),
  _coeffs1(coeffs1),
  _coeffs2(coeffs2)
{}

EvtItgTwoCoeffFcn::~EvtItgTwoCoeffFcn( )
{}

double
EvtItgTwoCoeffFcn::myFunction(double x) const{
  return _myFunction(x, _coeffs1, _coeffs2);
}

void
EvtItgTwoCoeffFcn::setCoeff(int vect, int which, double the_value)
{
  if (vect == 1) _coeffs1[which] = the_value;
  else if (vect == 2) _coeffs2[which] = the_value;
}

double
EvtItgTwoCoeffFcn::getCoeff(int vect, int which)
{
  if (vect == 1) return _coeffs1[which];
  else if (vect == 2) return _coeffs2[which];
  else {return 0;}
}

