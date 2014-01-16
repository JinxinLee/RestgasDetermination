#pragma once
#ifndef PNDEMCWAVEFORMDIGITIZER_H_
#define PNDEMCWAVEFORMDIGITIZER_H_

class PndEmcWaveformDigitizer : public PndEmcAbsWaveformModifier {
	public:
		PndEmcWaveformDigitizer();	
		PndEmcWaveformDigitizer(Int_t nBits, Double_t energyRange, Double_t inputScale);

		virtual void  Modify(PndEmcWaveform* wf);
		virtual Double_t GetScale() { return fTotalChannels/fInputScale; }

	private:

		Int_t  fNBits;
		Double_t fEnergyRange; 		//in GeV;
		Double_t fInputScale;

		Int_t fTotalChannels;
		Double_t fOneBitResolution;

		ClassDef(PndEmcWaveformDigitizer, 0)
 };

#endif
