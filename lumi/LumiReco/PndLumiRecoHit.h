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

#include <iostream>
using std::cout;
using std::endl;

class PndLumiRecoHit : public FairHit
{
public:
	/** Default constructor */
	PndLumiRecoHit();

	PndLumiRecoHit(TVector3 MCPos, TVector3 RecoPos);

	virtual ~PndLumiRecoHit(){;}

	TVector3 GetMCPosition() const {return fMCPosition;}
	TVector3 GetRecoPosition() const {return fRecoPosition;}

	virtual void Print(const Option_t* opt = 0) const;

private:
	TVector3 fMCPosition, fRecoPosition;

	ClassDef(PndLumiRecoHit,1);
};

#endif /* PNDLUMIRECOHIT_H_ */
