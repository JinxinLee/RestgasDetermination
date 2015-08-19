//#pragma once
#ifndef PNDEMCWFRESCALER
#define PNDEMCWFRESCALER

#include "PndEmcAbsWaveformModifier.h"

class PndEmcWaveform;
class PndEmcAbsWaveformSimulator;



class PndEmcWfRescaler : public PndEmcAbsWaveformModifier {
	
	public:
		PndEmcWfRescaler();
		PndEmcWfRescaler(Double_t inputScale, Double_t outputScale);
		PndEmcWfRescaler(Double_t multiplicator);
		virtual void Modify(PndEmcWaveform* wf);
		virtual Double_t GetScale() { return fMultiplicator; }

	private:
		Double_t fMultiplicator;

	ClassDef(PndEmcWfRescaler,1)
};

#endif
