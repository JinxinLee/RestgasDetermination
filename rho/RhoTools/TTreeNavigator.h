//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TTreeNavigator.h,v 1.3 2002-02-01 03:54:59 marcel Exp $
//
// Description:
//	TTreeNavigator
//      Utility class to navigate in a decay tree
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   Oct, 1998  - creation
//
// Copyright Information:
//	Copyright (C) 1998	       CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TTREENAVIGATOR_H
#define TTREENAVIGATOR_H

#include <iostream>

#include "TList.h"
#include "TIterator.h"

#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsVertex.h"

class TTreeNavigator : public TObject
{
  //--------------------
  // Instance Members --
  //--------------------

  // the decay tree
  TCandidate *_theTree; //!

  // the lists of unowned pointers
  TList _vertices; //!
  TList _finalCands; //!
  TList _unstableCands; //!

public:

  // ctor and destructor
  TTreeNavigator(TCandidate& theTree );
  virtual ~TTreeNavigator() {}
  TTreeNavigator(const TTreeNavigator &) {}

  // list of vertices
  int NVertex() const;
  TIterator& VertexIterator();
  
  // list of stable final candidates
  int NFinalCand() const;
  TIterator& FinalCandIterator();

  // list of intermediate candidates
  int NUnstableCand() const;
  TIterator& UnstableCandIterator();
  
  // printing
  void PrintOn( std::ostream& o=std::cout ) const;

  // non-recursive tree printing
  static void PrintCand( const TCandidate& );

  // recursive tree printing
  static void PrintTree( const TCandidate& );

  // vertex printing
  static void PrintVertex( const VAbsVertex& );

  // the function isCloneOf returns kTRUE if the given tree is a clone
  // Implemented by F. Martinez-Vidal May/25/99
  // Modified by F. Martinez-Vidal and Jan Stark, August/18/99
  // A tree is clone of another tree if:
  //   1) they have the same number of final candidates, unstable candidates and vertices
  //   2) the final candidates are all clones (see isCloneOf in TCandidate) with the same
  //      mass hyphoteses and the mothers are the same 
  //   3) all unstable candidates match and that the daughters are the same
  Bool_t IsCloneOf(const TCandidate& theOtherTree, Bool_t checkType=kTRUE);

private:

  // recursive initialization
  void fillLists( const TCandidate& );

public:
    ClassDef(TTreeNavigator,1) // Utility class to navigate in a decay tree
};

#endif
