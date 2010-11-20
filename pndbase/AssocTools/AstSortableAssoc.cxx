//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSortableAssoc.cc,v 1.2 2006/06/30 11:24:22 steinke Exp $
//
// Description:
//	Class AstSortableAssoc
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
//	Copyright (C) 1999      LBNL
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------
 
//-----------------------
// This Class's Header --
//-----------------------

#include "AstSortableAssoc.h"

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


template<class T, class Q>
AstSortableAssoc<T, Q>::AstSortableAssoc(T *theT, Q *theQ, 
 					 bool (*lessThan)(const Q &, const Q&)):
 					 _lessThanFunction(lessThan),
 					 _myT(theT),
 					 _myQ(theQ)
{}

// Copy constructor

template<class T, class Q>
AstSortableAssoc< T,  Q>::AstSortableAssoc(const AstSortableAssoc<T, Q> 
					   &other)
{
  if (this != &other){
    _myT = other._myT;
    _myQ = other._myQ;
    AstSortableAssoc< T,  Q>::_lessThanFunc = other._lessThanFunc;
  }
}

//--------------
// Destructor --
//--------------


template<class T, class Q>
AstSortableAssoc< T,  Q>::~AstSortableAssoc()
{}

//-------------
// Methods   --
//-------------
    
//-------------
// Operators --
//-------------


template<class T, class Q>
AstSortableAssoc<T, Q> &
AstSortableAssoc<T, Q>::operator=(const AstSortableAssoc<T, Q> &other)
{
  if (this != &other){
    _myT = other._myT;
    _myQ = other._myQ;
  }
  return *this;
}

template<class T, class Q>
bool 
AstSortableAssoc<T, Q>::operator==(const AstSortableAssoc<T, Q> &other) const
{
  return (_myT == other._myT && _myQ == other._myQ);
}

template<class T, class Q>
bool 
AstSortableAssoc<T, Q>::operator!=(const AstSortableAssoc<T, Q> &other) const
{
  return (! (*this==other));
}

template<class T, class Q>
bool 
AstSortableAssoc<T, Q>::operator<(const AstSortableAssoc<T, Q> &other) const
{
  return _lessThanFunction(*_myQ, *other._myQ);
}
    
//-------------
// Selectors --
//-------------
    
//-------------
// Modifiers --
//-------------

template <class T, class Q>
void 
AstSortableAssoc<T, Q>::deleteMembers()
{
  delete _myT;
  _myT=0;
  delete _myQ;
  _myQ=0;
}

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








