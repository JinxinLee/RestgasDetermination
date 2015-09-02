#include "PndEmcFullStackedWaveformSimulator.h"

#include "PndEmcWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcAbsPulseshape.h"

#include "TClonesArray.h"
#include "TMath.h"

#include "FairLink.h"

#include <vector>
#include <map>
#include <iostream>

using namespace std;

PndEmcFullStackedWaveformSimulator::PndEmcFullStackedWaveformSimulator() : PndEmcAbsWaveformSimulator(0.), fSamplingBeforeFirstPulse(0), fSamplingAfterLastPulse(0), fCutoff(0), fPulseshape(NULL), fScale(0.), f1GeVWaveform(NULL), fPulseRiseTime(0), fActiveTimeIncrement(0) {
}

PndEmcFullStackedWaveformSimulator::PndEmcFullStackedWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape* pulseshape) : PndEmcAbsWaveformSimulator(sampleRate), fSamplingBeforeFirstPulse(0), fSamplingAfterLastPulse(0), fCutoff(0), fPulseshape(pulseshape), fScale(0.), f1GeVWaveform(NULL), fPulseRiseTime(0), fActiveTimeIncrement(0) {
}


PndEmcFullStackedWaveformSimulator::~PndEmcFullStackedWaveformSimulator() {
	delete f1GeVWaveform;
}

void PndEmcFullStackedWaveformSimulator::Init(Double_t samplingBeforeFirstPulse, Double_t samplingAfterLastPulse, Double_t cutoff, Double_t activeTimeIncrement) {

	fSamplingBeforeFirstPulse = samplingBeforeFirstPulse;
	fSamplingAfterLastPulse = samplingAfterLastPulse;
	fCutoff = cutoff;
	fActiveTimeIncrement = activeTimeIncrement;

	//generate signal with:
	Double_t tmpSamlingRate = 1.0; // in ns^-1
	Int_t tmpLength = 100; //in samples
	Int_t emergencyStop = 10000000;

	std::vector<Double_t> tmpSignal;

	Bool_t maxFound = kFALSE;
	Bool_t cutoffFound = kFALSE;

	Int_t maxSample;
	Int_t cutoffSample;

	do {
		tmpLength*=2;
		tmpSignal.resize(tmpLength);
		if(tmpLength>=emergencyStop) {
			std::cout << "-E PndEmcFullStackedWaveformSimulator: no maximum in pulse during " << emergencyStop/tmpSamlingRate << " ns found. WaveformGeneration might fail." << std::endl; 
			break;
		}

		for(Int_t iSample=0; iSample<tmpLength; iSample++) {
			tmpSignal[iSample] = CalcSingleWaveForTime(iSample/tmpSamlingRate, 1.0, 0.);
		}
		

		if(!maxFound) {	//search for maximum
			std::vector<Double_t>::const_iterator begin_constIt = tmpSignal.begin();
			std::vector<Double_t>::const_iterator end_constIt = tmpSignal.end();
			std::vector<Double_t>::const_iterator max_constIt = std::max_element(begin_constIt, end_constIt);
			maxSample = std::distance(begin_constIt, max_constIt);

			//generated pusle might be to short for proper maximum detection
			maxFound = (maxSample < tmpLength-1) ? kTRUE : kFALSE; 

			fScale = (*max_constIt);
			fPulseRiseTime = maxSample * tmpSamlingRate;
		}

		if(maxFound) { //search for cutOff
			for(Int_t iSample=maxSample; iSample<tmpLength; iSample++) {
				if(tmpSignal[iSample] < fCutoff*fScale) {
					cutoffFound = kTRUE;				
					cutoffSample = iSample;
					break;
				}
			}
		}

	} while(!(maxFound && cutoffFound)); 		//maximum inside signal, i.e. signal covers tail (at least partially) 

	//generate 1GeV signal again..now with correct sampling rate
	Double_t totalTime = (cutoffSample/tmpSamlingRate + fSamplingBeforeFirstPulse + fSamplingAfterLastPulse);

	try{
	std::vector<Double_t> signal((Int_t) TMath::Floor(totalTime*GetSampleRate() + 0.5), 0.);
	} catch(...) {
		std::cerr << "catched error" << std::endl;
		std::cerr << "sampleRate:" << GetSampleRate() << std::endl;
		std::cerr << "total Time:" << totalTime << std::endl;
		abort();
	}

	f1GeVWaveform = MakeSingleWaveform(1.0, 0.);

	std::cerr << "finishing init" << std::endl;

}



