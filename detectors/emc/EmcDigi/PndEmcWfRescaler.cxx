#include "PndEmcWfRescaler.h"

PndEmcWfRescaler::PndEmcWfRescaler() : fMultiplicator(1.) {
}


PndEmcWfRescaler::PndEmcWfRescaler(Double_t inputScale, Double_t outputScale) : fMultiplicator(outputScale/inputScale) {
}

PndEmcWfRescaler::PndEmcWfRescaler(Double_t multiplicator) : fMultiplicator(multiplicator) {
}


void PndEmcWfRescaler::Modify(PndEmcWaveform* wf) {
	for(std::vector<Double_t>::iterator it=GetWaveformReference(wf).begin(); it!=GetWaveformReference(wf).end(); ++it) { 
		(*it) *= fMultiplicator;
	}
}
