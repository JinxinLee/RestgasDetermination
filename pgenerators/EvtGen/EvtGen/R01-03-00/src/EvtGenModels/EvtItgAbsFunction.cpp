//--------------------------------------------------------------------------
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Module: EvtItgAbsFunction.hh
//
// Description:
//      Abstraction of a generic function for use in integration methods elsewhere
//      in this package. (Stolen and modified from the BaBar IntegrationUtils package 
//      - author: Phil Strother).
//
// Modification history:
//
//    Jane Tinslay                March 21, 2001       Module adapted for use in 
//                                                     EvtGen
//
//------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"

#include "EvtGenModels/EvtItgAbsFunction.hh"

//-------------
// C Headers --
//-------------
extern "C" {
}
#include "assert.h"
#include "EvtGenBase/EvtReport.hh"
using std::endl;

EvtItgAbsFunction::EvtItgAbsFunction(double the_lowerRange, double the_upperRange):
  _upperRange(the_upperRange),
  _lowerRange(the_lowerRange){}

EvtItgAbsFunction::~EvtItgAbsFunction( )
{}


double
EvtItgAbsFunction::value( double x) const{
  if (x >= _lowerRange && x <= _upperRange) return myFunction(x);
   report(EVT_ERROR,"EvtGen") << "Error in EvtItgAbsFunction::value.  Given co-ordinate " << x
                << " is outside of allowed range [" << _lowerRange << ", "
                << _upperRange << "].  Returning 0.0" << endl;
  return 0.0;  // Never get here
}
   
double 
EvtItgAbsFunction::operator()(double x) const{
  return myFunction(x);
}
