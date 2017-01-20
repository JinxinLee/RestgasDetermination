#include "PndEmcHighLowPSA.h"
#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcPSAFPGA/PndEmcPSAFPGASampleAnalyser.h"

#include <iostream>


PndEmcHighLowPSA::PndEmcHighLowPSA(Int_t verbose) : fLowgainPSA(NULL), fHighgainPSA(NULL), fVerbose(verbose) {
};

void PndEmcHighLowPSA::Init(PndEmcPSAFPGASampleAnalyser* highgainPSA, PndEmcPSAFPGASampleAnalyser* lowgainPSA, Double_t overflowThreshold, Int_t highgainWfIndex, Int_t lowgainWfIndex) {
	fHighgainPSA = highgainPSA;
	fLowgainPSA = lowgainPSA;
	fIdx_high = highgainWfIndex;
	fIdx_low = lowgainWfIndex;
	fOverflowThreshold = overflowThreshold;
}

void PndEmcHighLowPSA::Reset() {
	//fActivePSA=0;
	fHitsInFE.clear();
	fHighgainPSA->Reset();
	fLowgainPSA->Reset();
}


Int_t PndEmcHighLowPSA::Process(const PndEmcWaveform* waveform) {

	Reset();

	PndEmcMultiWaveform* multiWf = const_cast<PndEmcMultiWaveform*>(dynamic_cast<const PndEmcMultiWaveform*>(waveform));

	if(!multiWf) {
		std::cerr << "passed waveform is not of type PndEmcMultiwaveform" << std::endl;
		return -1;
	}

	//active method
	
	Int_t activeWf = multiWf->GetActiveWaveform();

	multiWf->SetActiveWaveform(fIdx_low);
	std::vector<double> signal_low = waveform->GetSignal();
	std::vector<double>::iterator it_low=signal_low.begin();

	multiWf->SetActiveWaveform(fIdx_high);
	std::vector<double> signal_high = waveform->GetSignal();
	std::vector<double>::iterator it_high=signal_high.begin();

	multiWf->SetActiveWaveform(activeWf);

	bool active_high = false;
	bool active_low = false;

	Int_t counter_low=0;
	Int_t counter_high=0;

	PndEmcPSAFPGASampleAnalyser::status_t status_low;
	PndEmcPSAFPGASampleAnalyser::status_t status_high;

	std::set<Int_t> hits_lowgain;
	bool overflow = false;

	while(it_low!=signal_low.end() && it_high!=signal_high.end()) {

		fHighgainPSA->put(*it_high);
		fLowgainPSA->put(*it_low);
		status_high = fHighgainPSA->GetStatus();
		status_low = fLowgainPSA->GetStatus();

		if(status_high==PndEmcPSAFPGASampleAnalyser::kPulseFinished) {
			active_high = false;
			if(!overflow) {
				fHitsInFE.push_back(std::make_pair(fHighgainPSA, counter_high));
				if(fVerbose>=2) { 
					std::cout << "I- PndEmcHighLowPSA: adding highgain hit: #:"<< counter_high << std::endl;
				}
			}
			counter_high++;
		} else if(status_high==PndEmcPSAFPGASampleAnalyser::kPileupFinished) {
			active_high = false;
			if(!overflow) {
				fHitsInFE.push_back(std::make_pair(fHighgainPSA, counter_high));
				fHitsInFE.push_back(std::make_pair(fHighgainPSA, counter_high+1));
				if(fVerbose>=2) { 
					std::cout << "I- PndEmcHighLowPSA: adding highgain hits #:" << counter_high << "," << counter_high++ << std::endl;
				}
			}
			counter_high+=2;
		} else if(status_high==PndEmcPSAFPGASampleAnalyser::kPulseDetected) {
			active_high = true;
		}

		if(active_high && ((*it_high) > fOverflowThreshold) && !overflow) {
			if(fVerbose>=2) { 
				std::cout << "I- PndEmcHighLowPSA: overflow detected" << std::endl;
			}
			overflow = true;
		}

		if(status_low==PndEmcPSAFPGASampleAnalyser::kPulseFinished) {
			hits_lowgain.insert(counter_low);
			counter_low++;
			active_low = false;
		} else if(status_low==PndEmcPSAFPGASampleAnalyser::kPileupFinished) {
			if(fVerbose>=3) {
				std::cout << "I- PndEmcHighLowPSA: pileup in lowgain candidate: #" << counter_low << ", #" << counter_low+1 << std::endl;
		}
			hits_lowgain.insert(counter_low);
			hits_lowgain.insert(counter_low+1);
			counter_low+=2;
			active_low = false;
		} else if(status_low==PndEmcPSAFPGASampleAnalyser::kPulseDetected) {
			active_low = true;
		}

		if((!active_high) && (!active_low)) {
			if(overflow) {
				if(fVerbose>=2) { 
					std::cout << "I- PndEmcHighLowPSA: adding lowgain hit(s) #:";
					for(std::set<Int_t>::iterator it=hits_lowgain.begin(); it!=hits_lowgain.end(); ++it) {
						std::cout << *it << ",";
					}
					std::cout << "\b\n" << std::flush;
				}
				for(std::set<Int_t>::iterator it=hits_lowgain.begin(); it!=hits_lowgain.end(); ++it) {
					fHitsInFE.push_back(std::make_pair(fLowgainPSA, *it));
				}
			}
			hits_lowgain.clear();
			overflow = false;
		}
		it_low++;
		it_high++;
	}

	return fHitsInFE.size();
}


void PndEmcHighLowPSA::GetHit(Int_t i, Double_t& energy, Double_t& time) {

	PndEmcPSAFPGASampleAnalyser* psa = fHitsInFE[i].first;
	if(psa!=NULL) {
		psa->GetHit(fHitsInFE[i].second, energy, time);
	} else {
		//std::cerr << "-E PndEmcHighLowPSA::GetHit: No suitable PSA available" << std::cout;
		energy = 0;
		time = 0;
	}

}

Int_t PndEmcHighLowPSA::GetWaveformIdx(Int_t i) {

	if(i<(int)fHitsInFE.size() && i>=0) {
		PndEmcPSAFPGASampleAnalyser* psa = fHitsInFE[i].first;
		if(psa==fHighgainPSA) return fIdx_high;
		else if(psa==fLowgainPSA) return fIdx_low;
	}

	return -1;		//error
}
	

