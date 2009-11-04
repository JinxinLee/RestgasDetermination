//----------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMap.cc,v 1.2 2006/06/30 11:24:22 steinke Exp $
//
// Description:
//      Implementation of AstSTLMap class.
//   
// Author List:
//      Luca Lista         26 Mar 97
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//---------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "AstSTLMap.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <map>
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
AstSTLMap<T1, T2>::AstSTLMap(size_t buckets) :
  AstAbsSTLMap<T1,T2>(buckets),
  _allT2(new std::vector<T2*>())
{}

template<class T1, class T2>
AstSTLMap<T1, T2>::AstSTLMap(unsigned (*hashFunction)(T1 *const &), size_t buckets) :
  AstAbsSTLMap<T1,T2>(hashFunction, buckets),
  _allT2(new std::vector<T2*>())
{}

//--------------
// Destructor --
//--------------

template<class T1, class T2>
AstSTLMap<T1, T2>::~AstSTLMap()
{
  delete _allT2;
}

    
//-------------
// Modifiers --
//-------------

template <class T1, class T2>
void
AstSTLMap<T1, T2>::appendToMap( T1* const & t1ref, T2* const & t2ref){
  (*this)[t1ref].push_back(t2ref);
  
  _allT2->push_back(t2ref);
}

template <class T1, class T2>
bool 
AstSTLMap<T1, T2>::findIntersectionSet(const std::set<T1*> &withThis, 
				    std::vector<T2*> &returned) const{

  // First construct a dummy hash set since the intersection function modifies self.
  
  std::set<T1*> dummy;

  return findIntersectionSet(withThis, returned, dummy);

}


template <class T1, class T2>
bool 
AstSTLMap<T1, T2>::findIntersectionSet(const std::set<T1*> &withThis, 
				    std::vector<T2*> &returned, 
				    std::set<T1*> &returnedSet) const{

  if (!AstAbsSTLMap<T1,T2>::findIntersectionSet(withThis, returnedSet)) return false;
  
  typename std::set<T1*>::iterator iter = returnedSet.begin();
  while(iter != returnedSet.end()){
    std::vector<T2*> contained;
    typename std::map<T1*, std::vector<T2*> >::const_iterator position;
    if ((position = find(*iter)) != AstSTLMap<T1, T2>::end()) {
      contained = position->second;
      // RW Ordered Vector cannot append one of itself!
      for (size_t i=0; i<contained.size();i++)
	returned.push_back(contained[i]);
    }
    ++iter;
  }

  return true;

}

template <class T1, class T2>
bool 
AstSTLMap<T1, T2>::findIntersectionSet(const std::vector<T1*> &vector,
				    std::vector<T2*> &returned) const{
  

  std::set<T1*> inputVector;

  for (size_t i= 0; i<vector.size();i++){
      inputVector.insert(vector[i]);
  }
  return findIntersectionSet(inputVector, returned);

}

template <class T1, class T2>
bool 
AstSTLMap<T1, T2>::findDifferenceSet(const std::set<T1*> &withThis, 
				  std::vector<T2*> &returned) const{
  
  std::set<T1*> dummy;

  return findDifferenceSet(withThis, returned, dummy);

  
}

template <class T1, class T2>
bool 
AstSTLMap<T1, T2>::findDifferenceSet(const std::set<T1*> &withThis, 
				  std::vector<T2*> &returned, 
				  std::set<T1*> &returnedSet) const{

   if (!AstAbsSTLMap<T1,T2>::findDifferenceSet(withThis, returnedSet)) return false;

  typename std::set<T1*>::iterator iter = returnedSet.begin();

//    while(iter != returnedSet.end()){
    if ( find(*iter) != AstSTLMap<T1, T2>::end()) {     // Bertram: replaced upper line by this line
    std::vector<T2*> contained = find(*iter)->second;
    for (size_t i=0; i<contained.size();i++)
      returned.push_back(contained[i]);    
    ++iter;
     }

  return true;
}

template <class T1, class T2>
bool 
AstSTLMap<T1, T2>::findDifferenceSet(const std::vector<T1*> &vector, 
				  std::vector<T2*> &returned) const{

  std::set<T1*> inputVector;
  
  for (size_t i= 0; i<vector.size();i++){
    inputVector.insert(vector[i]);
  }
  
  return findDifferenceSet(inputVector, returned);

}

template <class T1, class T2>
AstSTLMap<T1, T2> &
AstSTLMap<T1, T2>::unionMap(const AstSTLMap<T1, T2> &withThis){

  typename std::set<T1*>::iterator iter = withThis.hashSetT1()->begin();
  while (iter != withThis.hashSetT1()->end()) {
    AstSTLMap<T1, T2>::hashSetT1()->insert(*iter);
    ++iter;
  }

  iter = AstSTLMap<T1, T2>::hashSetT1()->begin();
  
  while (iter != AstSTLMap<T1, T2>::hashSetT1()->end()){

    // Only add new contents.  Do not want overwrites
    if (find(*iter) == AstSTLMap<T1, T2>::end()) {
      std::vector<T2*> retVal = withThis.find(*iter)->second;
      (*this)[*iter] = retVal;
    }
    ++iter;
  }

  // It's more efficient to clear the T2 list and re-write it 
  // than to remove specific objects.
  _allT2->clear();

  typename std::map<T1*, std::vector<T2*> >::iterator iterator = AstSTLMap<T1, T2>::begin();
  while (iterator != AstSTLMap<T1, T2>::end()) {
    std::vector<T2*> addThese = iterator->second;
    for (size_t i= 0; i<addThese.size();i++) 
      _allT2->push_back(addThese[i]);
    ++iterator;
  }
  return *this;
}

template <class T1, class T2>
AstSTLMap<T1, T2> &
AstSTLMap<T1, T2>::intersectionMap(const AstSTLMap<T1, T2> &withThis){

  std::set<T1*> result;
  typename std::set<T1*>::iterator iter = withThis.hashSetT1()->begin();
  while (iter != withThis.hashSetT1()->end()) {
    if (AstSTLMap<T1, T2>::hashSetT1()->find(*iter) != AstSTLMap<T1, T2>::hashSetT1()->end())
      result.insert(*iter);
    ++iter;
  }

  AstSTLMap<T1, T2>::hashSetT1()->clear();
  (*(AstSTLMap<T1, T2>::hashSetT1())) = result;

  iter = AstSTLMap<T1, T2>::hashSetT1()->begin();
  
  while (iter != AstSTLMap<T1, T2>::hashSetT1()->end()){

    // Only add new contents.  Do not want overwrites
    if (find(*iter) == AstSTLMap<T1, T2>::end()) {     
      std::vector<T2*> retVal = withThis.find(*iter)->second;
      (*this)[*iter] = retVal;
    }
    ++iter;
  }

  _allT2->clear();

  typename std::map<T1*, std::vector<T2*> >::iterator iterator = AstSTLMap<T1, T2>::begin();
  while (iterator != AstSTLMap<T1, T2>::end()) {
    std::vector<T2*> addThese = iterator->second;
    for (size_t i= 0; i<addThese.size();i++) _allT2->push_back(addThese[i]);
    ++iterator;
  }
  
  return *this;
}


