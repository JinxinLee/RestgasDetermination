//-----------------------------------------------------------
// Description:
//      Pulseshape analysis for ADC waveforms
//      Amplitude and time are determined from the simple parabolic fit
//      by three points (maximum bin and its left and right neigbours)
//
//-----------------------------------------------------------

#ifndef EMCPSAPARABOLICBASELINE_HH
#define EMCPSAPARABOLICBASELINE_HH

// Base Class Headers ----------------
#include <vector>
#include "PndEmcAbsPSA.h"
#include "TObject.h"		

class PndEmcWaveform;

class PndEmcPSAParabolicBaseline: public PndEmcAbsPSA {
public:

	// Constructors/Destructors ---------
	PndEmcPSAParabolicBaseline(Int_t baselength =0):lBaselength(baselength){}
	virtual ~PndEmcPSAParabolicBaseline(){;}
	
	virtual void Process(const PndEmcWaveform * waveform, Double_t &amplitude, Double_t &time);
	
	void FitPeak(const std::vector<Double_t> & signal, Double_t& ampl, Double_t& pos,Int_t peakBin) const;
	void FitPeak(const std::vector<Double_t> & signal, Double_t& ampl, Double_t& pos, Int_t start, Int_t end) const;
	void FitPeak(const std::vector<Double_t> & signal, Double_t& ampl, Double_t& pos) const;
	void GetBaseline(const std::vector<Double_t> &signal,Double_t &baseline)const;
	private:
	Int_t lBaselength;
};

#endif
