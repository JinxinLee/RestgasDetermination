//----------------------------------------------------------------
// File and Version Information:
// 	$Id: AstAbsSTLMap.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Implementation of AstAbsMap class.
//   
// Author List:
//      Matthias Steinke
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//---------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "AstAbsSTLMap.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <set>

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
AstAbsSTLMap<T1, T2>::AstAbsSTLMap(size_t buckets) :
  _myHashFunction(&AstAbsSTLMap<T1, T2>::hashFunction),
  _hashSetT1(new std::set<T1*>),
  _buckets(buckets)
{}

template<class T1, class T2>
AstAbsSTLMap<T1, T2>::AstAbsSTLMap(unsigned (*hashFunction)(T1 *const &), 
			     size_t buckets) :
  _myHashFunction(hashFunction),
  _hashSetT1(new std::set<T1*>),
  _buckets(buckets)
{}

//--------------
// Destructor --
//--------------

template<class T1, class T2>
AstAbsSTLMap<T1, T2>::~AstAbsSTLMap()
{
  delete _hashSetT1;
}

    
//-------------
// Modifiers --
//-------------

template <class T1, class T2>
void
AstAbsSTLMap<T1, T2>::append( T1* const & t1ref, T2* const & t2ref){
    
  _hashSetT1->insert(t1ref);
  this->appendToMap(t1ref, t2ref);
}



//              -------------------------------------------
//              -- Protected Function Member Definitions --
//              -------------------------------------------


template <class T1, class T2>
bool 
AstAbsSTLMap<T1, T2>::findIntersectionSet(const std::set<T1*> &withThis, 
				       std::set<T1*> &returnedSet) const{

  std::set<T1*> result;
  typename std::set<T1*>::const_iterator iter = withThis.begin();
  while (iter != withThis.end()) {
    if (_hashSetT1->find(*iter) != _hashSetT1->end())
      result.insert(*iter);
    ++iter;
  }

  returnedSet.clear();
  returnedSet = result;

  if (returnedSet.size()==0) return false;
  
  return true;

}


template <class T1, class T2>
bool 
AstAbsSTLMap<T1, T2>::findDifferenceSet(const std::set<T1*> &withThis, 
				     std::set<T1*> &returnedSet) const{

  returnedSet=withThis;


  typename std::set<T1*>::iterator iter = _hashSetT1->begin();
  typename std::set<T1*>::iterator found;

  while (iter != _hashSetT1->end()) {
    if ( (found = returnedSet.find(*iter)) != returnedSet.end())
      returnedSet.erase(found);
    ++iter;
  }

  if (returnedSet.size()==0) return false;
  
  return true;

}




//              -----------------------------------------------
//              -- Static Data & Function Member Definitions --
//              -----------------------------------------------


template<class T1, class T2>
unsigned AstAbsSTLMap<T1, T2>::hashFunction(T1 *const& p)
{ 
  return ((unsigned long) p >> 5) & 0x03ff; 
}

