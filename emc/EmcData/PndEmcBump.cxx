//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcBump
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//	Stephen J. Gowdy	Originator
// Copyright Information:
//	Copyright (C) 1997	University of Edinburgh
//
// Dima Melnychuk, adaption for PANDA
//------------------------------------------------------------------------


//-----------------------
// This Class's Header --
//-----------------------

#include "PndEmcBump.h"

//---------------
// C++ Headers --
//---------------

#include <iostream>
#include <iomanip>
#include <vector>
using std::vector;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "PndEmcSharedDigi.h"
using std::endl;
using std::ios;
using std::ostream;
using std::setw;

//----------------
// Constructors --
//----------------

PndEmcBump::PndEmcBump()
  : _madeFrom( 0 )
{
  setNBumps(1);
}

//Copy
PndEmcBump::PndEmcBump(const PndEmcBump &copy)
{
	_madeFrom=copy._madeFrom;
	_nbumps=copy._nbumps;
  
	fLocalMaxList=copy.fLocalMaxList;
	fDigiList=copy.fDigiList;
	fMemberDigiMap=copy.fMemberDigiMap;
	fEnergyValid=copy.fEnergyValid;
	fEnergy=copy.fEnergy;
	fWhereValid=copy.fWhereValid;
	fWhere=copy.fWhere;
}

//--------------
// Destructor --
//--------------

PndEmcBump::~PndEmcBump()
{
}

//-------------
// Modifiers --
//-------------

void
PndEmcBump::madeFrom( PndEmcCluster* thisCluster )
{
  _madeFrom = thisCluster;
}

double
PndEmcBump::energy() const
{
	Double_t weight=0;
	if ( ! fEnergyValid )
	{
		double sum=0;
      std::vector<PndEmcDigi*>::const_iterator digi_iter;

      for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
		{
			weight=(*digi_iter)->dynamic_cast_PndEmcSharedDigi()->weight();
			sum+=(*digi_iter)->GetEnergy()*weight;
		}
		
      fEnergy = sum;
      fEnergyValid = true;
    }
  
  return fEnergy;
}
