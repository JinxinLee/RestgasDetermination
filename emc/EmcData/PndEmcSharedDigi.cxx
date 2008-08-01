//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class Template for EmcSharedDigi. Inherits from EmcDigi
//           
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Stephen J. Gowdy           University of Edinburgh
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcSharedDigi.h"
		
#include <iostream>

//----------------
// Constructors --
//----------------
PndEmcSharedDigi::PndEmcSharedDigi( const PndEmcDigi &theDigi, const double weight )
  : PndEmcDigi( theDigi ),
    fWeight( weight ),
    fDigi( (PndEmcDigi*)&theDigi )
{
}

PndEmcSharedDigi::PndEmcSharedDigi( const PndEmcSharedDigi &theDigi )
  : PndEmcDigi( theDigi ),
    fWeight( theDigi.fWeight ),
    fDigi( theDigi.fDigi )
{
}

PndEmcSharedDigi::PndEmcSharedDigi()
{}
//--------------
// Destructor --
//--------------

PndEmcSharedDigi::~PndEmcSharedDigi()
{
}

PndEmcDigi*
PndEmcSharedDigi::clone() const
{
  return new PndEmcSharedDigi(*this);
}

//-------------
// Operators --
//-------------
void
PndEmcSharedDigi::weight( double newWeight )
{
  fWeight = newWeight;
}

//-------------
// Selectors --
//-------------

PndEmcSharedDigi* 
PndEmcSharedDigi::dynamic_cast_PndEmcSharedDigi()
{
  return this;
}

const PndEmcSharedDigi* 
PndEmcSharedDigi::dynamic_cast_PndEmcSharedDigi() const
{
  return this;
}

ClassImp(PndEmcSharedDigi)
