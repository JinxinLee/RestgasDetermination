/*
 * PndMvdRadDamHit.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PNDMVDRADDAMHIT_H_
#define PNDMVDRADDAMHIT_H_

#include "TObject.h"
#include "TVector3.h"
class PndMvdRadDamHit : public TObject
{
public:
	PndMvdRadDamHit();
	PndMvdRadDamHit(Int_t trackID, Int_t hitID, TString detName, Int_t pdgCode, Double_t energy,
	  	    TVector3 pos, TVector3 mom, Double_t radDam);
	virtual ~PndMvdRadDamHit(){};

	void SetRadDam(Double_t radDam){fRadDam = radDam;};
	Double_t GetRadDam(){return fRadDam;};

private:
	Double_t fRadDam;
	TString fDetName;
	Int_t fPdgCode;
	Double_t fEnergy;
	Int_t fTrackID;
	Int_t fHitID;
	TVector3 fPos;
	TVector3 fMom;


	ClassDef(PndMvdRadDamHit,2);
};


#endif /* PNDMVDRADDAMHIT_H_ */
