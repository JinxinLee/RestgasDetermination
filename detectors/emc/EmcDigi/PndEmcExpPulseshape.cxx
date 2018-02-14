//-----------------------------------------------------------
// Description:
//      Exponential pulseshape
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndEmcExpPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------


// Class Member definitions -----------

PndEmcExpPulseshape::PndEmcExpPulseshape(double Tdif) 
  : fTdif(Tdif)
{
}

double 
PndEmcExpPulseshape::operator() (const double t, 
	    const double amp, const double toffset) const {
  return general_solution(t,amp,toffset);
}

double 
PndEmcExpPulseshape::general_solution(const double t, 
				    const double amp, 
				    const double toffset) const 
{
  double dt=t-toffset;
  if(dt<0) return 0;                           // piecewise definition!
  return amp*exp(-dt/fTdif);
}
