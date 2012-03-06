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
#pragma once
#ifndef PNDEMCABSCLUSTERPROPERTY_H
#define PNDEMCABSCLUSTERPROPERTY_H

#include "TObject.h"
#include "PndEmcCluster.h"

class PndEmcCluster;
class PndEmcDigi;
class PndEmcTwoCoordIndex;
class TClonesArray;

//		---------------------
// 		-- Class Interface --
//		---------------------

class PndEmcAbsClusterProperty: public TObject
{

public:

  // Constructors
  PndEmcAbsClusterProperty(const PndEmcCluster &cluster, const TClonesArray *digiArray);

  // Destructor
  virtual ~PndEmcAbsClusterProperty();

protected:

  // Helper functions
  //Possible memory problems (FIXME)
  const PndEmcCluster& MyCluster()const;
  const std::map<Int_t,Int_t>& Members() const; // Map <detId,digiIndex>
  const TClonesArray* DigiArray() const;

  PndEmcAbsClusterProperty(const  PndEmcAbsClusterProperty& L);
  PndEmcAbsClusterProperty &operator=(const PndEmcAbsClusterProperty&) {return *this;};

private:

//  PndEmcAbsClusterProperty(const  PndEmcAbsClusterProperty& L);
//  PndEmcAbsClusterProperty &operator=(const PndEmcAbsClusterProperty&) {return *this;};
//  PndEmc& operator= (const  PndEmc&) {return *this;};
  // Friends

  // Data members
  const PndEmcCluster& fMyCluster;
  const std::map<Int_t,Int_t>& fMembers; // Map <detId,digiIndex>
  const TClonesArray* fDigiArray;


};

#endif // PNDEMCABSCLUSTERPROPERTY_HH
