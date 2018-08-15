/*
 * PndHelixPropagator.h
 *
 *  Created on: Sep 23, 2013
 *      Author: stockman
 */

#ifndef PNDHELIXPROPAGATOR_H_
#define PNDHELIXPROPAGATOR_H_

#include "TObject.h"
#include "TVector3.h"
#include "FairTrackPar.h"
#include <utility>

class PndHelixPropagator : public TObject {
public:
	PndHelixPropagator();
	PndHelixPropagator(Double_t fieldStrength, TVector3 origin, TVector3 momentum, Double_t charge):
		fFieldStrength(fieldStrength), fOrigin(origin), fMomentum(momentum), fCharge(charge)
	{};
	virtual ~PndHelixPropagator();
	FairTrackPar PropagateToZ(Double_t zPos);		//< returns position and momentum at requested z-Value [cm]
	FairTrackPar PropagateToS(Double_t arcLength); 
	FairTrackPar PropagateByAngle(Double_t step); //< angle step in degrees from start position/momentum
	FairTrackPar PropagateToXYPos(TVector2 xyPos); //< propagate to closes distance to given xy position

private:
	Double_t Radius();

	Double_t fFieldStrength; //< magentic field strength in z-Direction in Tesla
	TVector3 fOrigin;		 //< Origin of track in cm
	TVector3 fMomentum;		 //< Momentum of track in GeV
	Double_t fCharge;		 //< Sign of charge
public:
  ClassDef(PndHelixPropagator,1)
};

#endif /* PNDHELIXPROPAGATOR_H_ */
