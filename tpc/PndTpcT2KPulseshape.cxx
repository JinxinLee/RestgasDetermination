//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcT2KPulseshape
//      see PndTpcT2KPulseshape.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcT2KPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------


// Class Member definitions -----------

PndTpcT2KPulseshape::PndTpcT2KPulseshape(int Tp)
  : fTp(Tp)    
{
  assert(fTp==100 ||fTp == 200 || fTp==400 || fTp==1000 );//must have one of these 4 values
}


double 
PndTpcT2KPulseshape::operator() (const double t, 
	    const double amp, const double toffset) const {

  if(fTp==100)
    return solution_100(t,amp,toffset);
  if(fTp==200)
    return solution_200(t,amp,toffset);
  if(fTp==400)
    return solution_400(t,amp,toffset);
  if(fTp==1000)
    return solution_1000(t,amp,toffset);
  else
    return -1.0;
}

//TODO: calculate the response of the system to a exponantial function and not a step function

double 
PndTpcT2KPulseshape::solution_100(const double t, 
				    const double amp, 
				    const double toffset) const 
{
  double dt=t-toffset;
  double A0=0.835065;
  double to=2.05*20;
  if(dt<0) return 0;                           // piecewise definition!
  return amp*A0*pow(dt/to,3)*sin(dt/(3.0*to))*exp(-dt/to);
}

double 
PndTpcT2KPulseshape::solution_200(const double t, 
				    const double amp, 
				    const double toffset) const 
{
  double dt=t-toffset;
  double A0=0.91629;
  double to=3.55*20;
  if(dt<0) return 0;                           // piecewise definition!
  return amp*A0*pow(dt/to,3)*sin(dt/(3.5*to))*exp(-dt/to);
}

double 
PndTpcT2KPulseshape::solution_400(const double t, 
				  const double amp, 
				  const double toffset) const 
{
  double dt=t-toffset;
  double A0=0.95396;
  double to=7.55*20;
  if(dt<0) return 0;                           // piecewise definition!
  return amp*A0*pow(dt/to,3)*sin(dt/(3.7*to))*exp(-dt/to);
}

double 
PndTpcT2KPulseshape::solution_1000(const double t, 
				    const double amp, 
				    const double toffset) const 
{
  double dt=t-toffset;
  double A0=0.984186;
  double to=18.2*20;
  if(dt<0) return 0;                           // piecewise definition!
  return amp*A0*pow(dt/to,3)*sin(dt/(3.85*to))*exp(-dt/to);
}

