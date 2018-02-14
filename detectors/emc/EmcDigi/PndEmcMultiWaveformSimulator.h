#ifndef PNDEMCMULTIWAVEFORMSIMULATOR_H_
#define PNDEMCMULTIWAVEFORMSIMULATOR_H_

#include "PndEmcFullStackedWaveformSimulator.h"
#include <vector>

class PndEmcAbsPulseshape;
class PndEmcMultiWaveform;
class TClonesArray;

/**
 * @brief Simulator to create PndEmcMultiWaveform
 * 
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcMultiWaveformSimulator : public PndEmcFullStackedWaveformSimulator 
{
	public:
		PndEmcMultiWaveformSimulator();
		PndEmcMultiWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape* pulseshape, Int_t wfMultiplicity);	

		/*! wfIndex characterizes wf, the modifier will apllied on*
		 *  If the modifier itself will work on the multiwaveform itself, pass a -1. In this case, the Modifier is considered to scale all waveforms equally.
		 */
		virtual void AddModifier(PndEmcAbsWaveformModifier* wfModifier, Int_t wfIndex);

		virtual Double_t GetTotalScale(Int_t wfIndex);

	protected:
		virtual PndEmcWaveform* CallModifiers(PndEmcWaveform* wf);
		virtual PndEmcWaveform* MakeWaveform(PndEmcWaveformData* wfData, TClonesArray* arrayToStore=NULL);
		virtual PndEmcWaveform* MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray* arrayToStore=NULL, Int_t detId=-1, Int_t trackId=-1, Int_t hitIndex=-1);


	private:
		Int_t fWfMultiplicity;
		std::vector<Int_t> fIndexList;

		PndEmcMultiWaveform* MultiplyWaveform(const PndEmcWaveform* singleWaveform, TClonesArray* arrayToStore);

	ClassDef(PndEmcMultiWaveformSimulator, 1)
};

#endif
