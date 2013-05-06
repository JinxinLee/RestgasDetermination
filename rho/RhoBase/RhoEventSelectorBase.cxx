//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoEventSelectorBase                                                 //
//                                                                      //
// Base class for event selection                                       //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "RhoBase/RhoEventSelectorBase.h"
#include "RhoBase/RhoCandidate.h"

ClassImp ( RhoEventSelectorBase )

TBuffer& operator>> ( TBuffer& buf, RhoEventSelectorBase *&obj )
{
  obj = ( RhoEventSelectorBase* ) buf.ReadObject ( RhoEventSelectorBase::Class() );
  return buf;
}

#include <iostream>
using namespace std;

void RhoEventSelectorBase::PrintOn ( std::ostream& o ) const
{
  o << GetName() << " settings:";
  if ( fTight ) { o << " Tight mode"; }
  o << endl;
  o << "Number of Tracks        = "  << fNTracksMin << " ... " << fNTracksMax << endl;
  if ( fDocaMax < 1.E8 ) { o << "Primary vertex (d,-z,z) < ("  << fDocaMax << ',' << fZMin << ',' << fZMax << ')' << endl; }
  if ( fTotChar < 65535 ) { o << "Total charge            < " << fTotChar << endl; }
  if ( fETotLow > 0.0 ) { o << "Total energy            = " << fETotLow   << " ... " << fETotHigh << " GeV" << endl; }
  if ( fPTotLow > 0.0 ) { o << "Total momentum          = " << fPTotLow <<   " ... " << fPTotHigh << " GeV" << endl; }
  if ( fELow > 0.0 ) { o << "Track energy            = " << fELow   << " ... " << fEHigh << " GeV" << endl; }
  if ( fPLow > 0.0 ) { o << "Track momentum          = " << fPLow <<   " ... " << fPHigh << " GeV" << endl; }
  if ( fPtLow > 0.0 ) { o << "Track Pt                = " << fPtLow <<   " ... " << fPtHigh << " GeV" << endl; }
  if ( fEmcLow > 0.0 ) { o << "Track EMC energy        = " << fEmcLow << " ... " << fEmcHigh << " GeV" << endl; }
  if ( fThetaLow > 0.0 ) { o << "Track theta window      = " << fThetaLow << " ... " << fThetaHigh << " Rad" << endl; }
  if ( fPhiLow > 0.0 ) { o << "Track phi window        = " << fPhiLow << " ... " << fPhiHigh << " Rad" << endl; }
  o << endl;
}

std::ostream&  operator << ( std::ostream& o, const RhoEventSelectorBase& a )
{
  a.PrintOn ( o );
  return o;
}

