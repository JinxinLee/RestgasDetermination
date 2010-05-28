//-----------------------------------------------------------
//
// Description:
//      Pulseshape analysis for ADC waveforms
//      Trapezoidal filter represents convolution of
//      MWD (moving window deconvolution) and MA (moving average)
//      It is characterised by the rise time and the length of the flat top part
//      (in sampling periods)
//      Dima Melnychuk
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndEmcPSATrapDigiFilter.h"
#include "PndEmcWaveform.h"
#include "PndEmcFadcFilter.h"

#include "TH1D.h"
#include "TCanvas.h"

// Class Member definitions -----------

PndEmcPSATrapDigiFilter::PndEmcPSATrapDigiFilter(const std::vector<Double_t> params):
fParams(params)
{}

void PndEmcPSATrapDigiFilter::Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time)
{
	const std::vector<Double_t> signal=waveform->GetSignal();
	std::vector<Double_t> output;
	
	// Signal filtartion 
	Int_t rise_time=(Int_t) fParams[0]; // rise time of trapezoid (in sampling periods)
	Int_t flat_length=(Int_t) fParams[1]; // length of the flat top period of trapezoid (in sampling periods) 
	Int_t shift=(Int_t) fParams[2]; // shift of the point to determine energy
	
	// Energy is determined from the amplitude at the end of the flat to period (with possible delats due to some latencies) 
	Int_t energy_point = rise_time+flat_length+shift-1; 
	
	PndEmcFadcFilter *flt  = new PndEmcFadcFilter();
	flt->SetupTrapez(rise_time,flat_length);
	flt->Filter(signal, output);
	delete flt;
	
	// Extraction of energy information from the flat top part of trapezoidal signal
	//output
	amplitude=output[energy_point];
	
	// Detrmination of the time is not yet implemented
	time=0;
			
	return;
}

