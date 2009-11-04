//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLFracMap.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstSTLFracMap.  Defines a fractional map between objects
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

#ifndef ASTSTLFRACMAP_HH
#define ASTSTLFRACMAP_HH

#include "AstFracAss.h"

#include <vector>

template <class A,class B> 
class AstSTLFracMap {

public:

  // construct given approximate number of elements
  AstSTLFracMap(unsigned na=16);

  virtual ~AstSTLFracMap();

  // append a new pair.  if this pair already exists, the weights are added
  void insert(const A*,const B*,double weight=1.0);

  // recover the weight of a given pair, will return zero if there's none
  double weight(const A*,const B*) const;

  // recover the list of 'A' entries.  Caller supplies the vector
  void entries(std::vector<const A*>& vector) const;

  // Recover the list of weighted elements for a given 'A'.  Caller supplies
  // the vector
  void elements(const A* a, 
		std::vector<AstFracElem<B> >& vector ) const;

  // simple accessors
  unsigned nAssociations() const { return _vector.size(); }

  // Renormalize.  Providing no entry normalizes all entries to the value
  void normalize(double normvalue,const A* a=0);
  void clear() { _vector.clear(); }

private:

  std::vector< AstFracAss<A,B> >  _vector;
  void rescaleEntry(double normvalue,const A* a=0);
};

#include  "AstSTLFracMap.cxx"

#endif
