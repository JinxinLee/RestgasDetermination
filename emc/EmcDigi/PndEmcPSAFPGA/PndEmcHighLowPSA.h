//-----------------------------------------------------------
// Description:
//     	Wrapping class to manage two independent PSAs 
//     	for high, resp. low gain 
//      Philipp Mahlberg
//-----------------------------------------------------------


#ifndef PNDEMCHIGHLOWPSA_H_
#define PNDEMCHIGHLOWPSA_H_


#include "PndEmcAbsPSA.h"

class PndEmcPSAFPGASampleAnalyser;


/**
 * @brief Wrapping class to manage two independent PSAs for high, resp. low gain
 * 
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcHighLowPSA : public PndEmcAbsPSA 
{
	public:
		PndEmcHighLowPSA(Int_t verbose=0);
		virtual void Init(PndEmcPSAFPGASampleAnalyser* highgainPSA, PndEmcPSAFPGASampleAnalyser* lowgainPSA, Double_t overflowThreshold, Int_t highgainWfIndex=0, Int_t lowgainWfIndex=1);
		virtual ~PndEmcHighLowPSA(){}

		virtual void SetVerbose(Int_t verbose=0) { fVerbose = verbose; };
		virtual Int_t Process(const PndEmcWaveform* waveform);
		virtual void Reset();
		virtual void GetHit(Int_t i, Double_t &energy, Double_t &time);
		virtual Int_t GetWaveformIdx(Int_t i);

	private:
		PndEmcPSAFPGASampleAnalyser* fHighgainPSA;
		PndEmcPSAFPGASampleAnalyser* fLowgainPSA;
		
		Int_t fIdx_high;
		Int_t fIdx_low;

		std::vector< std::pair<PndEmcPSAFPGASampleAnalyser*, int> >fHitsInFE;
		Double_t fOverflowThreshold;

		Int_t fVerbose;	

		ClassDef(PndEmcHighLowPSA, 1)
};

#endif