void PndEmcFullStackedWaveformSimulator::GetAbsoluteTimeInterval(PndEmcWaveformData* wfData, Double_t& startTime, Double_t& activeTime) {

	const std::map<Double_t, Double_t>& hitMap = wfData->GetHitMap();
	startTime = hitMap.begin()->first - fSamplingBeforeFirstPulse;
	
	activeTime = hitMap.rbegin()->first + fPulseRiseTime;
	while (TMath::Abs(CalcWaveForTime(activeTime, wfData)) > fCutoff*fScale) {
		activeTime += fActiveTimeIncrement;
	}

	activeTime += fSamplingAfterLastPulse;

	SyncWithADCClock(startTime);
	SyncWithADCClock(activeTime);
}

Double_t PndEmcFullStackedWaveformSimulator::CalcSingleWaveForTime(Double_t absoluteTime, Double_t energy, Double_t pulseTime) {
	return fPulseshape->value(absoluteTime, energy, pulseTime);
}

Double_t PndEmcFullStackedWaveformSimulator::CalcWaveForTime(Double_t absoluteTime, PndEmcWaveformData* wfData) {
	Double_t returnValue = 0;

	for(std::map<Double_t, Double_t>::const_iterator it = wfData->GetHitMap().begin(); it!=wfData->GetHitMap().end(); ++it) {
		Double_t energy = it->second;
		Double_t pulseTime = it->first;

		returnValue += CalcSingleWaveForTime(absoluteTime, energy, pulseTime);
	}
	return returnValue;
}

PndEmcWaveform* PndEmcFullStackedWaveformSimulator::MakeWaveform(PndEmcWaveformData* wfData, TClonesArray* arrayToStore) {


	Int_t nSamples = static_cast<Int_t>((wfData->GetTimeOfLastSample()-wfData->GetTimeStamp())*GetSampleRate() + 0.5);
	//cout << "TimeOfLastSample: " << wfData->GetTimeOfLastSample() << endl;
	//cout << "Timestamp: " << wfData->GetTimeStamp() << endl;
	//cout << "sampleRate: " << GetSampleRate() << endl;
	//cout << "calculted " << nSamples << " samples" << endl;
	std::vector<Double_t> signal(nSamples, 0.0);

	for(Int_t iSample=0; iSample<nSamples; ++iSample) {
		signal[iSample] = CalcWaveForTime(wfData->GetTimeStamp() + iSample*1/GetSampleRate(), wfData);
	}

	Int_t hitIndex = -1;
	if(wfData->GetNLinks()) {
		hitIndex = wfData->GetLink(0).GetIndex();
	}

	PndEmcWaveform* wave;	
	if(arrayToStore) {
		wave = new ((*arrayToStore)[arrayToStore->GetEntriesFast()]) PndEmcWaveform(-1, wfData->GetDetectorId(), signal, hitIndex);
		wave->ResetLinks();
	} else {
		wave = new PndEmcWaveform(-1, wfData->GetDetectorId(), signal, hitIndex);
	}
	wave->SetLinks(wfData->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit")));
	wave->AddInterfaceData(wfData);

	Double_t timeStamp = wfData->GetTimeStamp();
	wave->SetTimeStamp(timeStamp);
	wave->SetSampleRate(fSampleRate*1.0e9);		//converting form ns into s;

	return wave;

}
				
PndEmcWaveform* PndEmcFullStackedWaveformSimulator::MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray* arrayToStore, Int_t detId, Int_t trackId, Int_t hitIndex) {

	std::vector<Double_t> signal;

	Double_t startTime = hitTime - fSamplingBeforeFirstPulse;
	SyncWithADCClock(startTime);

	Int_t nRising = (Int_t) ((fSamplingBeforeFirstPulse + fPulseRiseTime) * GetSampleRate());
	Int_t iSample =0;

	for(;iSample<nRising; ++iSample) { 	//time before pulse + rising edge
		signal.push_back(CalcSingleWaveForTime(startTime + iSample/GetSampleRate(), hitEnergy, hitTime));
	}

	do { 					//falling edge..check weather pulse undergoes cutoff value
		signal.push_back(CalcSingleWaveForTime(startTime + iSample/GetSampleRate(), hitEnergy, hitTime));
	} while (signal[iSample++] > fCutoff*fScale);

	Int_t addSamples = (Int_t) (fSamplingAfterLastPulse*GetSampleRate());

	for(addSamples += iSample; iSample<addSamples; iSample++) {
		signal.push_back(CalcSingleWaveForTime(startTime + iSample/GetSampleRate(), hitEnergy, hitTime));
	}

	PndEmcWaveform* wave;	
	if(arrayToStore) {		
		wave = new ((*arrayToStore)[arrayToStore->GetEntriesFast()]) PndEmcWaveform(trackId, detId, signal, hitIndex);
	} else {
		wave = new PndEmcWaveform(trackId, detId, signal, hitIndex);
	}

	Double_t timestamp = startTime;
	wave->SetTimeStamp(timestamp);
	wave->SetSampleRate(fSampleRate*1.0e9);		//converting form ns into s;

	return wave;
}
