//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannTTCorrelator
//      see PndTpcRiemannTTCorrelator.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Johannes Rauch       TUM
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcProximityTTCorrelator.h"

// C/C++ Headers ----------------------
#include "TVector3.h"
#include "TH1D.h"

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"
#include "DebugLogger.h"

// Class Member definitions -----------

PndTpcProximityTTCorrelator::PndTpcProximityTTCorrelator(double proxcut)
  : _proxcut(proxcut)
{}


bool
PndTpcProximityTTCorrelator::corr(PndTpcRiemannTrack* trk1,
        PndTpcRiemannTrack* trk2,
				bool& survive,
				double& matchQuality)
{
  //std::cout<<" PndTpcProximityTTCorrelator::corr"<<std::endl;

  TVector3 t1h1 = trk1->getFirstHit()->cluster()->pos();
  TVector3 t1hn = trk1->getLastHit()->cluster()->pos();
  TVector3 t2h1 = trk2->getFirstHit()->cluster()->pos();
  TVector3 t2hn = trk2->getLastHit()->cluster()->pos();

  double dist = (t1hn - t2h1).Mag();
  double d = (t1hn - t2hn).Mag();
  if (d<dist) dist = d;
  d = (t1h1 - t2h1).Mag();
  if (d<dist) dist = d;
  d = (t1h1 - t2hn).Mag();
  if (d<dist) dist = d;

  matchQuality=TMath::Abs(dist);

  if(dist<_proxcut){
    DebugLogger::Instance()->Histo("TT_prox_dist",dist,0,5,100);
    survive=true;
    return true;
  }

  survive=false;
  return true;
}

