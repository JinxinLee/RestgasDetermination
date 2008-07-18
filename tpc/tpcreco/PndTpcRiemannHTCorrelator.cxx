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

PndTpcRiemannHTCorrelator::PndTpcRiemannHTCorrelator(double planecut, double proxcut, double szcut)
  : _planecut(planecut), _proxcut(proxcut), _szcut(szcut)
{}


bool
PndTpcRiemannHTCorrelator::corr(PndTpcRiemannTrack* trk,
				PndTpcRiemannHit* rhit,
				bool& survive,
				double& matchQuality)
{
  // check if we have a fit:
  if(!trk->isFitted())return false;

  // check if reasonably near the trk
  double ld=999; // distance in 3D
  const PndTpcRiemannHit* trkHit=*(trk->getClosestHit(rhit,ld));
  TVector3 dis=rhit->x()-trkHit->x();
  double l=dis.Mag();
  DebugLogger::Instance()->Histo("riem_l",l,0,0.2,1000);
  if(l>_proxcut){
    DebugLogger::Instance()->Histo("riemanncuts",10,0,20,20);
    survive=false;
    return true;
  }

  double l2=trk->szDist(rhit,true);
  DebugLogger::Instance()->Histo("riem_l2",l2,-4,4,100);
  if(TMath::Abs(l2)>_szcut){  
    DebugLogger::Instance()->Histo("riemanncuts",12,0,20,20);
    survive=false;
    return true;
  }

  double d=trk->dist(rhit);
  DebugLogger::Instance()->Histo("riem_d",d,-1E-2,1E-2,200);
  if(TMath::Abs(d)>_planecut){
    DebugLogger::Instance()->Histo("riemanncuts",11,0,20,20);
    survive=false;
    return true;
  }

  matchQuality=TMath::Abs(d);
  survive=true;
  return true;
}

