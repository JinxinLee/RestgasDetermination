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
#include "FairTrackPar.h"


class PndTrack : public TObject{
public:
	PndTrack();
	//use pointers because of polymorphic use od FairTrackParP/H and not copy the candidate all the time
	//ownership of first,last, and cand goes to PndTrack
	PndTrack(FairTrackPar* first, FairTrackPar* last, PndTrackCand* cand);
 private:
	/*
	  I can not make a consistent copy ctor, because the polymorphic
	  FairTrackPar do not have a cloning machanism
	 */
	PndTrack(const PndTrack&){}
 public:
	virtual ~PndTrack();

	void Print();


	 /** Public method SortHits
	  ** Sorts the hits in downstream direction
	  **/
	Int_t GetPidHypo()               const { return fPidHypo; }
	Int_t GetFlag()                  const { return fFlag; } //Quality flag
	Double_t GetChi2()               const { return fChi2; }
	Int_t GetNDF()                   const { return fNDF; }
	void SetPidHypo(Int_t i)         { fPidHypo=i; }
	void SetFlag(Int_t i)            { fFlag=i; }
	void SetChi2(Double_t d)         { fChi2=d; }
	void SetNDF(Int_t i)             { fNDF=i; }
	FairTrackPar* GetParamFirst() { return fTrackParamFirst; }
	FairTrackPar* GetParamLast()  { return fTrackParamLast ; }

private:
	//use pointers because of polymorphic use od FairTrackParP/H
	FairTrackPar* fTrackParamFirst;
	FairTrackPar* fTrackParamLast;

	PndTrackCand* fTrackCand;

	Int_t fPidHypo;
	Int_t fFlag;
	Double_t fChi2;
	Int_t fNDF;

public:
	ClassDef(PndTrack,1)

};

#endif /* PNDTRACK_H_ */
