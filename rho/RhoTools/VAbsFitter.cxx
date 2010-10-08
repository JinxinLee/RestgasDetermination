//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsFitter				    				//
//                                                                      //
// Abstract class for vertexers						//
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "RhoTools/VAbsFitter.h"
#include "RhoTools/TOpClone.h"
#include "RhoBase/TFactory.h"

ClassImp(VAbsFitter)

TBuffer &operator>>(TBuffer &buf, VAbsFitter *&obj)
{
   obj = (VAbsFitter *) buf.ReadObject(VAbsFitter::Class());
   return buf;
}

#include <iostream>
using namespace std;

VAbsFitter::VAbsFitter( const TCandidate& head ) : fVerbose(kFALSE), fChi2Map(20)
{
    //fHeadOfTree=TFactory::Instance()->NewCandidate(head);
    fHeadOfTree=CopyTree(head);
}

VAbsFitter::~VAbsFitter() 
{
    //delete fHeadOfTree;
}

VAbsFitter::VAbsFitter(const VAbsFitter& other): fVerbose(other.fVerbose)
{
    fHeadOfTree=CopyTree(*other.fHeadOfTree);
}


TCandidate*
VAbsFitter::CopyTree( const TCandidate& b )
{
    TOpClone op;
    return op.Create( b );
}

const TCandidate* 
VAbsFitter::FittedCand( const TCandidate& c ) const 
{
    assert( fHeadOfTree!=0 );
    return fHeadOfTree->CloneInTree( c ); 
}

TCandidate
VAbsFitter::GetFitted(const TCandidate& c ) const 
{
    const TCandidate* fc = FittedCand( c );
    if( fc==0 )
    {
	cerr << " Cannot find the clone of this candidate in the fitted tree. " <<endl;
	return TCandidate();
    } 
    return *fc;
}


Double_t
VAbsFitter::Chi2Contribution( const TCandidate& b ) const
{
    //Double_t chi2;
    return fChi2Map(b.Uid())>=0.0 ? fChi2Map(b.Uid()) : -1;
}


void
VAbsFitter::FitAll()
{
    Fit();
}
