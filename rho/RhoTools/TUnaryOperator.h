//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TUnaryOperator.h,v 1.1.1.1 2001-05-04 16:20:09 marcel Exp $
//
// Description:
//	Class TUnaryOperator
//	
//  Base class for unary operators
//  The semantics is ( b is a unary operator, cand a TCandidate )
//
//   o To return a new'd object:
//
//      TCandidate* newCand =  b.create( cand );
//
//   o To return an object by value :
//
//      TCandidate newCand = b( cand ); 
//
//   o To "fill" an existing object :
//
//      b.fill( newCand,  cand );
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen		        Original Author
//      Gautier Hamel de Monchenault
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TUNARYOPERATOR_H
#define TUNARYOPERATOR_H

//----------------------
// Base Class Headers --
//----------------------
#include "RhoTools/TOperatorBase.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "RhoBase/TCandidate.h"

//		---------------------
// 		-- Class Interface --
//		---------------------

class TUnaryOperator  :
  public TOperatorBase
{

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  
  // Destructor
  virtual ~TUnaryOperator( );

  // form that returns a TCandidate by value
  inline TCandidate& operator()( const TCandidate& input );

  // form that fills an object return via a reference
  inline void Fill( TCandidate& cand, const TCandidate& input );

  // form that returns a new object from heap
  virtual TCandidate* Create( const TCandidate& input ) =0;
    
public:
    ClassDef(TUnaryOperator,1) // Base class for unary operators
};

TCandidate&
TUnaryOperator::operator()( const TCandidate& input ) 
{
  TCandidate* cand = Create( input );
  return *cand;
}

void
TUnaryOperator::Fill( TCandidate& cand, const TCandidate& input )
{
  cand = *Create( input );
}


#endif


