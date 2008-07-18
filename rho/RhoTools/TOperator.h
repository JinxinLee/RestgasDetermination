//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOperator.h,v 1.1.1.1 2001-05-04 16:20:23 marcel Exp $
//
// Description:
//	Class TOperator
//	
//  Base class for operators which "combine" 2 or more candidates
//  The semantics is ( b is a unary operator, cand1, cand2..  TCandidates )
//
//   o To return a new'd object:
//
//      TCandidate* newCand =  b.create( cand1, cand2, ... );
//
//   o To return an object by value :
//
//      TCandidate newCand = b.combine( cand1, cand2, ... ); 
//
//   o To "fill" an existing object :
//
//      b.fill( newCand,  cand1, cand2, ... );
//      b.fillFromList( newCand, iter );
//
//   where iter is an iterator on a TCandidate list
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

#ifndef TOPERATOR_H
#define TOPERATOR_H

//----------------------
// Base Class Headers --
//----------------------
#include "RhoTools/TOperatorBase.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TCandList.h"

//		---------------------
// 		-- Class Interface --
//		---------------------

class TOperator  :
  public TOperatorBase
{

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  
  // Destructor
  virtual ~TOperator();

  // forms that returns a TCandidate by value
  TCandidate& Combine( const TCandidate& input1,
			const TCandidate& input2  );
  TCandidate& Combine( const TCandidate& input1,
			const TCandidate& input2,  
			const TCandidate& input3  );
  TCandidate& Combine( const TCandidate& input1,
			const TCandidate& input2,  
			const TCandidate& input3,
			const TCandidate& input4  );

  // forms that fills an object return via a reference
  void Fill( TCandidate& cand,
	     const TCandidate& input1,
	     const TCandidate& input2 );
  void Fill( TCandidate& cand,
	     const TCandidate& input1,
	     const TCandidate& input2,
	     const TCandidate& input3 );
  void Fill( TCandidate& cand,
	     const TCandidate& input1,
	     const TCandidate& input2,
	     const TCandidate& input3,
	     const TCandidate& input4 );
  void FillFromList( TCandidate& cand,
		     TCandListIterator& input );

  // form that returns a new object from heap
  TCandidate* Create( const TCandidate& input1,
			       const TCandidate& input2 );
  TCandidate* Create( const TCandidate& input1,
			const TCandidate& input2,
			const TCandidate& input3 );
  TCandidate* Create( const TCandidate& input1,
			const TCandidate& input2,
			const TCandidate& input3,
			const TCandidate& input4 );

  // the only function to be overloaded
  virtual TCandidate* CreateFromList( 
		       TCandListIterator& input ) =0;
    
    ClassDef(TOperator,1) // Base class for operators which "combine" 2 or more candidates
};


#endif



