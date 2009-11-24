//-----------------------------------------------------------------------
// File and Version Information:
//      $Id: $
//
// Description:
//	Class PndEmcClusterLinearPos.
// Linear digi weighting algorithm for	cluster position reconstruction.
// Created for test purpose, not for real cluster position reconstruction.
//
//------------------------------------------------------------------------

#include "PndEmcClusterLinearPos.h"

#include "PndEmcTwoCoordIndex.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcXtal.h"		
#include "PndEmcStructure.h"		
#include "TVector3.h"
#include "TROOT.h"

#include <iostream>
#include <fstream>				
#include "assert.h"

PndEmcClusterLinearPos::PndEmcClusterLinearPos( const PndEmcCluster& toUse )
{
}

PndEmcClusterLinearPos::PndEmcClusterLinearPos( const PndEmcClusterLinearPos& other )
{
}

//--------------
// Destructor --
//--------------

PndEmcClusterLinearPos::~PndEmcClusterLinearPos()
{
}


TVector3 
PndEmcClusterLinearPos::linearWhere( const PndEmcCluster *aClus )
{
	const double lClusEnergy=aClus->energy();
	
	assert(lClusEnergy!=0);
	
	TVector3 lLinSum( 0, 0, 0 );

	PndEmcDigiPtrDict *lDigis = 
		(PndEmcDigiPtrDict *)aClus->MemberDigiMap();
	
	PndEmcDigiPtrDict::iterator lDigiIter = (*lDigis).begin();
	
	PndEmcTciXtalMap const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	
	while(lDigiIter != (*lDigis).end())
	{
		PndEmcDigi *lDigi=lDigiIter->second;
		PndEmcXtal* xtal = tciXtalMap.find(lDigi->GetTCI())->second;
		
		const TVector3 lDigiWhere=lDigi->where();
		
		const double lDigiEnergy=lDigi->GetEnergy();
		const double lLinWeight=lDigiEnergy/lClusEnergy;
		
		lLinSum+=lLinWeight*lDigiWhere;
		++lDigiIter;
	}
  
	PndEmcTwoCoordIndex *lTCI=PndEmcStructure::Instance()->locateIndex(lLinSum.Theta(),lLinSum.Phi());
  
	assert(lTCI!=0);  
	
	const PndEmcXtal *lGeom=tciXtalMap.find(lTCI)->second;
	
	// First, find out if the point is outside the crystal.

	const TVector3 lCentre = lGeom->frontCentre();
	const TVector3 lNormal = lGeom->normalToFrontFace();
	const TVector3 lVector = lCentre - lLinSum;
	const double lLength = lNormal.Dot(lVector);

	if (lLength < 0.0)
	{
		// Point is outside crystal
		// Project point back onto front-face.
		const TVector3 lUnit = lLinSum.Unit();
		const double lMag = (lNormal.Dot(lCentre)/lNormal.Dot(lUnit));  
		lLinSum.SetMag(lMag);
	}

	return lLinSum;
}

//ClassImp(PndEmcClusterLinearPos)
