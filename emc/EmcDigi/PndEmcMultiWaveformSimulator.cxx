#include "PndEmcMultiWaveformSimulator.h"
#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcAbsPulseshape.h"
#include "PndEmcAbsWaveformModifier.h"

#include "TClonesArray.h"
#include "TList.h"

#include <vector>


PndEmcMultiWaveformSimulator::PndEmcMultiWaveformSimulator() : PndEmcFullStackedWaveformSimulator(), fWfMultiplicity(0) {
}

PndEmcMultiWaveformSimulator::PndEmcMultiWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape* pulseshape, Int_t wfMultiplicity) : PndEmcFullStackedWaveformSimulator(sampleRate, pulseshape), fWfMultiplicity(wfMultiplicity) {
}


PndEmcWaveform* PndEmcMultiWaveformSimulator::MakeWaveform(PndEmcWaveformData* wfData, TClonesArray* arrayToStore) {
	PndEmcWaveform* singleWaveform = PndEmcFullStackedWaveformSimulator::MakeWaveform(wfData, NULL);
	PndEmcMultiWaveform* multiWaveform = MultiplyWaveform(singleWaveform, arrayToStore);

	delete singleWaveform;
	return multiWaveform;
}


PndEmcWaveform* PndEmcMultiWaveformSimulator::MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray* arrayToStore, Int_t detId, Int_t trackId, Int_t hitIndex) {
	PndEmcWaveform* singleWaveform = PndEmcFullStackedWaveformSimulator::MakeSingleWaveform(hitEnergy, hitTime, NULL, detId, trackId, hitIndex);
	PndEmcMultiWaveform* multiWaveform = MultiplyWaveform(singleWaveform, arrayToStore);
	
	delete singleWaveform;
	return multiWaveform;
}

//pass wfIndex Modifier shall work on as second parameter
void PndEmcMultiWaveformSimulator::AddModifier(PndEmcAbsWaveformModifier* wfModifier, Int_t wfIndex) {
	fIndexList.push_back(wfIndex);
	PndEmcFullStackedWaveformSimulator::AddModifier(wfModifier);
}

PndEmcWaveform* PndEmcMultiWaveformSimulator::CallModifiers(PndEmcWaveform* wf) {

	PndEmcMultiWaveform* multiWaveform = dynamic_cast<PndEmcMultiWaveform*>(wf);
	Int_t count = 0;
	TIter iterModifiers(&fListOfWaveformModifiers);

	while(PndEmcAbsWaveformModifier* wfmod = (PndEmcAbsWaveformModifier*) iterModifiers()) {
		if(fIndexList[count]>=0) {
			multiWaveform->SetActiveWaveform(fIndexList[count]);
		}
		wfmod->Modify(wf);
		++count;
	}	       
	return wf;
}


Double_t PndEmcMultiWaveformSimulator::GetTotalScale(Int_t wfIndex) {

	Double_t totalScale = GetScale();

	TIter iterModifiers(&fListOfWaveformModifiers);
	Int_t count = 0;

	while(PndEmcAbsWaveformModifier* wfmod = (PndEmcAbsWaveformModifier*) iterModifiers()) {
		if(fIndexList[count] == wfIndex || fIndexList[count]<0) {	
			totalScale *= wfmod->GetScale();
		}
		++count;
	}
	return totalScale;
}


PndEmcMultiWaveform* PndEmcMultiWaveformSimulator::MultiplyWaveform(const PndEmcWaveform* singleWaveform, TClonesArray* arrayToStore) {

	const std::vector<Double_t>& signal = singleWaveform->GetSignal();

	PndEmcMultiWaveform* multiWaveform = NULL;
	
	if(NULL!=arrayToStore) {
		multiWaveform = new ((*arrayToStore)[arrayToStore->GetEntriesFast()]) PndEmcMultiWaveform(singleWaveform->GetTrackId(), singleWaveform->GetDetectorId(), signal.size(), singleWaveform->GetHitIndex());
	} else {
		multiWaveform = new PndEmcMultiWaveform(singleWaveform->GetTrackId(), singleWaveform->GetDetectorId(), signal.size(), singleWaveform->GetHitIndex());
	}

	for(Int_t iWf = 0; iWf<fWfMultiplicity; ++iWf) {
		multiWaveform->SetWaveform(signal, signal.size(), iWf);
	}

	multiWaveform->SetLinks(singleWaveform->GetLinks());
	multiWaveform->SetTimeStamp(singleWaveform->GetTimeStamp());

	return multiWaveform;
}

