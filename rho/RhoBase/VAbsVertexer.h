#ifndef VABSVERTEXER_H
#define VABSVERTEXER_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsVertexer	    							//
//                                                                      //
// Abstract interface for the vertexer					//
//                                                                      //
// Author: Marcel Kunze, RUB, Mar. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

//
// Forward declarations
//
class VAbsVertex;
class TLorentzVector;
class TError;
class TCandListIterator;

class VAbsVertexer : public TObject
{

  //--------------------
  // Public Interface --
  //--------------------

public:

  // the vertex
  virtual VAbsVertex* Compute( TCandListIterator& ) =0;

  // momentum and charge of the decaying state
  virtual TLorentzVector& P4() const =0;
  virtual TError&       P4Err() const =0; 

  // destructor
  virtual ~VAbsVertexer();

  ClassDef(VAbsVertexer,1) // Base class for vertexers
};

#endif 
