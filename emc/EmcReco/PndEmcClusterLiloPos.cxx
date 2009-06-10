//-----------------------------------------------------------------------
// File and Version Information:
//      $Id: $
//
//  Description:
//	Class PndEmcClusterLiloPos. Lilo is a combined linear/logarithmic digi 
//	weighting algorithm for	cluster position reconstruction. It features 
//	an energy dependent offset parameter and an effective addend 
//	counter. If more than one digi was found in the logarithmic sum for 
//	one angle, this sum is used for this angle. If not, the linear sum 
//	is used which normally includes more digis.
//
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Stephan Otto
// Dima Melnichuk - adaption for PANDA
//		
// Copyright Information:
//	Copyright (C) 2000	TU Dresden
//------------------------------------------------------------------------

#include "PndEmcClusterLiloPos.h"

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

Double_t PndEmcClusterLiloPos::fOffsetParmA=0;
Double_t PndEmcClusterLiloPos::fOffsetParmB=0;
Double_t PndEmcClusterLiloPos::fOffsetParmC=0;
		
PndEmcClusterLiloPos::PndEmcClusterLiloPos( const PndEmcCluster& toUse )
{
}

PndEmcClusterLiloPos::PndEmcClusterLiloPos( const PndEmcClusterLiloPos& other )
{
}

//--------------
// Destructor --
//--------------

PndEmcClusterLiloPos::~PndEmcClusterLiloPos()
{
}

void PndEmcClusterLiloPos::SetParameters(PndEmcClusterLiloPosData ClusterPositionParameters)
{
	fOffsetParmA=ClusterPositionParameters.OffsetParmA;
	fOffsetParmB=ClusterPositionParameters.OffsetParmB;
	fOffsetParmC=ClusterPositionParameters.OffsetParmC;
}


TVector3 
PndEmcClusterLiloPos::liloWhere( const PndEmcCluster *aClus )
{
	const double lClusEnergy=aClus->energy();
	
	assert(lClusEnergy!=0);
	
	const double lOffset=
		fOffsetParmA-fOffsetParmB*exp(-fOffsetParmC*lClusEnergy);
	
	TVector3 lLiloPoint( 1, 1, 1 );
	
	TVector3 lLinSum( 0, 0, 0 );
	TVector3 lLogSum( 0, 0, 0 );
	
	double lLogWeightSum=0;
	int lLogNum=0;
	
	bool lLogSecondTheta  = false;
	bool lLogSecondPhi    = false;
	int  lLogFirstTheta   = -666;
	int  lLogFirstPhi     = -666;
	
	PndEmcDigiPtrDict *lDigis = 
		(PndEmcDigiPtrDict *)aClus->MemberDigiMap();
	
	PndEmcDigiPtrDict::iterator lDigiIter = (*lDigis).begin();
	
	std::map<PndEmcTwoCoordIndex*, PndEmcXtal*> const &tciXtalMap=PndEmcStructure::Instance()->GetTciXtalMap();
	
	while(lDigiIter != (*lDigis).end())
	{
		PndEmcDigi *lDigi=lDigiIter->second;
		PndEmcXtal* xtal = tciXtalMap.find(lDigi->GetTCI())->second;
		const TVector3 tNormal=xtal->normalToFrontFace();
		
		
		// TODO Consider forwars endcup separetly
		//TVector3 tmpPoint=lDigi->where();
		//		if(lDigi->theta() > 3000)
		//tmpPoint += *tNormal * 7.0 * 6.2;
		//else
		//tmpPoint += tNormal * 6.2;

		const TVector3 lDigiWhere=lDigi->where();
		
		const int lDigiTheta=lDigi->GetThetaInt();
		const int lDigiPhi=lDigi->GetPhiInt();
		const double lDigiEnergy=lDigi->GetEnergy();
		const double lLinWeight=lDigiEnergy/lClusEnergy;
		const double lLogWeight=lOffset+log(lLinWeight);
		
		lLinSum+=lLinWeight*lDigiWhere;
		if(lLogWeight>0)
		{
			lLogSum+=lLogWeight*lDigiWhere;
			lLogWeightSum+=lLogWeight;
			lLogNum++;
			
			if(lLogNum==1)
			{ 
					lLogFirstTheta=lDigiTheta;
					lLogFirstPhi=lDigiPhi;
			}
			else
			{
				if(!lLogSecondTheta&&lDigiTheta!=lLogFirstTheta) 
					lLogSecondTheta=true;
				if(!lLogSecondPhi&&lDigiPhi!=lLogFirstPhi) 
					lLogSecondPhi=true;
			}
		}
		++lDigiIter;
	}
  
	if(lLogNum>0) lLogSum*=1./lLogWeightSum;
	
	
	lLiloPoint.SetTheta(lLogSecondTheta?lLogSum.Theta():lLinSum.Theta());
	lLiloPoint.SetPhi(lLogSecondPhi?lLogSum.Phi():lLinSum.Phi());
	
	
	PndEmcTwoCoordIndex *lTCI=PndEmcStructure::Instance()->locateIndex(lLiloPoint.Theta(),lLiloPoint.Phi());
  
	assert(lTCI!=0);  
	
	const PndEmcXtal *lGeom=tciXtalMap.find(lTCI)->second;
	
	// First, find out if the point is outside the crystal.
	
	const TVector3 lLiloVector = lLiloPoint;
	const TVector3 lCentre = lGeom->frontCentre();
	
	const TVector3 lNormal = lGeom->normalToFrontFace();
	const TVector3 lVector = lCentre - lLiloVector;
	const double lLength = lNormal.Dot(lVector);
	//const Hep3Vector lUnit = lLiloVector.unit();
	//const double lMag = (lNormal->dot(lCentre)/lNormal->dot(lUnit));  
	//cout<<" With original code, lilo-position "<<float(lMag)<<endl;

	if (lLength < 0.0)
	{
		// Point is outside crystal (not sure its correct for all cases?). 
		// Anyhow, project point back onto front-face.
		const TVector3 lUnit = lLiloVector.Unit();
		const double lMag = (lNormal.Dot(lCentre)/lNormal.Dot(lUnit));  
		lLiloPoint.SetMag(lMag);
	}
	else
	{
	// Don't project onto front-face. Keep the centroid inside the crytal.
	//cout<<" Point is inside crystal, keep as it is "<<endl;
		if (lLogNum > 1)
		{
			// Use logarithmic centroid position
			lLiloPoint.SetMag(lLogSum.Mag());
			//cout<<" With log centroid method, lilo-position "<<lLiloPoint.Mag()<<endl;
		}
		else
		{
			// Use linear centroid position
			lLiloPoint.SetMag(lLinSum.Mag());
			//cout<<" Use lin centroid method, lilo-position "<<lLiloPoint.Mag()<<endl;
		}
	
	}

	return lLiloPoint;
}

//ClassImp(PndEmcClusterLiloPos)
