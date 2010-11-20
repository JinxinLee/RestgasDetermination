//-------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstIndexedObject.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Simple class to hold an object and its index (in a vector
//      for example) in the same class
//   
// Author List:
//      Phil Strother               March 2000
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "AstIndexedObject.h"


template<class T>
AstIndexedObject<T>::AstIndexedObject(int anInt, T*aT){
  _myInt = anInt;
  _myT = aT;
}

//--------------
// Destructor --
//--------------
template<class T>
AstIndexedObject<T>::~AstIndexedObject()
{}

template<class T>
bool
AstIndexedObject<T>::operator==(const AstIndexedObject<T> &other) const{
  return ( (other._myInt == _myInt) && (other._myT == _myT) );
}
