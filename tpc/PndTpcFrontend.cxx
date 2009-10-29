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
PndTpcFrontend::PndTpcFrontend(const double Tint,
			 const double Tdiff,
			 const double Tsig,
			 const double AdcThreshold,
			 const double Adcmax,
			 const int Adcbits,
			 const double SamplingFreq_Mhz,
			 const double T0,
			 const int timebits,
			 const unsigned int PSAthreshold)
  : fadcThreshold(AdcThreshold),
    fadcmax(Adcmax),
    fadcbits(Adcbits),
    ft0(T0),ftimebits(timebits),
    fpsaThreshold(PSAthreshold),
    ftint(Tint),ftdiff(Tdiff),ftsig(Tsig)
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

