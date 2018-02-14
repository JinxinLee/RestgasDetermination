#include "PndEmcFittedPulseshape.h"
#include "math.h"


// Class Member definitions -----------
PndEmcFittedPulseshape::PndEmcFittedPulseshape() : fTau(0), fN(0) {
}

PndEmcFittedPulseshape::PndEmcFittedPulseshape(double tau, double N) : fTau(tau), fN(N) {
}

double PndEmcFittedPulseshape::operator() (const double t, const double amp, const double toffset) const {
	if(t<toffset) {
		return 0.0;
	} 

	double t_norm = (t-toffset) / fTau;
	return amp * pow(t_norm, fN) * exp(-fN * t_norm);
}

ClassImp(PndEmcFittedPulseshape);
