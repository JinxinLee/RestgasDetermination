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
  int trksize = trk->getNumHits();
  // do some fast approximation to speed things up
  if(trksize > 3){
    TVector3 posX = rhit->cluster()->pos();
    TVector3 pos;
    double dis;
    double largecut = 5*_proxcut;
    bool faraway = true;
    unsigned int i=0;

    while(i<trksize){
      pos = trk->getHit(0)->cluster()->pos();
      dis = (posX-pos).Mag();
      if(dis<largecut) {
        faraway=false;
        break;
      }
      i+=(int)(dis/_proxcut);// step at least 5 hits 
    }
    pos = trk->getHit(trksize-1)->cluster()->pos();  // check last hit
    dis = (posX-pos).Mag();
    if(dis<largecut) faraway=false;
    if(faraway){
      matchQuality=largecut;
      survive=false;
      return true;
    }
  }
  
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

