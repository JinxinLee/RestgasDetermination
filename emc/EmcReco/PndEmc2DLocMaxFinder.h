//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//	Class EmcERatioLocMaxMaxFinder./
//      Searches for local maxima in a cluster based on the ratio
//      between the energy of the maxima crystal and that of 
//      its neighbours 
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother         
//      Helmut Schmuecker                   Ruhr-Uni Bochum      
//
// Copyright Information:
//	Copyright (C) 1997	            Imperial College
// Modified:
// M. Babai
//------------------------------------------------------------------------

#ifndef PNDEMC2DLOCALMAXFINDER_HH
#define PNDEMC2DLOCALMAXFINDER_HH

#include <set>
#include <map>
		
#include "TObject.h"

class PndEmcDigi;
class PndEmcCluster;
class PndEmcTwoCoordIndex;
class PndEmcRecoPar;

struct PndEmc2DLocMaxFinderData
{
  Double_t MaxECut;
  Double_t NeighbourECut;
  Double_t CutSlope;
  Double_t CutOffset;
  Double_t ERatioCorr;
  Int_t    TheNeighbourLevel;
};

class PndEmc2DLocMaxFinder{
  
  typedef std::set<PndEmcTwoCoordIndex*> EmcCoordIndexSet;
  typedef std::map<PndEmcTwoCoordIndex*, PndEmcDigi*> EmcDigiPtrDict;
  
 public:
  
  // Constructors
  
  PndEmc2DLocMaxFinder(PndEmc2DLocMaxFinderData locMaxData,Int_t verbose=0);
  // Copy Constructor
  //PndEmc2DLocMaxFinder( const PndEmc2DLocMaxFinder& other);
  
  // Destructor
  virtual ~PndEmc2DLocMaxFinder();
  
  // Operators
  //PndEmc2DLocMaxFinder & operator= ( const PndEmc2DLocMaxFinder& other);
  
  // Methods 
  void findMaxima( const PndEmcCluster * const theCluster, 
		   std::set<PndEmcTwoCoordIndex*>& res) const;
  
  // Modifiers
  
 protected:
  
  // Helper functions
  virtual bool isALocalMax( const PndEmcDigi *const, const PndEmcCluster * const, 
			    const EmcCoordIndexSet &amongstTheseNeighbours ) const;
  
  // Test to see whether the given Digi is a local maxima, within the set of
  // crystals given in "amongstTheseNeighbours".  Note that this set need
  // all be crystals that actually were within the cluster.
  
 private:
  // Copy Constructor
  PndEmc2DLocMaxFinder( const PndEmc2DLocMaxFinder& other);
  // Methods
  void getNeighbourDigis( EmcCoordIndexSet &, EmcCoordIndexSet &, int,
			  const EmcDigiPtrDict* const) const;
  
  PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
  
  // Data members
  Double_t fMaxECut;
  Double_t fNeighbourECut;
  Double_t fCutSlope;
  Double_t fCutOffset;
  Double_t fERatioCorr;
  Int_t fTheNeighbourLevel;
  /** Verbosity level **/
  Int_t fVerbose;
};
#endif // PNDEMC2DLOCALMAXFINDER_HH
