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
  : _adcThreshold(AdcThreshold),
    _adcmax(adcmax),
    _adcbits(adcbits),
    _t0(t0),_timebits(timebits),
    _psaThreshold(PSAthreshold),
    _tint(tint),_tdiff(tdiff),_tsig(tsig)
{
  _dt=1/SamplingFreq_Mhz * 1000.; // conversion to ns;
  assert(_adcbits<=32);
  assert(_timebits<=32);
  _maxsamples=(unsigned int)pow(2.,_timebits);
  _maxcounts=(unsigned int)pow(2.,_adcbits);
  _adcstep=_adcmax/_maxcounts;
}

unsigned int
PndTpcFrontend::Clock(double const t) const {
  assert(t>=_t0);
  double trel=t-_t0;
  unsigned int c=((unsigned int)floor(trel/_dt))%_maxsamples;
  return c;
}

double
PndTpcFrontend::ClockFine(double const t) const {
  assert(t>=_t0);
  double trel=t-_t0;
  return trel/_dt;
}

double
PndTpcFrontend::TimeAtClock(double const t) const {
  unsigned int c=Clock(t);
  return Clock2Time(c);
}


double 
PndTpcFrontend::Clock2Time(unsigned int const clock) const {
  double tnew=_dt*(double)clock+_t0;
  return tnew;
}

unsigned int
PndTpcFrontend::A2D(double const amp) const {
  if(amp<0)return 0;
  unsigned int d=(unsigned int)floor(amp/_adcstep);
  if(d>_maxcounts)d=_maxcounts;
  return d;
}


PndTpcFrontend::~PndTpcFrontend()
{}

