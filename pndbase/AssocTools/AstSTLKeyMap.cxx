//----------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLKeyMap.cc,v 1.2 2006/06/30 11:24:22 steinke Exp $
//
// Description:
//      Implementation of AstSTLKeyMap class.
//   
// Author List:
//     Matthias Steinke
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//---------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------

#include "AstSTLKeyMap.h"

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
AstSTLKeyMap<T1, T2>::AstSTLKeyMap(unsigned (*hashFunction)(const T1&),
			     size_t buckets) :
  std::map<T1, std::vector<T2*> >(),
  _myHashFunction(hashFunction),
  _allT2(new std::vector<T2*>),
  _hashSetT1(new std::set<T1>),
  _buckets(buckets)
{}

//--------------
// Destructor --
//--------------

template<class T1, class T2>
AstSTLKeyMap<T1, T2>::~AstSTLKeyMap()
{
  delete _allT2;
  delete _hashSetT1;
}

    
//-------------
// Modifiers --
//-------------

template <class T1, class T2>
void
AstSTLKeyMap<T1, T2>::append( const T1 &t1ref, T2* const & t2ref){
  (*this)[t1ref].push_back(t2ref);
  
  _allT2->push_back(t2ref);
  
  _hashSetT1->insert(t1ref);

}

template <class T1, class T2>
bool 
AstSTLKeyMap<T1, T2>::findIntersectionSet(const std::set<T1> &withThis, 
					  std::vector<T2*> &returned) const{

  // First construct a dummy hash set since the intersection function modifies self.
  
  std::set<T1> dummy;

  return findIntersectionSet(withThis, returned, dummy);

}


template <class T1, class T2>
bool 
AstSTLKeyMap<T1, T2>::findIntersectionSet(const std::set<T1> &withThis, 
					  std::vector<T2*> &returned, 
					  std::set<T1> &returnedSet) const{

  std::set<T1> clone(withThis);
  returnedSet.clear();

  typename std::set<T1>::iterator iter = clone.begin();
  while(iter != clone.end()){
    if (_hashSetT1->find(*iter) != _hashSetT1->end())
      returnedSet.insert(*iter);
    ++iter;
  }
  
  clone = returnedSet;
  
  //  returnedSet=clone.intersection(*_hashSetT1); 

  if (returnedSet.size()==0) return false;
  
  iter = clone.begin();
  while(iter != clone.end()){
    std::vector<T2*> contained;
    if (this->find(*iter) != AstSTLKeyMap<T1, T2>::end()) {
      contained = this->find(*iter)->second;
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
AstSTLKeyMap<T1, T2>::findIntersectionSet(const std::vector<T1> &vector,
					  std::vector<T2*> &returned) const{
  

  std::set<T1> inputVector;

  for (size_t i= 0; i<vector.size();i++){
      inputVector.insert(vector[i]);
  }
  return findIntersectionSet(inputVector, returned);

}

template <class T1, class T2>
bool 
AstSTLKeyMap<T1, T2>::findDifferenceSet(const std::set<T1> &withThis, 
					std::vector<T2*> &returned) const{
  
  std::set<T1> dummy;

  return findDifferenceSet(withThis, returned, dummy);

  
}

template <class T1, class T2>
bool 
AstSTLKeyMap<T1, T2>::findDifferenceSet(const std::set<T1> &withThis,
					std::vector<T2*> &returned, 
					std::set<T1> &returnedSet) const{

  std::set<T1> clone(*_hashSetT1);
  typename std::set<T1>::iterator iter = withThis.begin();

  while (iter != withThis.end()) {
    clone.erase(*iter);
    ++iter;
  }
  returnedSet = clone;

  //  returnedSet = clone.difference(withThis);

  iter = clone.begin();

  while(iter != clone.end()){

    if ( this->find(*iter) != AstSTLKeyMap<T1, T2>::end()) {
    std::vector<T2*> contained = this->find(*iter)->second;
    for (size_t i=0; i<contained.size();i++)
      returned.push_back(contained[i]);    
    }

    ++iter;
  }

  return true;
}

template <class T1, class T2>
bool 
AstSTLKeyMap<T1, T2>::findDifferenceSet(const std::vector<T1> &vector, 
					std::vector<T2*> &returned) const{

  std::set<T1> inputVector;
  
  for (size_t i= 0; i<vector.size();i++){
    inputVector.insert(vector[i]);
  }
  
  return findDifferenceSet(inputVector, returned);

}

template <class T1, class T2>
AstSTLKeyMap<T1, T2> &
AstSTLKeyMap<T1, T2>::unionMap(const AstSTLKeyMap<T1, T2> &withThis){

  typename std::set<T1>::iterator iter = withThis._hashSetT1->begin();
  while (iter != withThis._hashSetT1->end()) {
    if (_hashSetT1->find(*iter) == _hashSetT1->end())
      _hashSetT1->insert(*iter);
    ++iter;
  }

    //  _hashSetT1->Union(*(withThis._hashSetT1));

  iter = _hashSetT1->begin();
  
  while (iter != _hashSetT1->end()){

    // Only add new contents.  Do not want overwrites
    if (find(*iter) == AstSTLKeyMap<T1, T2>::end()) {
      std::vector<T2*> retVal;
      retVal = withThis.find(*iter)->second;
      (*this)[*iter] = retVal;
    }
    ++iter;
  }

  // It's more efficient to clear the T2 list and re-write it 
  // than to remove specific objects.
  _allT2->clear();

  typename std::map<T1, std::vector<T2*> >::iterator iterator = AstSTLKeyMap<T1, T2>::begin();
  while (iterator != AstSTLKeyMap<T1, T2>::end()) {
    std::vector<T2*> addThese = iterator->second;
    for (size_t i= 0; i<addThese.size();i++) _allT2->push_back(addThese[i]);
    ++iterator;
  }
  return *this;
}

template <class T1, class T2>
AstSTLKeyMap<T1, T2> &
AstSTLKeyMap<T1, T2>::intersectionMap(const AstSTLKeyMap<T1, T2> &withThis){

  std::set<T1> intersection;
  typename std::set<T1>::iterator iter = _hashSetT1->begin();
  while(iter != _hashSetT1->end()){
    if (withThis._hashSetT1->find(*iter) != withThis._hashSetT1->end())
      intersection.insert(*iter);
      ++iter;
  }

  (*_hashSetT1) = intersection;

  //  _hashSetT1->intersection(*(withThis._hashSetT1));

  iter = _hashSetT1->begin();
  
  while (iter != _hashSetT1->end()){

    // Only add new contents.  Do not want overwrites
    if (find(*iter) == AstSTLKeyMap<T1, T2>::end()) {
      std::vector<T2*> retVal;
      retVal = withThis.find(*iter)->second;
      (*this)[*iter] = retVal;
    }
    ++iter;
  }

  _allT2->clear();

  typename std::map<T1, std::vector<T2*> >::iterator iterator = AstSTLKeyMap<T1, T2>::begin();
  while (iterator != AstSTLKeyMap<T1, T2>::end()) {
    std::vector<T2*> addThese = iterator->second;
    for (size_t i= 0; i<addThese.size();i++) _allT2->push_back(addThese[i]);
    ++iterator;
  }
  
  return *this;
}


//              -------------------------------------------
//              -- Protected Function Member Definitions --
//              -------------------------------------------

//              -----------------------------------------
//              -- Private Function Member Definitions --
//              -----------------------------------------

//              -----------------------------------
//              -- Internal Function Definitions --
//              -----------------------------------


