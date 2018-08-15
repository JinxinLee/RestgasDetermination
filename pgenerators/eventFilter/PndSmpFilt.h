/**
 @class PndSmpFilt

 @author Klaus Goetzen <k [dot] goetzen (at) gsi [dot] de>

 @brief Simple container for filter definition (criteria) for PndFilteredPrimaryGenerator

 */
#ifndef PndSmpFilt_H
#define PndSmpFilt_H

#include "TString.h"
#include <vector>

using std::vector;

// --------------------------------------------------------------------

class PndSmpFilt {
public:
	PndSmpFilt() : name(""), compo(false), veto(false), nmin(1), nmax(10000), pmin(0.0), pmax(1e8), ptmin(0.0), ptmax(1e8), 
				pzmin(-1e8), pzmax(1e8), thtmin(0.0), thtmax(180.0), phimin(-1e8), phimax(1e8), ndau(0), mcntr(0.0), mwin(0.0), nocc(false) 
	            { pdg[0] = pdg[1] = pdg[2] = pdg[3] = pdg[4] = 0; }
	            
	void Print();
	
	TString name;

	bool   compo;            // composites
	bool   veto;             // negate filter 
	
	int    pdg[5];           // pdg code(s of daughters for composites)
	int    nmin,   nmax;     // range particle mulitplicity
	double pmin,   pmax;     // range momentum p
	double ptmin,  ptmax;    // range transverse momentum pt
	double pzmin,  pzmax;    // range longitudinal momentum pz
	double thtmin, thtmax;   // range polar angle theta
	double phimin, phimax;   // range angle phi
	
	int    ndau;             // for composites: number of daughters
	double mcntr,  mwin;     // range angle phi
	bool   nocc;             // include charged conjugates? true = no
};


// --------------------------------------------------------------------

#endif

