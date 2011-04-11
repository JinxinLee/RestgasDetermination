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
  double l;
  trk->getClosestHit(rhit,l);
  //std::cout<<"distance in 3D: "<<l<<std::endl;
  DebugLogger::Instance()->Histo("HT_prox_l",l,0,5,100);
  matchQuality=l;
  if(l>_proxcut){
    DebugLogger::Instance()->Histo("HT_riemanncuts",1,0,20,20);
    survive=false;
    return true;
  }
  survive=true;
  return true;
}

