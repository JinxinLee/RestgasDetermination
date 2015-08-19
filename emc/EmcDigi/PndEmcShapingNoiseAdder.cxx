#include "PndEmcShapingNoiseAdder.h"
#include "PndEmcAbsWaveformSimulator.h"

#include "PndEmcWaveform.h"
#include "TRandom.h"
#include "TMath.h"

PndEmcShapingNoiseAdder::PndEmcShapingNoiseAdder() : 
	fShapingTime(0),
	fNoiseWidth(0.),
	fSingleNoiseWidth(0.0),
	fMA_buffer(0),
	fInputScale(0.0) {
}

PndEmcShapingNoiseAdder::PndEmcShapingNoiseAdder(Double_t shapingTime, Double_t sampleRate, Double_t noiseWidth, Double_t inputScale) :
	fShapingTime(shapingTime),
	fNoiseWidth(noiseWidth),
	fInputScale(inputScale),
	//MA buffer size as long as no of samples forming rising edge of wf

	fMA_buffer((Int_t) (shapingTime*sampleRate)) {
		Int_t bufferSize = fMA_buffer.GetSize();

		if(!bufferSize) {
			std::cout << "-E PndEmcShapedNoiseWaveformSimulator: pulse raises in less than one sample. Noise generation will fail" << std::endl;
		}

		//noise of individual sample is averaged over bufferSize samples --> fNoiseWidth² = bufferSize * fSingleNoiseWidth²
		fSingleNoiseWidth = fNoiseWidth * TMath::Sqrt(bufferSize);			//still in GeV

		//Init buffer
		while(bufferSize--) { 								//init buffer
			fMA_buffer(gRandom->Gaus(0, fSingleNoiseWidth*fInputScale));
		}
	}


void PndEmcShapingNoiseAdder::Modify(PndEmcWaveform* wf) {

	for(std::vector<Double_t>::iterator it=GetWaveformReference(wf).begin(); it!=GetWaveformReference(wf).end(); ++it) { 
	
		(*it) += fMA_buffer(gRandom->Gaus(0, fSingleNoiseWidth*fInputScale)); //smear with shaping noise
	}
}
