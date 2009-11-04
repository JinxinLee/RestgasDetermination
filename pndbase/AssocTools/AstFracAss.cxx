//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstFracAss.cc,v 1.2 2006/06/30 11:24:22 steinke Exp $
//
// Description:
//	Class AstFracAssTerm
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

#include "AstFracAss.h"

template<class A,class B>
AstFracAss<A,B>::AstFracAss(const A* aterm,
			    const B* bterm,
			    double weight) :
  AstFracElem<B>(bterm,weight), _aelem(aterm)
{;}

template<class A,class B>
AstFracAss<A,B>::AstFracAss():
  AstFracElem<B>(), _aelem(0)
{;}

template<class A,class B>
AstFracAss<A,B>::AstFracAss(const AstFracAss<A,B>& other) :
  AstFracElem<B>(other),_aelem(other._aelem)
{;}

template<class A,class B> 
AstFracAss<A,B>&
AstFracAss<A,B>::operator = (const AstFracAss<A,B>& other) {
  if(this != &other){
    AstFracElem<B>::operator = (other);
    _aelem = other._aelem;
  }
  return *this;
}


template<class A,class B> 
AstFracAss<A,B>::~AstFracAss()
{;}


template<class A,class B> 
const A*
AstFracAss<A,B>::firstArgument() const { 
  return _aelem; 
}

template<class A,class B> 
const B*
AstFracAss<A,B>::secondArgument() const { 
  return this->element(); 
}


// arithmetic allowed only if logically equal

template<class A,class B> 
AstFracAss<A,B>&
AstFracAss<A,B>::operator += (const AstFracAss<A,B>& other) {
  if(_aelem == other._aelem) AstFracElem<B>::operator += (other);
  return *this;
}

template<class A,class B> 
AstFracAss<A,B>&
AstFracAss<A,B>::operator -= (const AstFracAss<A,B>& other) {
  if(_aelem == other._aelem) AstFracElem<B>::operator -= (other);
  return *this;
}

// ignore the weight in logical comparisons.

template<class A,class B> 
bool
AstFracAss<A,B>::operator < (const AstFracAss<A,B>& other) const {
  return _aelem != other._aelem ? _aelem < other._aelem : 
    AstFracElem<B>::ltElement(other);
}

template<class A,class B>
bool
AstFracAss<A,B>::operator > (const AstFracAss<A,B>& other) const {
  return _aelem != other._aelem ? _aelem > other._aelem : 
    AstFracElem<B>::gtElement(other);
}

template<class A,class B> 
bool
AstFracAss<A,B>::operator == (const AstFracAss<A,B>& other) const {
  return _aelem == other._aelem && AstFracElem<B>::sameElement(other);
}

template<class A,class B> 
bool
AstFracAss<A,B>::operator != (const AstFracAss<A,B>& other) const {
  return !( *this == other);
}

