//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOpMakeTree.h,v 1.3 2002-02-01 03:54:57 marcel Exp $
//
// Description:
//	Class TOpMakeTree
//	
//  Child class of TOperator that allows to build up TCandidate trees
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen		        Original Author
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TOPMAKETREE_H
#define TOPMAKETREE_H

//----------------------
// Base Class Headers --
//----------------------
#include "RhoTools/TOperator.h"

class TAdd4Vertexer;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TOpMakeTree  : 
  public TOperator
{

//--------------------
// Instance Members --
//--------------------

  TAdd4Vertexer* _theVertexer;//!Do not stream

public:

  // Constructors
  TOpMakeTree(TAdd4Vertexer& );
  TOpMakeTree(  );

  // Destructor
  virtual ~TOpMakeTree();

  // Operations
  virtual void PrintOn( std::ostream& o=std::cout ) const;

  // the following _must_ be overriden
  virtual TCandidate* CreateFromList( TCandListIterator &input );
  
  ClassDef(TOpMakeTree,1) // TOperator that allows to build up TCandidate trees
};

// standalone print
std::ostream&  operator << (std::ostream& o, const TOpMakeTree&);

#endif




