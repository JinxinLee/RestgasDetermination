/*
 * This returns to the strips which are fired by the track.
 * Energy deposited at each strip is calculated according
 * to the path length that it contains (simple proportionality)
 * Work is being done by PndLumiStripHitProducer using PndLumiCalcStrip
 *
 */

#ifndef PNDLUMISTRIPHIT_H_
#define PNDLUMISTRIPHIT_H_

#include "FairHit.h"

#include "PndLumiCalcStrip.h"
#include "PndLumiStrip.h"

#include "TVector3.h"


class PndLumiStripHit : public FairHit
{
public:
	PndLumiStripHit();

	PndLumiStripHit( Int_t detID, TVector3& entrypos, TVector3& dpos,
			Int_t index, TVector3& exitpos, Int_t multi, PndLumiStrip strip,
			Double_t eLoss);

	virtual ~PndLumiStripHit();

	Int_t GetMultiplicity() const {return fMultiplicity;}
	PndLumiStrip GetStrip() const {return fStrip;}
	Double_t GetEnergyLoss() const {return fELoss;}

private:
	Int_t fMultiplicity;
	Double_t fELoss;
	PndLumiStrip fStrip;


	ClassDef(PndLumiStripHit,1);

};

#endif /*PNDLUMISTRIPHIT_H_*/
