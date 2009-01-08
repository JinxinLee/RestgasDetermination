//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcBump. This call provides a standard interface for
//          Emc bump classes.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Stephen J. Gowdy	Originator
// Copyright Information:
//	Copyright (C) 1997	University of Edinburgh
//
// Dima Melnychuk, adaption for PANDA
//------------------------------------------------------------------------
#pragma once
#ifndef PNDEMCBUMP_H
#define PNDEMCBUMP_H

//----------------------
// Base Class Headers --
//----------------------
#include "PndEmcCluster.h"
#include "TObject.h"

class PndEmcBump: public PndEmcCluster
{

public:

	// Constructors
	PndEmcBump();
	
	// Destructor
	virtual ~PndEmcBump();
	
	/** Copy **/
	PndEmcBump(const PndEmcBump &);

	// Modifiers
	virtual void MadeFrom( Int_t clusterIndex );
	virtual Double_t energy() const;
	virtual Int_t NumberOfDigis() const;
	virtual Double_t RnumberOfDigis() const;


protected:
  // Data members
  Int_t  fClusterIndex; // Index of cluster the bump is made in TClonesArray


	ClassDef(PndEmcBump,1)
};
#endif // PNDEMCBUMP_HH
