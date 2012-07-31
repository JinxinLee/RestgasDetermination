//-----------------------------------------------------------
// Description:
//      Pulseshape analysis for ADC waveforms
//
//-----------------------------------------------------------

#ifndef EMCPSATRAPDIGIFILTER_HH
#define EMCPSATRAPDIGIFILTER_HH

// Base Class Headers ----------------
#include <vector>
#include "PndEmcAbsPSA.h"
#include "TObject.h"		

class PndEmcWaveform;

class PndEmcPSATrapDigiFilter: public PndEmcAbsPSA {
public:

	// Constructors/Destructors ---------
	PndEmcPSATrapDigiFilter(const std::vector<Double_t> params);
	virtual ~PndEmcPSATrapDigiFilter(){;}

	virtual Int_t Process(const PndEmcWaveform *waveform);
	virtual void GetHit(Int_t i, Double_t &energy, Double_t &time);
	
	virtual void Process(const PndEmcWaveform * waveform, Double_t &amplitude, Double_t &time);
	void Draw(const PndEmcWaveform * waveform);
	
	std::vector<Double_t> fParams; // Parameters of the filter

private:
	Double_t lAmplitude;
	Double_t lTime;
	
};

#endif
