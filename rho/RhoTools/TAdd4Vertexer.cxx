//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TAdd4Vertexer.cxx,v 1.3 2002-02-01 23:00:04 marcel Exp $
//
// Description:
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault
//
// Copyright Information:
//	Copyright (C) 1998		Saclay
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TAdd4Vertexer.h"
#include "RhoBase/TFactory.h"
#include <assert.h>
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandidate.h"

ClassImp(TAdd4Vertexer)

TBuffer &operator>>(TBuffer &buf, TAdd4Vertexer *&obj)
{
   obj = (TAdd4Vertexer *) buf.ReadObject(TAdd4Vertexer::Class());
   return buf;
}

#include <iostream>
using namespace std;

TAdd4Vertexer::TAdd4Vertexer() 
:fP4(),fErr(4,4) 
{
}

TAdd4Vertexer::TAdd4Vertexer( const TVector3& p ) 
:  fP4(),fErr(4,4)
{
}

TAdd4Vertexer::TAdd4Vertexer( const TAdd4Vertexer& o ) 
:  fP4(o.fP4), fErr(o.fErr) 
{
}

//--------------
// Destructor --
//--------------

TAdd4Vertexer::~TAdd4Vertexer( )
{
}

//--------------
// Operations --
//--------------

TLorentzVector 
TAdd4Vertexer::P4() const 
{ 
    return fP4; 
}

TMatrixD&
TAdd4Vertexer::P4Err() 
{ 
    return fErr; 
}

VAbsVertex* 
TAdd4Vertexer::Compute( TCandListIterator& iter )
{
    fP4  = TLorentzVector();
    fErr.Zero();
    TCandidate* cand=0;
    while( cand=iter.Next() )
    {
	fP4 +=cand->P4();
	fErr+=cand->P4Err();
    }
    return TFactory::Instance()->NewVertex();
}


