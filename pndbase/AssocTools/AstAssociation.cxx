//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstAssociation.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstAssociation
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

#include "AstAssociation.h"

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
AstAssociation< T1,  T2>::AstAssociation(T1 *theT1,
					      T2 *theT2):
  _itsT1(theT1),
  _itsT2(theT2)
{}

// Copy constructor

template<class T1, class T2>
AstAssociation< T1,  T2>::AstAssociation(const AstAssociation<T1, T2> 
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
AstAssociation< T1,  T2>::~AstAssociation()
{}

//-------------
// Methods   --
//-------------
    
//-------------
// Operators --
//-------------


template<class T1, class T2>
AstAssociation<T1, T2> &
AstAssociation<T1, T2>::operator=(const AstAssociation<T1, T2> &other)
{
  if (*this != other){
    _itsT1 = other._itsT1;
    _itsT2 = other._itsT2;
  }
  return *this;
}

//-------------
// Selectors --
//-------------

template <class T1, class T2>
const T1 *
AstAssociation<T1, T2>::firstArgument() const{
  return _itsT1;
}

template <class T1, class T2>
const T2 *
AstAssociation<T1, T2>::secondArgument() const{
  return _itsT2;
}

template <class T1, class T2>
T1 *
AstAssociation<T1, T2>::firstArgument() {
  return _itsT1;
}

template <class T1, class T2> 
T2 *
AstAssociation<T1, T2>::secondArgument() {
  return _itsT2;
}


//-------------
// Modifiers --
//-------------
 

template <class T1, class T2>
void 
AstAssociation<T1, T2>::deleteMembers()
{
  delete _itsT1;
  _itsT1=0;
  delete _itsT2;
  _itsT2=0;
}




