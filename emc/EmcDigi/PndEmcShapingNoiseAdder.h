//#pragma once
#ifndef PNDEMCSHAPINGNOISEADDER_H_
#define PNDEMCSHAPINGNOISEADDER_H_

#include "PndEmcAbsWaveformModifier.h"

class PndEmcWaveform;
class PndEmcAbsWaveformSimulator;

class PndEmcShapingNoiseAdder : public PndEmcAbsWaveformModifier {
	
	public:
		PndEmcShapingNoiseAdder();
		PndEmcShapingNoiseAdder(Double_t fShapingTime, Double_t sampleRate, Double_t noiseWidth, Double_t inputScale);

		virtual void  Modify(PndEmcWaveform* wf);

	protected:

	private:
		Double_t fShapingTime;
		Double_t fNoiseWidth;
		Double_t fInputScale;

		Double_t fSingleNoiseWidth;


		class MA {			// moving Average Buffer
		    private:
			std::vector<Double_t> buffer;
			UInt_t bpointer;
			Double_t sum;

		    public:
			MA(UInt_t size): buffer(size, 0.0), bpointer(0), sum(0) {};

			Double_t operator() (Double_t xN) {
			    sum += xN - buffer[bpointer];
			    buffer[bpointer] = xN;
			    bpointer++;
			    bpointer %= buffer.size();

			    return sum/buffer.size();
			}
			UInt_t GetSize() { return buffer.size(); };
		} fMA_buffer;

		ClassDef(PndEmcShapingNoiseAdder, 0)
};

#endif
