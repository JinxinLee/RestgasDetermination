//-------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMap2.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Implementation of AstSTLMap2.  May or may not be included 
//      by the header file depending on BABAR_COMP_INST
//   
// Author List:
//      Luca Lista         27 Mar 97
//
// Bertram Kopf (RUB) migrated to PandaRoot
//-------------------------------------------------------------


//-----------------------
// This Class's Header --
//-----------------------

#include "AstSTLMap2.h"

//---------------
// C++ Headers --
//---------------
#include <map>
#include <vector>

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
AstSTLMap2<T1, T2>::AstSTLMap2() 
  //  _map1(AstMap2<T1, T2>::hashFunction1), _map2(AstMap2<T1, T2>::hashFunction2)
{
//   cerr << " AstMap2: warning: using pointer based hash function might"
//        << " be non portable and might give problems with persistency"
//        << endl;
}

template<class T1, class T2>
AstSTLMap2<T1, T2>::AstSTLMap2(unsigned (*hFun)(T1 *const&),
				unsigned (*hFun2)(T2 *const&))
  //  _map1(hFun), _map2(hFun2)
{}


//--------------
// Destructor --
//--------------

template<class T1, class T2>
AstSTLMap2<T1, T2>::~AstSTLMap2()
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

template<class T1, class T2>
bool AstSTLMap2<T1, T2>::contains(T1* t1, T2* t2) const
{ 
  bool contained=false;

  typename std::map<T1*, std::vector<T2*>, PndPtrLess>::const_iterator position;
  if ((position = _map1.find(t1)) != _map1.end()) {
    typename std::vector<T2*>::const_iterator iter;
    for (iter = position->second.begin(); 
	 iter != position->second.end(); ++iter) {
      if (*t2 == *(*iter)) {
	contained = true;
	break;
      }
    }
  }

  return contained;
}

template<class T1, class T2>
bool AstSTLMap2<T1, T2>::containsT1(T1* t1) const
{ 
  return (_map1.find(t1) != _map1.end());
}

template<class T1, class T2>
bool AstSTLMap2<T1, T2>::containsT2(T2* t2) const
{ 
  return (_map2.find(t2) != _map2.end());
}

template<class T1, class T2>
int AstSTLMap2<T1, T2>::entries1(T1* t1) const {
  typename std::map<T1*, std::vector<T2*>, PndPtrLess>::const_iterator position;
  if ((position = _map1.find(t1)) != _map1.end()){
    return position->second.size();
  } else {
    return 0;
  }
}

template<class T1, class T2>
int AstSTLMap2<T1, T2>::entries2(T2* t2) const {
  typename std::map<T2*, std::vector<T1*>, PndPtrLess>::const_iterator position;
  if ((position = _map2.find(t2)) != _map2.end()){
    return position->second.size();
  } else {
    return 0;
  }
}

template <class T1, class T2>
bool
AstSTLMap2<T1, T2>::findValue1(T1* findThis, std::vector<T2*> &returnThis) const{
  if (findThis==0) return false;
  typename std::map<T1*, std::vector<T2*>, PndPtrLess>::const_iterator position;
  if ((position = _map1.find(findThis)) != _map1.end()) {
    returnThis = position->second;
    return true;
  }
  return false;
}

template <class T1, class T2>
bool
AstSTLMap2<T1, T2>::findValue2(T2* findThis, std::vector<T1*> &returnThis) const{
  if (findThis==0) return false;
  typename std::map<T2*, std::vector<T1*>, PndPtrLess>::const_iterator position;
  if ((position = _map2.find(findThis)) != _map2.end()) {
    returnThis = position->second;
    return true;
  }
  return false;
}

template <class T1, class T2>
T2*
AstSTLMap2<T1, T2>::findFirstValue1(const T1* findThis) const
{
  if (findThis==0) return 0;
  typename std::map<T1*, std::vector<T2*>, PndPtrLess>::const_iterator position;
  if ((position = _map1.find((T1*)findThis)) != _map1.end()) {
    return (position->second)[0];
  }
  return 0;
}

template <class T1, class T2>
T1*
AstSTLMap2<T1, T2>::findFirstValue2(const T2* findThis) const
{
  if (findThis==0) return 0;
  typename std::map<T2*, std::vector<T1*>, PndPtrLess>::const_iterator position;
  if ((position = _map2.find((T2*)findThis)) != _map2.end()) {
    return (position->second)[0];
  }
  return 0;
}
    
//-------------
// Modifiers --
//-------------

template<class T1, class T2>
void AstSTLMap2<T1, T2>::append(T1* t1, T2* t2){ 
  _map1[t1].push_back(t2); _map2[t2].push_back(t1); 
}

