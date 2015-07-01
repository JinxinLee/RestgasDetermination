//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class PndEmcClusterEnergySums.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Phil Strother          
//	Helmut Marsiske
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
// Dima Melnichuk, adaption for pandaroot
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCLUSTERENERGYSUMS_H
#define PNDEMCCLUSTERENERGYSUMS_H

#include "PndEmcAbsClusterProperty.h"
#include <iostream>
#include "TObject.h"

class TVector3;

//		---------------------
// 		-- Class Interface --
//		---------------------

class PndEmcClusterEnergySums: public PndEmcAbsClusterProperty
{


public:

  // Constructors
  PndEmcClusterEnergySums(const PndEmcCluster &cluster, const TClonesArray *digiArray);
  PndEmcClusterEnergySums(const PndEmcClusterEnergySums &);

  // Destructor
  virtual ~PndEmcClusterEnergySums();

	// Partial Energy of Cluster
	virtual Double_t energy( Int_t numberOfDigis ) const;
	virtual Double_t energy( Double_t distance ) const;
	virtual Double_t E1() const;
	virtual Double_t E9() const;
	virtual Double_t E25() const;
	
	// Energy ratios of Cluster
	virtual Double_t E1E9() const;
	virtual Double_t E9E25() const;
	
	// Printing
	virtual void Print(const Option_t* opt ="") const;

private:

	PndEmcClusterEnergySums();
	PndEmcClusterEnergySums &operator=(const PndEmcClusterEnergySums &);
	
	ClassDef(PndEmcClusterEnergySums,1)
};
#endif // PNDEMCCLUSTERENERGYSUMS_H
