//-----------------------------------------------------------------------
//  File and Version Information:
//    $Id: $
//
// Description:
//	Class PndEmcClusterLinearPos.
// Linear digi weighting algorithm for	cluster position reconstruction.
// Created for test purpose, not for real cluster position reconstruction.
//
//------------------------------------------------------------------------
#pragma once
#ifndef PNDEMCCLUSTERLINEARPOS_H
#define PNDEMCCLUSTERLINEARPOS_H

#include "TObject.h"
#include <map>
//#include <vector>
		
class TVector3;
class PndEmcCluster;
class PndEmcTwoCoordIndex;
class PndEmcDigi;

typedef std::map<PndEmcTwoCoordIndex*, PndEmcDigi*> PndEmcDigiPtrDict;

class PndEmcClusterLinearPos
{

public:

  // Constructors
  PndEmcClusterLinearPos( const PndEmcCluster & );
  PndEmcClusterLinearPos( const PndEmcClusterLinearPos & );

  // Destructor

  virtual ~PndEmcClusterLinearPos();

  // Selectors (const)

  static TVector3 linearWhere( const PndEmcCluster * );
  
private:

  //ClassDef(PndEmcClusterLinearPos,1)
};

#endif
