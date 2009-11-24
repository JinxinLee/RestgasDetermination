//-----------------------------------------------------------------------
//  File and Version Information:
//    $Id: $
//
//  Description:
//	Class PndEmcClusterLiloPos. Lilo is a combined linear/logarithmic digi 
//	weighting algorithm for	cluster position reconstruction. It features 
//	an energy dependent offset parameter and an effective addend 
//	counter. If more than one digi was found in the logarithmic sum for 
//	one angle, this sum is used for this angle. If not, the linear sum 
//	is used which normally includes more digis.
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
//
//------------------------------------------------------------------------
#pragma once
#ifndef PNDEMCCLUSTERLILOPOS_H
#define PNDEMCCLUSTERLILOPOS_H

#include "TObject.h"
#include "PndEmcDataTypes.h"
#include <vector>
		
class TVector3;
class PndEmcCluster;
class PndEmcTwoCoordIndex;
class PndEmcDigi;


struct PndEmcClusterLiloPosData
{
	Double_t OffsetParmA;
	Double_t OffsetParmB;
	Double_t OffsetParmC;
};

class PndEmcClusterLiloPos
{

public:

  // Constructors
  PndEmcClusterLiloPos( const PndEmcCluster & );
  PndEmcClusterLiloPos( const PndEmcClusterLiloPos & );

  // Destructor

  virtual ~PndEmcClusterLiloPos();

  // Selectors (const)

  static TVector3 liloWhere( const PndEmcCluster * );
  static void SetParameters(PndEmcClusterLiloPosData ClusterPositionParameters);
  
private:
	static Double_t fOffsetParmA;
	static Double_t fOffsetParmB;
	static Double_t fOffsetParmC;

  //ClassDef(PndEmcClusterLiloPos,1)
};

#endif
