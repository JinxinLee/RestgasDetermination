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

#include "FairTask.h"
#include "TObject.h"
#include "PndEmcDataTypes.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class PndEmcCluster;
class PndEmcBump;
class PndEmcDigi;
class PndEmcSharedDigi;
class PndEmcTwoCoordIndex;

class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;

//		---------------------
// 		-- Class Interface --
//		---------------------

class PndEmcExpClusterSplitter: public FairTask
{
  
 public:
  
	PndEmcExpClusterSplitter(Int_t verbose=0);
	
	// Destructor
	virtual ~PndEmcExpClusterSplitter( );
	
	// Methods
	/** Virtual method Init **/
	virtual InitStatus Init();
	
	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	
	void SetStorageOfData(Bool_t p = kTRUE) {fPersistance=p;};
	PndEmcBump* AddBump();
	PndEmcSharedDigi* AddSharedDigi(PndEmcDigi*, Double_t weight);
	
  
 private:
	/** Input array of PndEmcClusters **/
	TClonesArray* fDigiArray;
	TClonesArray* fClusterArray;
	
	/** Output array of PndEmcBumps **/
	TClonesArray* fBumpArray;
	TClonesArray* fSharedDigiArray;

	PndEmcGeoPar*     fGeoPar;       /** Geometry parameter container **/
	PndEmcDigiPar*    fDigiPar;      /** Digitisation parameter container **/
	PndEmcRecoPar*    fRecoPar;      /** Reconstruction parameter container **/
	/** Get parameter containers **/
	virtual void SetParContainers();
	
	std::vector<Double_t> fClusterPosParam;
	
	Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
	// Data members
	Double_t fMoliereRadius;
	Double_t fMoliereRadiusShashlyk;
	Double_t fExponentialConstant;
	Int_t fMaxIterations;
	Double_t fCentroidShift;
	Int_t fMaxBumps;
	Double_t fMinDigiEnergy;
	
	/** Verbosity level **/
	Int_t fVerbose;
	
	ClassDef(PndEmcExpClusterSplitter,1);
};
#endif // EMCABSCLUSTERSPLITTER_HH
