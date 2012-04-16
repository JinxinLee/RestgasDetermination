//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TTotalMomentum.cc,v 1.1 1997/15/24 10:05:13 
//
// Description:
//	Class TTotalMomentum - a simple object to calculate the total 
//                               4 momentum in an event. More
//                               sophisticated algorithms can be added
//                               later.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Paul Harrison                         Original author  
//
// Copyright Information:
//	Copyright (C) 1997	              Hedgehog Concepts
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TTotalMomentum.h"
#include <assert.h>
#include <math.h>
#include "RhoBase/TRho.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "TLorentzVector.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TEventInfo.h"

ClassImp(TTotalMomentum)

TBuffer &operator>>(TBuffer &buf, TTotalMomentum *&obj)
{
   obj = (TTotalMomentum *) buf.ReadObject(TTotalMomentum::Class());
   return buf;
}

#include <iostream>
using namespace std;

TTotalMomentum::TTotalMomentum(TCandList& list, 
const TEventInfo& evtinfo) {

    TLorentzVector tot4mom = TLorentzVector(0,0,0,0);
    const TCandidate* trkptr;

    TCandListIterator iter(list);

    // loop over all the TCandidates
    while ( (trkptr = iter.Next()) ) {
      // Calculate total 4-momentum. More-sophisticated algorithms can
      // be substituted later.
      tot4mom += trkptr->P4();
    }

    _total4Momentum = tot4mom; // set data member
    _total4MomentumCms = _total4Momentum; //intermediate step
    TVector3 cms = evtinfo.CmFrame().BoostVector();
    _total4MomentumCms.Boost( -(cms)); //do boost
    
    _missing4Momentum = evtinfo.CmFrame()-tot4mom; // set data member
    _missing4MomentumCms = _missing4Momentum; //intermediate step
    _missing4MomentumCms.Boost( -(cms)); //do boost
}

TTotalMomentum::~TTotalMomentum() {}

