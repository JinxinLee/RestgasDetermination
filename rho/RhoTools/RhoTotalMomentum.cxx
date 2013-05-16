//--------------------------------------------------------------------------
// File and Version Information:
//  $Id: RhoTotalMomentum.cc,v 1.1 1997/15/24 10:05:13
//
// Description:
//  Class RhoTotalMomentum - a simple object to calculate the total
//                               4 momentum in an event. More
//                               sophisticated algorithms can be added
//                               later.
//
// Environment:
//  Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Paul Harrison                         Original author
//
// Copyright Information:
//  Copyright (C) 1997                Hedgehog Concepts
//
// ROOT Version by Marcel Kunze, RUB
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              
//------------------------------------------------------------------------

#include "RhoTools/RhoTotalMomentum.h"
#include <assert.h>
#include <math.h>
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoCandListIterator.h"
#include "TLorentzVector.h"
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoEventInfo.h"

ClassImp ( RhoTotalMomentum )

TBuffer& operator>> ( TBuffer& buf, RhoTotalMomentum *&obj )
{
  obj = ( RhoTotalMomentum* ) buf.ReadObject ( RhoTotalMomentum::Class() );
  return buf;
}

#include <iostream>
using namespace std;

RhoTotalMomentum::RhoTotalMomentum ( RhoCandList& list,
                                     const RhoEventInfo& evtinfo )
{

  TLorentzVector tot4mom = TLorentzVector ( 0,0,0,0 );
  const RhoCandidate* trkptr;

  RhoCandListIterator iter ( list );

  // loop over all the RhoCandidates
  while ( ( trkptr = iter.Next() ) ) {
    // Calculate total 4-momentum. More-sophisticated algorithms can
    // be substituted later.
    tot4mom += trkptr->P4();
  }

  _total4Momentum = tot4mom; // set data member
  _total4MomentumCms = _total4Momentum; //intermediate step
  TVector3 cms = evtinfo.CmFrame().BoostVector();
  _total4MomentumCms.Boost ( - ( cms ) ); //do boost

  _missing4Momentum = evtinfo.CmFrame()-tot4mom; // set data member
  _missing4MomentumCms = _missing4Momentum; //intermediate step
  _missing4MomentumCms.Boost ( - ( cms ) ); //do boost
}

RhoTotalMomentum::~RhoTotalMomentum() {}

