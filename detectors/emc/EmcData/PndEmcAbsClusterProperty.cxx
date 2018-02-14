//--------------------------------------------------------------------------
// Description:
//	Class PndEmcAbsClusterProperty
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Phil Strother   	Originator
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
//------------------------------------------------------------------------

#include "PndEmcAbsClusterProperty.h"
#include "PndEmcCluster.h"

//----------------
// Constructors --
//----------------
PndEmcAbsClusterProperty::PndEmcAbsClusterProperty( const PndEmcCluster& toUse, const TClonesArray* digiArray)
  : fMyCluster( toUse ),
    fMembers(toUse.MemberDigiMap()),
    fDigiArray(digiArray)
{
}

//--------------
// Destructor --
//--------------
PndEmcAbsClusterProperty::~PndEmcAbsClusterProperty()
{
}

const PndEmcCluster&
PndEmcAbsClusterProperty::MyCluster() const
{
  return fMyCluster;
}

const std::map<Int_t,Int_t>&
PndEmcAbsClusterProperty::Members() const
{
  return fMembers;
}
  
const TClonesArray* PndEmcAbsClusterProperty::DigiArray() const
{
	return fDigiArray;
}

