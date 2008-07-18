//======================================================================
// File and Version Information:
// 	$Id:$
//
// Class PndEmcTwoCoordIndex
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//		
// Class to hold coordinate and index information
// for system in which two co-ordinates specifies a channel uniquely.
// (Based on class from BABAR framework)
//
//  Author List:
//
//         Phil Strother                         Imperial College
//         Stephen J. Gowdy                      University of Edinburgh
// Dima Melnichuk - adaption for PANDA					
// 
//======================================================================

#include "PndEmcTwoCoordIndex.h"
#include "PndEmcNeighbourStore.h"
#include <iostream>
#include "math.h"

using namespace std;

//----------------
// Constructors --
//----------------
PndEmcTwoCoordIndex::PndEmcTwoCoordIndex()
{
   fCoords[0]=-1;
   fCoords[1]=-1;

   fIndex=-1;
}

PndEmcTwoCoordIndex::PndEmcTwoCoordIndex( long x, long y, long ind )
{
   fCoords[0]=x;
   fCoords[1]=y;
  
   fIndex=ind;
}

//--------------
// Destructor --
//--------------
PndEmcTwoCoordIndex::~PndEmcTwoCoordIndex()
{
}

//-------------
// Methods   --
//-------------

const PndEmcNeighbourStore
PndEmcTwoCoordIndex::GetNeighbours() const
{
   return fNeighbours;
}

void
PndEmcTwoCoordIndex::AddToNeighbourList( PndEmcTwoCoordIndex *addition )
{
  fNeighbours.insert( addition );
}

bool 
PndEmcTwoCoordIndex::IsNeighbour( PndEmcTwoCoordIndex* tci )
{
	
   if( abs(fCoords[0] - tci->XCoord()) > 1 ) return false;
	if (fCoords[1]<200) {
		if((abs(fCoords[1] - tci->YCoord()) > 1)&&(abs(fCoords[1] - tci->YCoord()) !=159 )) return false;
	} else if (abs(fCoords[1] - tci->YCoord()) > 1) {
		return false;
	}
	
   return true;
}

ClassImp(PndEmcTwoCoordIndex)
