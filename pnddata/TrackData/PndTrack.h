/*
 * PndTrack.h
 *
 *  Created on: 05.03.2009
 *      Author: everybody
 */

#ifndef PNDTRACK_H_
#define PNDTRACK_H_

#include "TObject.h"
#include "PndTrackCand.h"
#include "FairTrackParP.h"


class PndTrack : public TObject{
public:
	PndTrack();
	PndTrack(const FairTrackParP& first, const FairTrackParP& last, const PndTrackCand& cand);

	void Print();


	Int_t GetPidHypo()               const { return fPidHypo; }
	Int_t GetFlag()                  const { return fFlag; } //Quality flag
	Double_t GetChi2()               const { return fChi2; }
	Int_t GetNDF()                   const { return fNDF; }
	void SetPidHypo(Int_t i)         { fPidHypo=i; }
	void SetFlag(Int_t i)            { fFlag=i; }
	void SetChi2(Double_t d)         { fChi2=d; }
	void SetNDF(Int_t i)             { fNDF=i; }
        PndTrackCand GetTrackCand()      { return fTrackCand; }
	FairTrackParP GetParamFirst() { return fTrackParamFirst; }
	FairTrackParP GetParamLast()  { return fTrackParamLast ; }

private:
	FairTrackParP fTrackParamFirst;
	FairTrackParP fTrackParamLast;

	PndTrackCand fTrackCand;

	Int_t fPidHypo;
	Int_t fFlag;
	Double_t fChi2;
	Int_t fNDF;

public:
	ClassDef(PndTrack,1)

};

#endif /* PNDTRACK_H_ */
