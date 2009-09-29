//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcCRRCPulseshape
//      see PndTpcCRRCPulseshape.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcCRRCPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------


// Class Member definitions -----------

PndTpcCRRCPulseshape::PndTpcCRRCPulseshape(double Tint, double Tdif, double Tsig) 
  : fTint(Tint), fTdif(Tdif), fTsig(Tsig)    
{
  assert(fTint!=fTsig && fTdif!=fTsig);
}


double 
PndTpcCRRCPulseshape::operator() (const double t, 
	    const double amp, const double toffset) const {
  if(fTint==fTdif)return degenerate_solution(t,amp,toffset);
  return general_solution(t,amp,toffset);
}


double 
PndTpcCRRCPulseshape::general_solution(const double t, 
				    const double amp, 
				    const double toffset) const 
{
  double dt=t-toffset;
  if(dt<0) return 0;                           // piecewise definition!
  double term1=exp(-dt/fTdif)*fTdif/(fTdif-fTint)/(fTdif-fTsig);
  double term2=exp(-dt/fTint)*fTint/(-fTdif+fTint)/(fTint-fTsig);
  double term3=exp(-dt/fTsig)*fTsig/(-fTdif+fTsig)/(-fTint+fTsig);
  return amp*fTdif*(term1+term2+term3);
}


double 
PndTpcCRRCPulseshape::degenerate_solution(const double t,  // for Tdif=Tint
				       const double amp, 
				       const double toffset) const 
{
  double dt=t-toffset;
  if(dt<0) return 0;                           // piecewise definition!
  double term1=exp(-dt/fTsig)*fTsig;
  double term2=exp(-dt/fTint)*(fTint*(dt-fTsig)-dt*fTsig)/fTint;
  double term3=fTint-fTsig;
  term3*=term3;
  return amp*fTint/term3*(term1+term2);
}
