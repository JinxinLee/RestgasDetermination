//-------------------------------------------------------------
// File and Version Information:
// 	$Id: AstUniqMap2.cc,v 1.2 2006/06/30 11:24:22 steinke Exp $
//
// Description:
//      Implementation of AstUniqMap2.  May or may not be included 
//      by the header file depending on BABAR_COMP_INST
//   
// Author List:
//      Roland Martin        5th May 1998
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "AstUniqMap2.h"

//---------------
// C++ Headers --
//---------------
#include <map>
#include <algorithm>
#include "CollectionUtils.h"

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
AstUniqMap2<T1, T2>::AstUniqMap2() // :
//  _map1(AstUniqMap2<T1, T2>::hashFunction1), _map2(AstUniqMap2<T1, T2>::hashFunction2)
{
}

//
//
//
template<class T1, class T2>
AstUniqMap2<T1, T2>::AstUniqMap2(unsigned (*hFun)(const T1&),
				 unsigned (*hFun2)(const T2&)) //:
//   _map1(hFun), _map2(hFun2)
{}

//
//
//
template<class T1, class T2>
AstUniqMap2<T1, T2>::AstUniqMap2(unsigned (*hFun)(const T1&),
				 unsigned (*hFun2)(const T2&),
				 size_t buckets) // :
//  _map1(hFun, buckets), _map2(hFun2, buckets)
{}

template<class T1, class T2>
AstUniqMap2<T1, T2>::AstUniqMap2(const AstUniqMap2<T1, T2> &other):
  _map1(other._map1),
  _map2(other._map2)
{}

//--------------
// Destructor --
//--------------
template<class T1, class T2>
AstUniqMap2<T1, T2>::~AstUniqMap2()
{}

//-------------
// Methods   --
//-------------
    
//-------------
// Operators --
//-------------
    
//-------------
// Selectors --
//-------------
template <class T1, class T2>
const T2* AstUniqMap2<T1, T2>::find1(const T1* findThis) const{
  if (findThis==0) return 0;
  typename std::map<T1*, T2*, PndPtrLess>::const_iterator position = _map1.find((T1 *)findThis);
  if (position == _map1.end()) return 0;
  return position->second;
}

//
//
//
template <class T1, class T2>
const T1* AstUniqMap2<T1, T2>::find2(const T2* findThis) const {
  if (findThis==0) return 0;
  typename std::map<T2*, T1*, PndPtrLess>::const_iterator position = _map2.find((T2 *)findThis);
  if (position == _map2.end()) return 0;
  return position->second;
}
    
//-------------
// Modifiers --
//-------------
template<class T1, class T2>
bool AstUniqMap2<T1, T2>::append(T1* t1, T2* t2) { 
  if (_map1.find(t1) != _map1.end() || _map2.find(t2) != _map2.end()) return false;
  _map1.insert(typename std::map<T1*, T2*, PndPtrLess>::value_type(t1, t2));
  _map2.insert(typename std::map<T2*, T1*, PndPtrLess>::value_type(t2, t1));
  return true;
}

//
//
//
template<class T1, class T2>
T2* AstUniqMap2<T1, T2>::remove1(const T1* t1) {
  typename std::map<T1*, T2*, PndPtrLess>::iterator position = _map1.find((T1 *)t1);
  T2* t2 = 0;
  if (position != _map1.end()) {
    t2=position->second;
    _map1.erase(position);
    _map2.erase(_map2.find(t2));
  }
  return t2;
}

//
//
//
template<class T1, class T2>
T1* AstUniqMap2<T1, T2>::remove2(const T2* t2) {
  typename std::map<T2*, T1*, PndPtrLess>::iterator position = _map2.find((T2 *)t2);
  T1* t1 = 0;
  if (position != _map2.end()) {
    t1=position->second;
    _map2.erase(position);
    _map1.erase(_map1.find(t1));
  }
  return t1;
}

//
//
//
template<class T1, class T2>
void AstUniqMap2<T1, T2>::clear(){
  _map1.clear();
  _map2.clear();
}

//
// Only delete objects in one of the maps (they are the same as in the other)
//
template<class T1, class T2>
void AstUniqMap2<T1, T2>::clearAndDestroy(){
  _map1.clear();
  typename std::map<T2*, T1*, PndPtrLess>::iterator iter = _map2.begin();
  while (iter != _map2.end()) {
    delete iter->first;
    delete iter->second;
    ++iter;
  }
  _map2.clear();
}

//              -----------------------------------------------
//              -- Static Data & Function Member Definitions --
//              -----------------------------------------------

// default hash functions...based on pointer values - not good !
template<class T1, class T2>
unsigned AstUniqMap2<T1, T2>::hashFunction1(const T1& p)
{ 
  //  unsigned hash = ((unsigned long) p >> 5) & 0x03ff ;
  return ((unsigned long) &p >> 5) & 0x03ff; 
}

template<class T1, class T2>
unsigned AstUniqMap2<T1, T2>::hashFunction2(const T2& p)
{ 
  // unsigned hash = ((unsigned long) p >> 5) & 0x03ff ;
  return ((unsigned long) &p >> 5) & 0x03ff; 
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











