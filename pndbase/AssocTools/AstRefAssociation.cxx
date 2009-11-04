//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstRefAssociation.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstRefAssociation
//      Very simple class to provide an association between two
//      pointer types which don't obviously belong together in a class,
//      but need to go together for some reason.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother           Original author
//
// Copyright Information:
//	Copyright (C) 1997      Imperial College
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------
 
//-----------------------
// This Class's Header --
//-----------------------

#include "AstRefAssociation.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------


//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

//----------------
// Constructors --
//----------------


template<class T1, class T2>
AstRefAssociation< T1,  T2>::AstRefAssociation(const T1 &theT1,
					 const T2 &theT2):
  _itsT1(theT1),
  _itsT2(theT2)
{}

// Copy constructor

template<class T1, class T2>
AstRefAssociation< T1,  T2>::AstRefAssociation(const AstRefAssociation<T1, T2> 
					 &other)
{
  if (this != &other){
    _itsT1 = other._itsT1;
    _itsT2 = other._itsT2;
  }
}

//--------------
// Destructor --
//--------------


template<class T1, class T2>
AstRefAssociation< T1,  T2>::~AstRefAssociation()
{}

//-------------
// Methods   --
//-------------
    
//-------------
// Operators --
//-------------


template<class T1, class T2>
AstRefAssociation<T1, T2> &
AstRefAssociation<T1, T2>::operator=(const AstRefAssociation<T1, T2> &other)
{
  if (this != &other){
    _itsT1 = other._itsT1;
    _itsT2 = other._itsT2;
  }
  return *this;
}

template<class T1, class T2>
bool 
AstRefAssociation<T1, T2>::operator==(const AstRefAssociation<T1, T2> &other) const
{
  return (& _itsT1 == &(other._itsT1) && &_itsT2 == &(other._itsT2));
}

template<class T1, class T2>
bool 
AstRefAssociation<T1, T2>::operator!=(const AstRefAssociation<T1, T2> &other) const
{
  return (! (*this==other));
}

    
//-------------
// Selectors --
//-------------
    
//-------------
// Modifiers --
//-------------

//              -----------------------------------------------
//              -- Static Data & Function Member Definitions --
//              -----------------------------------------------

//              -------------------------------------------
//              -- Protected Function Member Definitions --
//              -------------------------------------------

//              -----------------------------------------
//              -- Private Function Member Definitions --
//              -----------------------------------------

//              -----------------------------------
//              -- Internal Function Definitions --
//              -----------------------------------