template<class T1, class T2>
size_t AstSTLMap2<T1, T2>::remove(T1* t1,T2* t2){ 

  size_t retVal=1;
  size_t retVal2;

  typename std::map<T1*, std::vector<T2*>, PndPtrLess>::const_iterator position1;
  if ((position1 = _map1.find(t1)) != _map1.end()) {
    std::vector<T2*> foundValueT2 = position1->second;
    typename std::vector<T2*>::iterator iterT2 = foundValueT2.begin();
    retVal2 = 0;
    while (iterT2 != foundValueT2.end()) {
      if ( *(*iterT2) == *t2) {
	iterT2 = foundValueT2.erase(iterT2);
	retVal2 = 1;
      }
      else
	++iterT2;
    }
    if (0 == retVal2) retVal = 0;

    _map1[t1].clear(); _map1[t1] = foundValueT2;

    if (position1->second.size() == 0) _map1.erase(t1);

    typename std::map<T2*, std::vector<T1*>, PndPtrLess>::const_iterator position2;
    if ((position2 = _map2.find(t2)) != _map2.end()) {
      std::vector<T1*> foundValueT1 = position2->second;
      typename std::vector<T1*>::iterator iterT1 = foundValueT1.begin();
      retVal2 = 0;
      while (iterT1 != foundValueT1.end()) {
	if ( *(*iterT1) == *t1) {
	  iterT1 = foundValueT1.erase(iterT1);
	  retVal2 = 1;
	}
	else
	  ++iterT1;
      }
      if (0 == retVal2) retVal = 0;
      
      _map2[t2].clear(); _map2[t2] = foundValueT1;

      if (position2->second.size() == 0 ) _map2.erase(t2);
	
    } else {

      retVal=0;
    }
  } else {

    retVal=0;
  }
  return retVal;
}


template<class T1, class T2>
void AstSTLMap2<T1, T2>::clear(){
  _map1.clear();
  _map2.clear();
}

template<class T1, class T2>
size_t AstSTLMap2<T1, T2>::removeMatchesForKey1(const T1 * t1)
{

  size_t removedThisMany = 0;
  size_t removed;

  typename std::map<T1*, std::vector<T2*>, PndPtrLess>::const_iterator position;
  if ((position = _map1.find((T1*const&)t1)) != _map1.end()) {
    std::vector<T2*> t2v = position->second;
    removedThisMany = t2v.size();
    typename std::vector<T2*>::const_iterator iter;
    for (iter = t2v.begin(); iter != t2v.end(); ++iter)
      removed = remove((T1*)t1, *iter);
  } 
  return removedThisMany;
}

template<class T1, class T2>
size_t 
AstSTLMap2<T1, T2>::removeMatchesForKey2(const T2 * t2)
{
  size_t removedThisMany = 0;
  size_t removed;

  typename std::map<T2*, std::vector<T1*>, PndPtrLess>::const_iterator position;
  if (( position = _map2.find((T2*const&)t2)) != _map2.end()) {
    std::vector<T1*> t1v = position->second;
    removedThisMany = t1v.size();
    typename std::vector<T1*>::const_iterator iter;
    for (iter = t1v.begin(); iter != t1v.end(); ++iter)
      removed = remove(*iter, (T2*)t2);
  }
  return removedThisMany;
}

//              -----------------------------------------------
//              -- Static Data & Function Member Definitions --
//              -----------------------------------------------

template<class T1, class T2>
unsigned AstSTLMap2<T1, T2>::hashFunction1(T1 *const & p)
{ 
  return ((unsigned long) p >> 5) & 0x03ff; 
}

template<class T1, class T2>
unsigned AstSTLMap2<T1, T2>::hashFunction2(T2 *const& p)
{ 
  return ((unsigned long) p >> 5) & 0x03ff; 
}

//              -------------------------------------------
//              -- Protected Function Member Definitions --
//              -------------------------------------------

//              -----------------------------------------
//              -- Private Function Member Definitions --
//              -----------------------------------------

template<class T1, class T2>
std::map<T1*, std::vector<T2*>, PndPtrLess> &
AstSTLMap2<T1, T2>::notConstMap1() const{
  return (std::map<T1*, std::vector<T2*>, PndPtrLess > &)_map1;
}

template<class T1, class T2>
std::map<T2*, std::vector<T1*>, PndPtrLess> &
AstSTLMap2<T1, T2>::notConstMap2() const{
  return (std::map<T2*, std::vector<T1*>, PndPtrLess > &)_map2;
}


//              -----------------------------------
//              -- Internal Function Definitions --
//              -----------------------------------
