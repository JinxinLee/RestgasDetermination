//#pragma once
#ifndef PNDEMCFULLSTACKEDWAVEFORMSIMULATOR_H_
#define PNDEMCFULLSTACKEDWAVEFORMSIMULATOR_H_

#include "PndEmcAbsWaveformSimulator.h"
#include <vector>

class PndEmcWaveformData;
class PndEmcWaveform;
class PndEmcAbsPulseshape;
class TClonesArray;

#include <iostream>

class PndEmcFullStackedWaveformSimulator : public PndEmcAbsWaveformSimulator {

	public:
		PndEmcFullStackedWaveformSimulator();
		PndEmcFullStackedWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape* pulseShape);
		virtual ~PndEmcFullStackedWaveformSimulator();

		virtual void Init(Double_t samplingBeforeFirstPulse, Double_t samplingAfterLastPulse, Double_t cutoff, Double_t activeTimeIncrement); //

		virtual void GetAbsoluteTimeInterval(PndEmcWaveformData* wfData, Double_t& startTime, Double_t& activeTime);	//in ns

		virtual Double_t GetTimeBeforeFirstHit(PndEmcWaveformData* wfData) { return fSamplingBeforeFirstPulse; }					//in ns

		virtual Double_t GetScale() { return fScale; }	
		virtual PndEmcWaveform* Get1GevWaveform() { return f1GeVWaveform; };

		virtual Double_t GetPulseRaiseTime() { return fPulseRaiseTime; };

	protected:

		virtual PndEmcWaveform* MakeWaveform(PndEmcWaveformData* wfData, TClonesArray* arrayToStore=NULL);
		virtual PndEmcWaveform* MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray* arrayToStore=NULL, Int_t detId=-1, Int_t trackId=-1, Int_t hitIndex=-1);

		virtual Double_t CalcWaveForTime(Double_t absoluteTime, PndEmcWaveformData* wfData);
		Double_t CalcSingleWaveForTime(Double_t absoluteTime, Double_t energy, Double_t pulseTime);

	private:
		Double_t fSamplingBeforeFirstPulse; //!< in ns. additional time interval before waveform starts
		Double_t fSamplingAfterLastPulse; //!< in ns. additional time interval after waveform falls below cutOff value
		Double_t fCutoff; //<! minimum pulse after which waveform simulation is terminated. Given as corresponding energy/GeV when cutoff value treated as pulse amplitude. 

		PndEmcAbsPulseshape* fPulseshape;

		Double_t fScale;	//!<Height of 1GeV pulse
		PndEmcWaveform* f1GeVWaveform;

		Double_t fPulseRaiseTime; //!< total raising time of pulse in ns

		Double_t fActiveTimeIncrement;


	ClassDef(PndEmcFullStackedWaveformSimulator, 1);

};

#endif
