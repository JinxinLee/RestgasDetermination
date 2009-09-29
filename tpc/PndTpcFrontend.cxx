//-----------------------------------------------------------
// File and Version Information:
// $Id: PndTpcFrontend.cc,v 1.7 2006/06/30 12:09:10 steinke Exp $
//
// Description:
//      Implementation of class PndTpcFrontend
//      see PndTpcFrontend.hh for details
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
#include "PndTpcFrontend.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------


// Class Member definitions -----------
PndTpcFrontend::PndTpcFrontend(const double tint,
			 const double tdiff,
			 const double tsig,
			 const double AdcThreshold,
			 const double adcmax,
			 const int adcbits,
			 const double SamplingFreq_Mhz,
			 const double t0,
			 const int timebits,
			 const unsigned int PSAthreshold)
  : fadcThreshold(AdcThreshold),
    fadcmax(adcmax),
    fadcbits(adcbits),
    ft0(t0),ftimebits(timebits),
    fpsaThreshold(PSAthreshold),
    ftint(tint),ftdiff(tdiff),ftsig(tsig)
{
  fdt=1/SamplingFreq_Mhz * 1000.; // conversion to ns;
  assert(fadcbits<=32);
  assert(ftimebits<=32);
  fmaxsamples=(unsigned int)pow(2.,ftimebits);
  fmaxcounts=(unsigned int)pow(2.,fadcbits);
  fadcstep=fadcmax/fmaxcounts;
}

unsigned int
PndTpcFrontend::Clock(double const t) const {
  assert(t>=ft0);
  double trel=t-ft0;
  unsigned int c=((unsigned int)floor(trel/fdt))%fmaxsamples;
  return c;
}

double
PndTpcFrontend::ClockFine(double const t) const {
  assert(t>=ft0);
  double trel=t-ft0;
  return trel/fdt;
}

double
PndTpcFrontend::TimeAtClock(double const t) const {
  unsigned int c=Clock(t);
  return Clock2Time(c);
}


double 
PndTpcFrontend::Clock2Time(unsigned int const clock) const {
  double tnew=fdt*(double)clock+ft0;
  return tnew;
}

unsigned int
PndTpcFrontend::A2D(double const amp) const {
  if(amp<0)return 0;
  unsigned int d=(unsigned int)floor(amp/fadcstep);
  if(d>fmaxcounts)d=fmaxcounts;
  return d;
}


PndTpcFrontend::~PndTpcFrontend()
{}

