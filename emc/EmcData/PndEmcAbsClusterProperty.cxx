//--------------------------------------------------------------------------
// File and Version Information:
//
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
PndEmcAbsClusterProperty::PndEmcAbsClusterProperty( const PndEmcCluster& toUse )
  : fMyCluster( toUse ),
    fMembers( *(toUse.MemberDigiMap()) )
{
}

PndEmcAbsClusterProperty::PndEmcAbsClusterProperty( const PndEmcAbsClusterProperty& t )
  : fMyCluster( t.fMyCluster ),
    fMembers( t.fMembers )
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

PndEmcDigiPtrDict&
PndEmcAbsClusterProperty::Members() const
{
  return (PndEmcDigiPtrDict&)fMembers;
}
