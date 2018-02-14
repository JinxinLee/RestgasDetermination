#include "PndEmcAbsPSA.h"
void PndEmcAbsPSA::Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time){
		if(Process(waveform)>0){
			GetHit(0,amplitude,time);
		}else{
			amplitude = 0.;
			time = 0.;
		}
	}; //deprecated do not use

ClassImp(PndEmcAbsPSA);

