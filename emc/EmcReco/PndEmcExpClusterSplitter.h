//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcExpClusterSplitter.
//      Concrete implementation of EmcAbsClusterSplitter which splits
//      on the basis of exponential distance from the bump centroid.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother               
//
// Copyright Information:
//	Copyright (C) 1997               Imperial College
//
// Modified:
// M. Babai
//------------------------------------------------------------------------
#pragma once
#ifndef PNDEMCEXPCLUSTERSPLITTER_H
#define PNDEMCEXPCLUSTERSPLITTER_H

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <map>

#include "TObject.h"
#include "PndEmcDataTypes.h"

struct PndEmcExpClusterSplitterData
{
  Double_t MoliereRadius; 
  Double_t ExponentialConstant;
  Int_t    MaxIterations;
  Double_t CentroidShift;
  Int_t    MaxBumps;
  Double_t MinDigiEnergy;
};

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class PndEmcCluster;
class PndEmcBump;
class PndEmcDigi;
class PndEmcTwoCoordIndex;

//		---------------------
// 		-- Class Interface --
//		---------------------

class PndEmcExpClusterSplitter{
  
 public:
  
  PndEmcExpClusterSplitter(PndEmcExpClusterSplitterData  expClusterSplitterData,Int_t verbose=0);
  
  // Destructor
  virtual ~PndEmcExpClusterSplitter( );
  
  // Methods
  
  virtual void splitCluster(const PndEmcCoordIndexSet &, 
			    const PndEmcCluster* const, Int_t clusterIndex, 
			    std::vector<PndEmcBump*>& theBumpList) const;
  
 private:
  //These methods are not implemented yet. Maybe wise to do so, ???
  // Copy Constructor
  PndEmcExpClusterSplitter( const PndEmcExpClusterSplitter& other);
  PndEmcExpClusterSplitter& operator= ( const PndEmcExpClusterSplitter& other);
  
  // Data members
  Double_t fMoliereRadius;
  Double_t fExponentialConstant;
  Int_t fMaxIterations;
  Double_t fCentroidShift;
  Int_t fMaxBumps;
  Double_t fMinDigiEnergy;
  
  /** Verbosity level **/
  Int_t fVerbose;
};
#endif // EMCABSCLUSTERSPLITTER_HH
