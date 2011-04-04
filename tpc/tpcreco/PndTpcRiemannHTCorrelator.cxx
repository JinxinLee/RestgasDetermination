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

PndTpcRiemannHTCorrelator::PndTpcRiemannHTCorrelator(double planecut, double proxcut)
  : _planecut(planecut), _proxcut(proxcut)
{}


bool
PndTpcRiemannHTCorrelator::corr(PndTpcRiemannTrack* trk,
				PndTpcRiemannHit* rhit,
				bool& survive,
				double& matchQuality)
{
  // check if we have a fit:
  if(!trk->isFittedPlane())return false;

  // check if reasonably near the trk
  double ld=999; // distance in 3D
  const PndTpcRiemannHit* trkHit=*(trk->getClosestHit(rhit,ld));
  TVector3 dis=rhit->x()-trkHit->x();
  double l=dis.Mag();
  //std::cout<<"distance on Riemann Sphere "<< l<<std::endl;
  DebugLogger::Instance()->Histo("HT_riem_prox",l,0,0.2,100);
  if(l>_proxcut){
    DebugLogger::Instance()->Histo("HT_riemanncuts",6,0,20,20);
    survive=false;
    return true;
  }


  double d=trk->dist(rhit);
  DebugLogger::Instance()->Histo("HT_riem_dist",d,0,0.2,100);
  if(TMath::Abs(d)>_planecut){
    DebugLogger::Instance()->Histo("HT_riemanncuts",7,0,20,20);
    survive=false;
    return true;
  }
  matchQuality=TMath::Abs(d);

  survive=true;
  return true;
}

