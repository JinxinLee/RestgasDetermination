/*
 * PndLumiRecoHit.h
 *
 *  Created on: Nov 10, 2008
 *      Author: tsito
 */

#ifndef PNDLUMIRECOHIT_H_
#define PNDLUMIRECOHIT_H_

#include "FairHit.h"

#include "TVector3.h"

class PndLumiRecoHit : public FairHit
{
public:
	/** Default constructor */
	PndLumiRecoHit();

	PndLumiRecoHit(TVector3 MCPos, TVector3 RecoPos,
			Double_t RecoEnergy, Int_t nhits);

	virtual ~PndLumiRecoHit(){;}

	TVector3 GetMCPosition() const {return fMCPosition;}
	TVector3 GetRecoPosition() const {return fRecoPosition;}
	Double_t GetRecoEnergy() const {return fRecoEnergy;}
	Int_t GetHitNumber() const {return fNHits;}

private:
	TVector3 fMCPosition, fRecoPosition;
	Double_t fRecoEnergy;
	Int_t fNHits;

	ClassDef(PndLumiRecoHit,1);
};

#endif /* PNDLUMIRECOHIT_H_ */
