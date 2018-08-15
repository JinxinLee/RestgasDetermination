//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoEventInfo                                                         //
//                                                                      //
// Contains and provides access to summary information in the event     //
// useful for analysis                                                  //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Dec. 2k                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/RhoEventInfo.h"
#include <assert.h>

ClassImp ( RhoEventInfo )

#include <iostream>
using namespace std;

RhoEventInfo::RhoEventInfo() :
  fMCIP ( 0 ),fBeamSpot ( RhoVector3Err() ),fPrimaryVertex (),fEventShape ( 0 )
{}

//--------------
// Destructor --
//--------------

RhoEventInfo::~RhoEventInfo( )
{
  delete fPrimaryVertex;
  delete fMCIP;
}

void
RhoEventInfo::SetPrimaryVertex ( RhoVector3Err* inVtx )
{
  delete fPrimaryVertex;
  fPrimaryVertex = inVtx;
//    fPrimaryVertex = ( inVtx == 0 ) ? ( VAbsVertex* ) 0 : ( VAbsVertex* ) inVtx->Clone();
}

const RhoVector3Err
RhoEventInfo::PrimaryVertex() const
{
  return ( fPrimaryVertex == 0 ) ?  RhoVector3Err ( TVector3 ( 0,0,0 ) ) :
         RhoVector3Err ( *((TVector3*)fPrimaryVertex),fPrimaryVertex->CovMatrix() );
}
void
RhoEventInfo::SetIPTruth ( const TLorentzVector& inVtx )
{
  fMCIP=new TVector3 ( inVtx.X(),inVtx.Y(),inVtx.Z() );
}

void
RhoEventInfo::SetIPTruth ( const TVector3& inVtx )
{
  fMCIP=new TVector3 ( inVtx );
}

void
RhoEventInfo::SetEventShape ( RhoEventShape* eventShape )
{
  fEventShape = eventShape;
}
//--------------
// Operations --
//--------------

void RhoEventInfo::PrintOn ( std::ostream&  ) const
{
}

std::ostream&  operator << ( std::ostream& o, const RhoEventInfo& a )
{
  a.PrintOn ( o );
  return o;
}


