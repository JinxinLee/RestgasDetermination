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
  : fClusterIndex( 0 )
{
  SetNBumps(1);
}

//Copy
PndEmcBump::PndEmcBump(const PndEmcBump &copy): fClusterIndex(copy.fClusterIndex)
{
	fClusterIndex=copy.fClusterIndex;
	fNbumps=copy.fNbumps;
  
	fLocalMaxMap=copy.fLocalMaxMap;
	fDigiList=copy.fDigiList;
	fMemberDigiMap=copy.fMemberDigiMap;
	fEnergyValid=copy.fEnergyValid;
	fEnergy=copy.fEnergy;
	fWhereValid=copy.fWhereValid;
	fWhere=copy.fWhere;

	fTimeStamp  = copy.fTimeStamp;
	fEvtNo = copy.fEvtNo;
	//fTimeStamp1 = copy.fTimeStamp1;
	//fSeedPosition = copy.fSeedPosition;
	//fTimeStamp2 = copy.fTimeStamp2;
	//fTimeStamp3 = copy.fTimeStamp3;
}

//--------------
// Destructor --
//--------------

PndEmcBump::~PndEmcBump()
{
}

void
PndEmcBump::Print(Int_t iBump) const
{
	std::cout<<"Bump["<<iBump<<"], (digis, E)=("<<DigiList().size()<<", "<<energy()<<")"<<endl;
}
//-------------
// Modifiers --
//-------------

void
PndEmcBump::MadeFrom( Int_t clusterIndex )
{
  fClusterIndex = clusterIndex;
}


// Double_t
// PndEmcBump::RnumberOfDigis() const
// {
// 	Double_t sum = 0;
// 	PndEmcDigi* current;
// 	
// 	std::vector<PndEmcDigi*>::const_iterator digi_iter;
// 	for (digi_iter=fDigiList.begin();digi_iter!=fDigiList.end();++digi_iter)
// 	{
// 		sum+=(*digi_iter)->dynamic_cast_PndEmcSharedDigi()->weight();
// 	}
// 		
// 	return sum;
// }
// 
// Int_t
// PndEmcBump::NumberOfDigis() const
// {
// 	Int_t numberOfDigis = Int_t( RnumberOfDigis() + 0.5 );
// 	if( numberOfDigis < 1 ) numberOfDigis = 1;
// 	
// 	return( numberOfDigis );
// }
