//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndEmcCRRCPulseshape
//      see PndEmcCRRCPulseshape.hh for details
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//		
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndEmcCRRCPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------


// Class Member definitions -----------

PndEmcCRRCPulseshape::PndEmcCRRCPulseshape(double Tint, double Tdif, double Tsig) 
  : _Tint(Tint), _Tdif(Tdif), _Tsig(Tsig)    
{
  assert(_Tint!=_Tsig && _Tdif!=_Tsig);
}

double 
PndEmcCRRCPulseshape::operator() (const double t, 
	    const double amp, const double toffset) const {
  if(_Tint==_Tdif)return degenerate_solution(t,amp,toffset);
  return general_solution(t,amp,toffset);
}


double 
PndEmcCRRCPulseshape::general_solution(const double t, 
				    const double amp, 
				    const double toffset) const 
{
  double dt=t-toffset;
  if(dt<0) return 0;                           // piecewise definition!
  double term1=exp(-dt/_Tdif)*_Tdif/(_Tdif-_Tint)/(_Tdif-_Tsig);
  double term2=exp(-dt/_Tint)*_Tint/(-_Tdif+_Tint)/(_Tint-_Tsig);
  double term3=exp(-dt/_Tsig)*_Tsig/(-_Tdif+_Tsig)/(-_Tint+_Tsig);
  return amp*_Tdif*(term1+term2+term3);
}


double 
PndEmcCRRCPulseshape::degenerate_solution(const double t,  // for Tdif=Tint
				       const double amp, 
				       const double toffset) const 
{
  double dt=t-toffset;
  if(dt<0) return 0;                           // piecewise definition!
  double term1=exp(-dt/_Tsig)*_Tsig;
  double term2=exp(-dt/_Tint)*(_Tint*(dt-_Tsig)-dt*_Tsig)/_Tint;
  double term3=_Tint-_Tsig;
  term3*=term3;
  return amp*_Tint/term3*(term1+term2);
}
