//-------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLMapIndex.cc,v 1.2 2006/06/30 11:24:22 steinke Exp $
//
// Description:
//      Given a HepAList or std::vector, provides a fast
//      map between the objects in the vector and the 
//      index within the vector.
//   
// Author List:
//      Matthias Steinke
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "AstSTLMapIndex.h"

//---------------
// C++ Headers --
//---------------
#include <vector>
#include <map>
#include "PndCollectionUtils.h"
using panda::Collection::PtrLess;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "AstIndexedObject.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//              ----------------------------------------
//              -- Public Function Member Definitions --
//              ----------------------------------------

//----------------
// Constructors --
//----------------

template <class T>
AstSTLMapIndex<T>::AstSTLMapIndex(const std::vector<T*> &list,
			    unsigned (*hFun)(const T&)){
  _map = new std::map<T*, AstIndexedObject<T>*, PtrLess >;
  _hashFun = hFun;
  
  int index;
  int listLength = list.size();
  for (index = 0; index < listLength; index++){
    T* theObject = list[index];
    AstIndexedObject<T> *indexedObject = new AstIndexedObject<T>(index, theObject);
    _map->insert(typename std::map<T*, AstIndexedObject<T>*, 
		 PtrLess>::value_type(theObject, indexedObject));
  }

}

//--------------
// Destructor --
//--------------
template<class T>
AstSTLMapIndex<T>::~AstSTLMapIndex()
{

  typename std::map<T*, AstIndexedObject<T>*, PtrLess>::iterator killer = _map->begin();
  
  while (killer != _map->end()) {
    delete killer->second;
    ++killer;
  }
  
  delete _map;
}

//-------------
// Methods   --
//-------------

template<class T>
bool
AstSTLMapIndex<T>::contains(const T* theT) const{
  return (_map->find(theT) != _map->end());
}

template<class T>
bool
AstSTLMapIndex<T>::index(const T* theT, int &index) const{
  typename std::map<T*, AstIndexedObject<T>*, PtrLess>::iterator position;
  position = _map->find((T*)theT);
  if (position != _map->end()) {
    AstIndexedObject<T> *foundIndex = position->second;
    index = foundIndex->getInt();
    return true;
  }
  else
    return false;
}

template<class T>
bool
AstSTLMapIndex<T>::append(T* theT) {

  if (_map->find(theT) != _map->end()) return false;

  AstIndexedObject<T> *newIndex = 
    new AstIndexedObject<T>(_map->size(),theT);
  _map->insert(typename std::map<T*, AstIndexedObject<T>*, PtrLess>::value_type(theT, newIndex));  
  return true;
}

template<class T>
T*
AstSTLMapIndex<T>::remove(const T*theT){
  typename std::map<T*, AstIndexedObject<T>*, PtrLess>::iterator position;
  position = _map->find((T*)theT);
  if (position == _map->end()) 
    return 0;
  AstIndexedObject<T> *foundIndex = position->second;
  int index = foundIndex->getInt();
  T* foundT = foundIndex->getT();

  std::vector<AstIndexedObject<T>*> temp;

  typename std::map<T*, AstIndexedObject<T>*, PtrLess>::iterator findMore = _map->begin(); 
  AstIndexedObject<T> *nextIndex;
  while (findMore != _map->end()){
    nextIndex = findMore->second;
    if (nextIndex->getInt() > index) temp.push_back(nextIndex);
    ++findMore;
  }
  _map->erase(_map->find((T*)theT));
  delete foundIndex;

  int i;
  for (i=0;i<temp.size();i++){
    T* findThis = temp[i]->getT();
    int findIndex = temp[i]->getInt();
    delete _map->find(findThis)->second;
    // Note that at this point the object at temp[i] is 
    // no longer functional!
    AstIndexedObject<T> *newIndex = new AstIndexedObject<T>(findIndex-1,findThis);
    (*_map)[findThis] = newIndex;
  }

  // Ouf! Done...
  return foundT;
}


template<class T>
int
AstSTLMapIndex<T>::members() const{
  return _map->size();
}

template<class T>
void 
AstSTLMapIndex<T>::clear(){

  typename std::map<T*, AstIndexedObject<T>*, PtrLess>::iterator killer = _map->begin();
  
  while (killer != _map->end()) {
    delete killer->second;
    ++killer;
  }
  _map->clear();

}

template<class T>
void
AstSTLMapIndex<T>::clearAndDestroy(){

  clear();

}
//-------------
// Operators --
//-------------
    
//-------------
// Selectors --
//-------------

//              -----------------------------------------------
//              -- Static Data & Function Member Definitions --
//              -----------------------------------------------

template<class T>
unsigned AstSTLMapIndex<T>::hashFunction(const T& p)
{ 
  return ((unsigned long) &p >> 5) & 0x03ff; 
}

