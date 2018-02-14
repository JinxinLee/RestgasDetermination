#pragma once
#ifndef PNDEMCABSWAVEFORMMODIFIER_H_
#define PNDEMCABSWAVEFORMMODIFIER_H_

#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"

#include "TObject.h"

/**
 * @brief interface for waveform modifiers
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcAbsWaveformModifier : public TObject {
	public: 
		virtual void Modify(PndEmcWaveform* wf) = 0;
		virtual Double_t GetScale() { return 1.; };

		virtual ~PndEmcAbsWaveformModifier() {};

	protected:
		std::vector<Double_t>& GetWaveformReference(PndEmcWaveform* wf) {
			if(PndEmcMultiWaveform* multiWf = dynamic_cast<PndEmcMultiWaveform*>(wf)) {
				Int_t activeWf = multiWf->GetActiveWaveform();
				return multiWf->fSignals.at(activeWf);
			}
			return wf->fSignal;
		}

	ClassDef(PndEmcAbsWaveformModifier, 1);
};

#endif
