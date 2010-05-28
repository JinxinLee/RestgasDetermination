//-----------------------------------------------------------
// Description:
//      Abstract interface for EMC Pulse Shape Analyzer
//      Dima Melnychuk
//-----------------------------------------------------------

#ifndef EMCABSPSA_HH
#define EMCABSPSA_HH

// Base Class Headers ----------------
#include <vector>
#include "TObject.h"

class PndEmcWaveform;

class PndEmcAbsPSA {
public:

	// Constructors/Destructors ---------
	PndEmcAbsPSA(){}
	virtual ~PndEmcAbsPSA(){;}
	
	virtual void Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time) = 0;

};

#endif
