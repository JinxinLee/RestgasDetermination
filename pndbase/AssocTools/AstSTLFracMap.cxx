//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLFracMap.cc,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstFracMap.  Defines a fractional map between objects
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//     David Brown
//
// Copyright Information:
//	Copyright (C) 2000		Lawrence Berkeley Laboratory
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#include "AstSTLFracMap.h"

#include <vector>
#include <algorithm>

template <class A,class B> 
AstSTLFracMap<A,B>::AstSTLFracMap(unsigned na)
{
  _vector.reserve(na);
}

template <class A,class B> 
AstSTLFracMap<A,B>::~AstSTLFracMap()
{;}

template <class A,class B> 
void
AstSTLFracMap<A,B>::insert(const A* a,const B* b,double weight) {
// create an AstFracAss from these
  AstFracAss<A,B> ab(a,b,weight);
// see if these elements already exist logically in the lists
  typename std::vector<AstFracAss<A,B> >::iterator iter = 
    std::find(_vector.begin(), _vector.end(), ab);
  if (iter != _vector.end()) {
    (*iter) += ab;
  } 
  else {
    _vector.push_back(ab);
    std::sort(_vector.begin(), _vector.end());
  }
}

template <class A,class B> 
double
AstSTLFracMap<A,B>::weight(const A* a,const B* b) const {
  AstFracAss<A,B> ab(a,b,0.0);
// see if these elements already exist logically in the lists
  typename std::vector<AstFracAss<A,B> >::const_iterator iter =
    std::find(_vector.begin(), _vector.end(), ab);
  if (iter != _vector.end()) {
    return iter->weight();
  }
  else {    
    return 0.0;
  }
}

template <class A,class B> 
void
AstSTLFracMap<A,B>::elements(const A* a,
			   std::vector<AstFracElem<B> >& vector ) const {
// always clear the input vector
  vector.clear();
// use the A vector; start by finding a match to any B
  AstFracAss<A,B> ab(a,0,0.0);
  typename std::vector<AstFracAss<A,B> >::const_iterator iter = 
    std::find(_vector.begin(), _vector.end(), ab);
  while (iter != _vector.end()) {
    vector.push_back(*iter);
    iter++;
    iter = std::find(iter, _vector.end(), ab);
  }
  std::sort(vector.begin(), vector.end());
}

template <class A,class B> 
void
AstSTLFracMap<A,B>::entries( std::vector<const A*>& vector) const {
  vector.clear();
  const A* old(0);
  typename std::vector<AstFracAss<A,B> >::const_iterator iter;
  for (iter = _vector.begin(); iter != _vector.end(); iter++) {
    if (iter->firstArgument() != old){
      old = iter->firstArgument();
      vector.push_back(old);
    }
  }
}

template <class A,class B> 
void
AstSTLFracMap<A,B>::normalize(double normvalue,const A* a) {
// if specified, rescale only the one entry
  if(a!=0)
    rescaleEntry(normvalue,a);
  else{
// otherwise, loop over them all
    std::vector<const A*> avec;
    entries(avec);
    typename std::vector<const A*>::iterator iter;
    for (iter = avec.begin(); iter != avec.end(); iter++) {
      rescaleEntry(normvalue,*iter);
    }
  }
}

template <class A,class B> 
void
AstSTLFracMap<A,B>::rescaleEntry(double normvalue,const A* a) {
  AstFracAss<A,B> ab(a,0,0.0);
  typename std::vector<AstFracAss<A,B> >::iterator iterStart = 
    std::find(_vector.begin(), _vector.end(), ab);
  typename std::vector<AstFracAss<A,B> >::iterator iterStop = iterStart; 
  double sum = 0.0;
  while (iterStop != _vector.end() && *iterStop == ab) { 
    sum += iterStop->weight();
    iterStop++;
  }
  if(sum>0.0) {
    double factor=normvalue/sum;
    typename std::vector<AstFracAss<A,B> >::iterator iter;
    for (iter = iterStart; iter != iterStop; iter++) { 
      iter->rescale(factor);
    }
  }
}






