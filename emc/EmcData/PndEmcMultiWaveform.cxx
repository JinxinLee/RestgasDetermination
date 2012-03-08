
//=============================================================================
// PndEmcMultiWaveform.cxx
//
// Class to hold multiple waveforms from one Emc Hit / ADC readout
//
// Author: Christian Hammmann
//         chammann@hiskp.uni-bonn.de
//
//=============================================================================

#include "PndEmcMultiWaveform.h"


PndEmcMultiWaveform::PndEmcMultiWaveform(int trackId, long detId, long waveform_length, Int_t hitIndex):PndEmcWaveform(trackId,detId,waveform_length,hitIndex),
	fSignals(1,std::vector<Double_t>(waveform_length,0.)),
	fActiveWaveform(1)
{
	SetLink(FairLink("EmcHit", hitIndex));
}

void PndEmcMultiWaveform::clearAndReset(){
	fSignals.clear();
	fSignals.at(0)=std::vector<Double_t>(fWaveformLength,0.);
}

std::vector<Double_t> PndEmcMultiWaveform::GetSignal() const{
	if( fActiveWaveform <1 || fActiveWaveform >= fSignals.size()){
		return fSignals.at(0);
	}else{
		return fSignals.at(fActiveWaveform);
	}
}

void PndEmcMultiWaveform::SetWaveform(std::vector<Double_t>&signal,Int_t length,Int_t Waveform){
	if(Waveform < 0){
		return;
	}
	if(Waveform>=fSignals.size()){
		fSignals.insert(fSignals.end(),Waveform-fSignals.size()+1,std::vector<Double_t>(fWaveformLength,0.));
	}
	fSignals[Waveform]= signal;
}


ClassImp(PndEmcMultiWaveform);
