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
//      Johannes Rauch       TUM
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcSzHTCorrelator.h"

// C/C++ Headers ----------------------
#include "TVector3.h"
#include "TH1D.h"

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"
#include "DebugLogger.h"

// Class Member definitions -----------

PndTpcSzHTCorrelator::PndTpcSzHTCorrelator(double szcut)
  : _szcut(szcut)
{}


bool
PndTpcSzHTCorrelator::corr(PndTpcRiemannTrack* trk,
				PndTpcRiemannHit* rhit,
				bool& survive,
				double& matchQuality)
{
  // check if we have a fit:
  if(!trk->isFitted())return false;

  double l2=trk->szDist(rhit,true);
  DebugLogger::Instance()->Histo("HT_sz_szDist",l2,-4,4,100);

  if(TMath::Abs(l2)>_szcut){  
    DebugLogger::Instance()->Histo("HT_riemanncuts",3,0,20,20);
    survive=false;
    return true;
  }
  matchQuality=TMath::Abs(l2);

  survive=true;
  return true;
}

