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

#ifndef PNDEMCBUMP_HH
#define PNDEMCBUMP_HH

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
	virtual void madeFrom( PndEmcCluster* thisCluster );
	virtual double energy() const;

protected:
  // Data members
  const PndEmcCluster *_madeFrom; //! transient element


	ClassDef(PndEmcBump,1)
};

#endif // PNDEMCBUMP_HH
