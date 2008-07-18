//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TUnaryOperator.cxx,v 1.3 2002-02-01 23:00:24 marcel Exp $
//
// Description:
//	Class TUnaryOperator
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

#include "RhoTools/TUnaryOperator.h"
#include <assert.h>
#include "RhoBase/TCandidate.h"

ClassImp(TUnaryOperator)

TBuffer &operator>>(TBuffer &buf, TUnaryOperator *&obj)
{
   obj = (TUnaryOperator *) buf.ReadObject(TUnaryOperator::Class());
   return buf;
}

#include <iostream>
using namespace std;

TUnaryOperator::~TUnaryOperator( )
{
}


