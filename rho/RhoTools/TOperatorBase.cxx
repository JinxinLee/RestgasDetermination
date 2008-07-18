//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOperatorBase.cxx,v 1.3 2002-02-01 23:00:18 marcel Exp $
//
// Description:
//	Class TOperatorBase
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
//------------------------------------------------------------------------

#include "RhoTools/TOperatorBase.h"
#include "RhoBase/TFitParams.h"
#include <assert.h>

ClassImp(TOperatorBase)

TBuffer &operator>>(TBuffer &buf, TOperatorBase *&obj)
{
   obj = (TOperatorBase *) buf.ReadObject(TOperatorBase::Class());
   return buf;
}

#include <iostream>
using namespace std;

TOperatorBase::~TOperatorBase( )
{
}

//--------------
// Operations --
//--------------

void 
TOperatorBase::PrintOn( std::ostream& o ) const 
{
  o << "TOperatorBase base class ";
}

std::ostream&  
operator << (std::ostream& o, const TOperatorBase& a) 
{a.PrintOn(o); return o;}

//
// The following may be overridden, but are intended to basically work
//
		     
void 
TOperatorBase::SetP4Err( TCandidate* base, TError err ) const 
{
  //TCandBase* base = contentOf( *temp );
  base->GetFitParams().SetCov7( base->P4Err(), err );
}

