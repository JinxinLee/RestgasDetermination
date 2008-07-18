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
    _weight( weight ),
    _digi( (PndEmcDigi*)&theDigi )
{
}

PndEmcSharedDigi::PndEmcSharedDigi( const PndEmcSharedDigi &theDigi )
  : PndEmcDigi( theDigi ),
    _weight( theDigi._weight ),
    _digi( theDigi._digi )
{
}

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
  _weight = newWeight;
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
