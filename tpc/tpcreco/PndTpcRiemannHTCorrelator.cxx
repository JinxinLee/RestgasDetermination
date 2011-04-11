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
#include "PndTpcRiemannHTCorrelator.h"

// C/C++ Headers ----------------------
#include "TVector3.h"
#include "TH1D.h"

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"
#include "DebugLogger.h"

// Class Member definitions -----------

PndTpcRiemannHTCorrelator::PndTpcRiemannHTCorrelator(double planecut)
  : _planecut(planecut)
{}


bool
PndTpcRiemannHTCorrelator::corr(PndTpcRiemannTrack* trk,
				PndTpcRiemannHit* rhit,
				bool& survive,
				double& matchQuality)
{
  // check if we have a fit:
  if(!trk->isFittedPlane())return false;

  double d=trk->dist(rhit);
  //std::cout<<"distance to plane: "<< d<<std::endl;
  DebugLogger::Instance()->Histo("HT_riem_dist",d,0,0.2,100);
  d = TMath::Abs(d);
  matchQuality=d;
  if(d>_planecut){
    DebugLogger::Instance()->Histo("HT_riemanncuts",7,0,20,20);
    survive=false;
    return true;
  }

  survive=true;
  return true;
}

