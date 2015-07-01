//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Software developed for the PANDA Detector at GSI.
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCRECOHIT_H
#define PNDEMCRECOHIT_H

//----------------------
// Base Class Headers --
//----------------------
#include "FairMultiLinkedData.h" 
#include "TObject.h"
#include "TVector3.h"

class PndEmcRecoHit: public FairMultiLinkedData
{

public:

	// Constructors 
	PndEmcRecoHit();
	PndEmcRecoHit(Double_t energy, TVector3 position);
	
	// Destructor
	virtual ~PndEmcRecoHit();
	
	// Modifiers
	Double_t GetEnergy() const;
	Double_t GetEnergyCorrected() const;
	TVector3 GetPosition() const;

private:
	// Data members
	Double_t fEnergy;
	Double_t fEnergyCorrected;
	TVector3 fPosition;

	ClassDef(PndEmcRecoHit,1)
};

#endif // PNDEMCRECOHIT_H
