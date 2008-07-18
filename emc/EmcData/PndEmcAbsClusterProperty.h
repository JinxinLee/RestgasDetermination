//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class EmcAbsClusterProperty.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Phil Strother   	Originator
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
//------------------------------------------------------------------------

#ifndef PNDEMCABSCLUSTERPROPERTY_HH
#define PNDEMCABSCLUSTERPROPERTY_HH

#include "TObject.h"
#include <map>

class PndEmcCluster;
class PndEmcDigi;
class PndEmcTwoCoordIndex;

typedef std::map<PndEmcTwoCoordIndex*, PndEmcDigi*> PndEmcDigiPtrDict;
//		---------------------
// 		-- Class Interface --
//		---------------------

class PndEmcAbsClusterProperty: public TObject
{

public:

  // Constructors
  PndEmcAbsClusterProperty( const PndEmcCluster& );
  PndEmcAbsClusterProperty( const PndEmcAbsClusterProperty& );

  // Destructor
  virtual ~PndEmcAbsClusterProperty();

protected:

  // Helper functions
  const PndEmcCluster& MyCluster() const;
  PndEmcDigiPtrDict& Members() const;

private:

  PndEmcAbsClusterProperty &operator=(const PndEmcAbsClusterProperty&);
  // Friends

  // Data members
  const PndEmcCluster& fMyCluster;
  const PndEmcDigiPtrDict& fMembers;


};

#endif // PNDEMCABSCLUSTERPROPERTY_HH
