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
	virtual ~PndTrack();

	void Print(){};


	 /** Public method SortHits
	  ** Sorts the hits in downstream direction
	  **/
	Int_t GetPidHypo()               const { return fPidHypo; }
	Int_t GetFlag()                  const { return fFlag; }		//Quality flag
	Double_t GetChi2()               const { return fChi2; }
	Int_t GetNDF()                   const { return fNDF; }
	FairTrackPar* GetParamFirst() { return &fTrackParamFirst; }
	FairTrackPar* GetParamLast()  { return &fTrackParamLast ; }



private:
	FairTrackPar fTrackParamFirst;
	FairTrackPar fTrackParamLast;
	PndTrackCand fTrackCand;

	Int_t fPidHypo;
	Int_t fFlag;
	Int_t fChi2;
	Int_t fNDF;

};

#endif /* PNDTRACK_H_ */
