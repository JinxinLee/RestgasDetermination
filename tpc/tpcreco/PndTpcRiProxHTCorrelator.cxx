//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannHTCorrelator
//      see PndTpcRiemannHTCorrelator.hh for details
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
#include "PndTpcRiProxHTCorrelator.h"

// C/C++ Headers ----------------------
#include "TVector3.h"
#include "TH1D.h"

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"
#include "DebugLogger.h"

// Class Member definitions -----------

PndTpcRiProxHTCorrelator::PndTpcRiProxHTCorrelator(double proxcut)
  : _proxcut(proxcut)
{}


bool
PndTpcRiProxHTCorrelator::corr(PndTpcRiemannTrack* trk,
				PndTpcRiemannHit* rhit,
				bool& survive,
				double& matchQuality)
{
  // check distance on the riemann sphere
  double dist; // distance in 3D
  double l = (trk->getHit(trk->getClosestHit(rhit,dist))->x() - rhit->x()).Mag();
  //std::cout<<"distance on Riemann Sphere "<< l<<std::endl;
  DebugLogger::Instance()->Histo("HT_riem_prox",l,0,0.2,100);
  if(l>_proxcut){
    DebugLogger::Instance()->Histo("HT_riemanncuts",6,0,20,20);
    survive=false;
    return true;
  }

}

