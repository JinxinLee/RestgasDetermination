//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstNamedMapVector.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstNamedMapVector
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
//------------------------------------------------------------------------
 
//-----------------------
// This Class's Header --
//-----------------------

#include "AstNamedMapVector.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <string>

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
AstNamedMapVector<T1, T2>::AstNamedMapVector(const std::string &name):
  _myName(new std::string(name)),
  _vectorOne(new std::vector<T1*>()),
  _vectorTwo(new std::vector<T2*>())
{}

template<class T1, class T2>
AstNamedMapVector<T1, T2>::AstNamedMapVector(const std::string &name,
					   unsigned (*fn1)(const T1&),
					   unsigned (*fn2)(const T2&)):
  AstUniqMap2<T1, T2>(fn1, fn2),
  _myName(new std::string(name)),
  _vectorOne(new std::vector<T1*>()),
  _vectorTwo(new std::vector<T2*>())
{}

template<class T1, class T2>
AstNamedMapVector<T1, T2>::AstNamedMapVector(const std::string &name,
					   unsigned (*fn1)(const T1&),
					   unsigned (*fn2)(const T2&), 
					   size_t buckets):
  AstUniqMap2<T1, T2>(fn1, fn2, buckets),
  _myName(new std::string(name)),
  _vectorOne(new std::vector<T1*>()),
  _vectorTwo(new std::vector<T2*>())
{
}

// Copy constructor

template<class T1, class T2>
AstNamedMapVector< T1,  T2>::AstNamedMapVector(const AstNamedMapVector<T1, T2> 
					     &other):
  AstUniqMap2<T1, T2>(other)
{
  _myName = new std::string(other.name());
  _vectorOne = new std::vector<T1*>(other.list1());
  _vectorTwo = new std::vector<T2*>(other.list2());
}

//--------------
// Destructor --
//--------------


template<class T1, class T2>
AstNamedMapVector< T1,  T2>::~AstNamedMapVector()
{
  delete _myName;
  delete _vectorOne;
  delete _vectorTwo;
}

//-------------
// Methods   --
//-------------

template<class T1, class T2>
bool
AstNamedMapVector< T1,  T2>::append(T1* theT1, T2* theT2){
  if (!AstUniqMap2<T1,T2>::append(theT1, theT2)) return false;

  _vectorOne->push_back(theT1);
  _vectorTwo->push_back(theT2);
  return true;
}

template<class T1, class T2>
T2*
AstNamedMapVector< T1,  T2>::remove1(const T1* theT1){
  T2 *theT2 = AstUniqMap2<T1, T2>::remove1(theT1);
  if (theT2  != 0){
    typename std::vector<T1*>::iterator posT1 =  std::find( _vectorOne->begin(), _vectorOne->end(), theT1 );
    typename std::vector<T1*>::iterator posT2 =  std::find( _vectorTwo->begin(), _vectorTwo->end(), theT2 );
    if (posT1 != _vectorOne->end() && posT2 != _vectorTwo->end() ) {
      _vectorOne->erase(posT1);
      _vectorTwo->erase(posT2);
    }
    //    _vectorOne->remove(theT1);
    //    _vectorTwo->remove(theT2);
  }

  return theT2;

}

template<class T1, class T2>
T1*
AstNamedMapVector< T1,  T2>::remove2(const T2* theT2){
  T1 *theT1 = AstUniqMap2<T1, T2>::remove2(theT2);
  if (theT1  != 0){
    typename std::vector<T1*>::iterator posT1 =  std::find( _vectorOne->begin(), _vectorOne->end(), theT1 );
    typename std::vector<T1*>::iterator posT2 =  std::find( _vectorTwo->begin(), _vectorTwo->end(), theT2 );
    if (posT1 != _vectorOne->end() && posT2 != _vectorTwo->end() ) {
      _vectorOne->erase(posT1);
      _vectorTwo->erase(posT2);
    }
    //    _vectorOne->remove(theT1);
    //    _vectorTwo->remove(theT2);
  }

  return theT1;
}

//-------------
// Operators --
//-------------


template<class T1, class T2>
AstNamedMapVector<T1, T2> &
AstNamedMapVector<T1, T2>::operator=(const AstNamedMapVector<T1, T2> &other)
{
  if (this != &other){
    AstUniqMap2<T1, T2>::operator=(other);
    delete _myName;
    delete _vectorOne;
    delete _vectorTwo;
    _myName = new std::string(other.name());
    _vectorOne = new std::vector<T1*>(other.list1());
    _vectorTwo = new std::vector<T2*>(other.list2());
  }
  return *this;
}

template<class T1, class T2>
bool 
AstNamedMapVector<T1, T2>::operator==(const AstNamedMapVector<T1, T2> &other) const
{
  // May look overly simplistic.  But the assumption is that the name
  // _uniquely_ identifies the map for a given T1, T2.
  return (_myName == other._myName);
}

template<class T1, class T2>
bool 
AstNamedMapVector<T1, T2>::operator!=(const AstNamedMapVector<T1, T2> &other) const
{
  return (! (*this==other));
}

    
//-------------
// Selectors --
//-------------
    
//-------------
// Modifiers --
//-------------

template <class T1, class T2>
void 
AstNamedMapVector<T1, T2>::clear()
{
  AstUniqMap2<T1, T2>::clear();
  _vectorOne->clear();
  _vectorTwo->clear();
}

template <class T1, class T2>
void 
AstNamedMapVector<T1, T2>::clearAndDestroy()
{
  AstUniqMap2<T1, T2>::clearAndDestroy();
  _vectorOne->clear();
  _vectorTwo->clear();
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








