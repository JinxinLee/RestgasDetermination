//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstFracElem.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstFracElem
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      David Brown
//
// Copyright Information:
//	Copyright (C) 2000 Lawrence Berkeley Laboratory
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#include "AstFracElem.h"

template<class B>
AstFracElem<B>::AstFracElem(const B* bterm,
			    double weight) :
  _weight(weight), _elem(bterm)
{;}

template<class B>
AstFracElem<B>::AstFracElem() :
  _weight(0.0), _elem(0)
{;}

template<class B>
AstFracElem<B>::AstFracElem(const AstFracElem<B>& other) :
  _weight(other._weight), _elem(other._elem)
{;}

template<class B> 
AstFracElem<B>&
AstFracElem<B>::operator = (const AstFracElem<B>& other) {
  if(this != &other){
    _elem = other._elem;
    _weight = other._weight;
  }
  return *this;
}

template<class B> AstFracElem<B>::~AstFracElem()
{;}

template<class B> 
bool
AstFracElem<B>::sameElement(const AstFracElem<B>& other) const {
  return _elem == other._elem || _elem==0 || other._elem==0;
}

template<class B> 
bool
AstFracElem<B>::ltElement(const AstFracElem<B>& other) const {
  return _elem !=0 && other._elem != 0 && _elem < other._elem; 
}

template<class B> 
bool
AstFracElem<B>::gtElement(const AstFracElem<B>& other) const {
  return _elem !=0 && other._elem != 0 && _elem > other._elem; 
}

// arithmetic allowed only if logically equal

template<class B> 
AstFracElem<B>&
AstFracElem<B>::operator += (const AstFracElem<B>& other) {
  if(_elem == other._elem)_weight += other.weight();
  return *this;
}

template<class B> 
AstFracElem<B>&
AstFracElem<B>::operator -= (const AstFracElem<B>& other) {
  if(_elem == other._elem)_weight -= other.weight();
  return *this;
}

// compare by weights.  The perversity of RW sorted vector requires
// I reverse the meaning of < and > in order to get a list with the
// largest value first.

template<class B> 
bool
AstFracElem<B>::operator < (const AstFracElem<B>& other) const {
  return _weight > other._weight;
}

template<class B>
bool
AstFracElem<B>::operator > (const AstFracElem<B>& other) const {
  return weight < other._weight;
}

template<class B> 
bool
AstFracElem<B>::operator == (const AstFracElem<B>& other) const {
  return _weight == other._weight;
}

template<class B> 
bool
AstFracElem<B>::operator != (const AstFracElem<B>& other) const {
  return !( *this == other);
}

