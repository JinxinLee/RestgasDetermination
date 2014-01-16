//-----------------------------------------------------------
// Description:
//      Experimentally derived Pulseshape of the form:
//	f(x) = A * (t')^ N * exp(-N t')
//-----------------------------------------------------------
#pragma once
#ifndef PNDEMCFITTEDPULSESHAPE_H
#define PNDEMCFITTEDPULSESHAPE_H

#include "PndEmcAbsPulseshape.h"

class PndEmcFittedPulseshape  : public PndEmcAbsPulseshape {

	public:

	  PndEmcFittedPulseshape();
	  PndEmcFittedPulseshape(double tau, double N);
	  virtual ~PndEmcFittedPulseshape(){};

	  virtual double operator() (const double t, const double amp, const double toffset) const;

	private:

	  double fTau;
	  double fN;

	ClassDef(PndEmcFittedPulseshape,1)
};

#endif

