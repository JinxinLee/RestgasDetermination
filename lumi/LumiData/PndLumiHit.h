#ifndef PNDLUMIHIT_H
#define PNDLUMIHIT_H 1

#include "FairHit.h"

#include "TVector3.h"
#include <iostream>
using std::cout;
using std::endl;

class PndLumiHit : public FairHit
{
public:
	/** Default constructor */
	PndLumiHit();

	/** Standard constructor
	 * 	\param detID     Detector unique volume ID
      	\param pos       Position coordinates [cm]
      	\param dpos      Errors in position coordinates [cm]
      	\param index     Index of corresponding MCPoint
      	\param hitLost   Flag is true whenever hit was lost by inefficiency
	 */
	PndLumiHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t index,
			Double_t eloss, Int_t hitLost);

	/** Destructor */
	virtual ~PndLumiHit();

	/** Output to screen */
	virtual void Print(const Option_t* opt = 0) const;

	/** Accessors apart from ones which have been inherited from FairHit*/
	Double_t GetEnergyLoss() const {return fELoss;}

	  /** Modifiers */
	void LoseHit(Bool_t newState = kTRUE) {fWasLost = newState;}

private:

	Bool_t fWasLost;
	Double_t fELoss;

	ClassDef(PndLumiHit,1);
};
#endif
