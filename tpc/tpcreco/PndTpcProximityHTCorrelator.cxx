//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcProximityHTCorrelator
//      see PndTpcProximityHTCorrelator.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcProximityHTCorrelator.h"

// C/C++ Headers ----------------------
#include "TVector3.h"

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"
#include "DebugLogger.h"

// Class Member definitions -----------

PndTpcProximityHTCorrelator::PndTpcProximityHTCorrelator(double cut)
  : _proxcut(cut)
{}


bool
PndTpcProximityHTCorrelator::corr(PndTpcRiemannTrack* trk,
				  PndTpcRiemannHit* rhit,
				  bool& survive,
				  double& matchQuality)
{
  const PndTpcCluster* cl=rhit->cluster();
  if(cl==NULL)return false; // not applicable
  // get closest hit from track
  double l=1000;
  trk->getClosestHit(rhit,l);
  DebugLogger::Instance()->Histo("prox_l",l,0,5,100);
  // TODO: look at errors properly!
  if(l>_proxcut){
    DebugLogger::Instance()->Histo("riemanncuts",9,0,20,20);
    survive=false;
    return true;
  }
  matchQuality=l;
  survive=true;
  return true;
}

