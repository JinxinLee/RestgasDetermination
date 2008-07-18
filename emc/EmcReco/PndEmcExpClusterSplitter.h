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
//------------------------------------------------------------------------

#ifndef PNDEMCEXPCLUSTERSPLITTER_HH
#define PNDEMCEXPCLUSTERSPLITTER_HH

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <set>
#include <map>

#include "TObject.h"

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
	typedef std::map<PndEmcTwoCoordIndex*, PndEmcDigi*> EmcDigiPtrDict;
  
public:
  
  PndEmcExpClusterSplitter(PndEmcExpClusterSplitterData  expClusterSplitterData,Int_t verbose=0);

  // Destructor
  virtual ~PndEmcExpClusterSplitter( );
  
  // Methods
  
  virtual void splitCluster(const std::set<PndEmcTwoCoordIndex*> &, const PndEmcCluster * const,
			    std::vector<PndEmcBump*> &) const;
  
private:
  
	// Copy Constructor
	PndEmcExpClusterSplitter( const PndEmcExpClusterSplitter& );
	
	PndEmcExpClusterSplitter&       operator= ( const PndEmcExpClusterSplitter& );
	
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
