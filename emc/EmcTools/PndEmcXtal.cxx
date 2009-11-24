//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcXtal
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//		
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//      Stephen J. Gowdy               University of Edinburgh
// Dima Melnichuk - adaption for PANDA	
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//	Copyright (C) 1996	       CEA - Centre d'Etude de Saclay
//	Copyright (C) 1997	       University of Edinburgh
//------------------------------------------------------------------------
#include "TObject.h"
#include "PndEmcXtal.h"
#include "PndEmcTwoCoordIndex.h"
#include "TGeoArb8.h"
#include "TVector3.h"
#include "TRotation.h"				
#include "TGeoMatrix.h"		
		
//#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//----------------
// Constructors --
//----------------

PndEmcXtal::PndEmcXtal(const PndEmcTwoCoordIndex *id, const TGeoTrap &trap, const TVector3 &pos, const TGeoRotation &rot):
       fTCIIndex(id),
       fTrap(trap)
{
	// calculate length of the crystal
	fLength=trap.GetDz()*2;
	// Obtain unitary vector in the direction of crystal in calorimeter
	Double_t unit[3]={0,0,1};
	Double_t unit_rotated[3];
	// rotate unitary vector
	rot.LocalToMaster(unit,unit_rotated);
	TVector3 unit_vector(unit_rotated[0],unit_rotated[1],unit_rotated[2]);
	fCentre = pos;
	//  case of backward endcup is considered separetly
	Int_t crystal_id = id->Index();
	
	// TODO I suppose that module should be obtained in the way independent on crystal numbering scheme, but it's not clear how at the moment
	Int_t module = crystal_id/100000000;
	
	if (module==4){
		fFrontCentre = pos+0.5*fLength*unit_vector;
		fNormalToFrontFace = -1.*unit_vector;
	} else {
		fFrontCentre = pos-0.5*fLength*unit_vector;
		fNormalToFrontFace = unit_vector;
	}
	
	
}

//--------------
// Destructor --
//--------------

PndEmcXtal::~PndEmcXtal()
{
}


//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

bool
PndEmcXtal::operator==( const PndEmcXtal& compare ) const
{
  bool answer = false;

  if ( *fTCIIndex == *compare.fTCIIndex ) answer=true;

  return answer;
}

bool
PndEmcXtal::operator<( const PndEmcXtal& compare ) const
{
  bool answer = false;

  if ( *fTCIIndex < *compare.fTCIIndex ) answer=true;
  else if ( *fTCIIndex == *compare.fTCIIndex && fLength < compare.fLength) answer=true;

  return answer;
}

const PndEmcTwoCoordIndex*
PndEmcXtal::myIndex() const
{
  return fTCIIndex;
}

const TVector3&
PndEmcXtal::centre() const 
{
  return fCentre;
}

const TVector3&
PndEmcXtal::frontCentre() const 
{
  return fFrontCentre;
}

const TVector3&
PndEmcXtal::normalToFrontFace() const
{
  return fNormalToFrontFace;
}

double
PndEmcXtal::npAngle() const
{
  return fNormalToFrontFace.Theta()-fCentre.Theta();
}

ClassImp(PndEmcXtal)
