/*
 * PndLumiRecoHit.cxx
 *
 *  Created on: Nov 10, 2008
 *      Author: tsito
 */

#include "PndLumiRecoHit.h"

PndLumiRecoHit::PndLumiRecoHit()
{

}

PndLumiRecoHit::PndLumiRecoHit(TVector3 MCPos, TVector3 RecoPos,
		Double_t RecoEnergy, Int_t nhits)
{
	fRecoEnergy = RecoEnergy;
	fMCPosition = MCPos;
	fRecoPosition = RecoPos;
	fNHits = nhits;
}
ClassImp(PndLumiRecoHit)
