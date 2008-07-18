//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOperator.cxx,v 1.3 2002-02-01 23:00:17 marcel Exp $
//
// Description:
//	Class TOperator
//
//	Abstract base class to combine TCandidates to form a new one
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen     		Original Author
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TOperator.h"
#include <assert.h>
#include "RhoBase/TCandidate.h"

ClassImp(TOperator)

TBuffer &operator>>(TBuffer &buf, TOperator *&obj)
{
   obj = (TOperator *) buf.ReadObject(TOperator::Class());
   return buf;
}

#include <iostream>
using namespace std;

TOperator::~TOperator( )
{
}

TCandidate&
TOperator::Combine( const TCandidate& input1,
		      const TCandidate& input2  )
{
  TCandidate* cand = Create( input1, input2 );
  //TCandidate cand(*c);
  //delete c;
  return *cand;
}

TCandidate&
TOperator::Combine( const TCandidate& input1,
		      const TCandidate& input2,
		      const TCandidate& input3  )
{
  TCandidate* cand = Create( input1, input2, input3 );
  //TCandidate cand(*c);
  //delete c;
  return *cand;
}

TCandidate&
TOperator::Combine( const TCandidate& input1,
		      const TCandidate& input2,
		      const TCandidate& input3,
		      const TCandidate& input4  )
{
  TCandidate* cand = Create( input1, input2, input3, input4 );
  //TCandidate cand(*c);
  //delete c;
  return *cand;
}


TCandidate*
TOperator::Create( const TCandidate& input1,
		     const TCandidate& input2  )
{
  TCandList list;
  TCandListIterator iter(list);
  list.Add(input1);
  list.Add(input2);
  //TCandidate cand;
  // fill using the overloaded member function
  return CreateFromList( iter );
}

TCandidate*
TOperator::Create( const TCandidate& input1,
		     const TCandidate& input2,
		     const TCandidate& input3  )
{
  TCandList list;
  TCandListIterator iter(list);
  list.Add(input1);
  list.Add(input2);
  list.Add(input3);
  //TCandidate cand;
  // fill using the overloaded member function
  return CreateFromList( iter );
}

TCandidate*
TOperator::Create( const TCandidate& input1,
		     const TCandidate& input2,
		     const TCandidate& input3,
		     const TCandidate& input4  )
{
  TCandList list;
  TCandListIterator iter(list);
  list.Add(input1);
  list.Add(input2);
  list.Add(input3);
  list.Add(input4);
  //TCandidate cand;
  // fill using the overloaded member function
  return CreateFromList( iter );
}

void
TOperator::FillFromList( TCandidate& cand,
			   TCandListIterator& input )
{
  cand = *CreateFromList( input );
  //cand = *c;
  //delete c;
}

void
TOperator::Fill(  TCandidate& cand,
		    const TCandidate& input1,
		    const TCandidate& input2  )
{
  cand = Combine( input1, input2 );
}

void
TOperator::Fill(  TCandidate& cand,
		    const TCandidate& input1,
		    const TCandidate& input2,
		    const TCandidate& input3  )
{
  cand = Combine( input1, input2, input3 );
}

void
TOperator::Fill(  TCandidate& cand, 
		    const TCandidate& input1,
		    const TCandidate& input2,
		    const TCandidate& input3,
		    const TCandidate& input4  )
{
  cand = Combine( input1, input2, input3, input4 );
}


