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
  int speedupfactor = 3;
  int trksize = trk->getNumHits();
  //std::cout<<"PndTpcProximityHTCorrelator::corr; tracksize: "<<trksize<<std::endl;
  unsigned int i=0;
  int closest = 0;
  // do some fast approximation to speed things up
  if(trksize > 3){
    TVector3 posX = rhit->cluster()->pos();
    TVector3 pos;
    double dis;
    double largecut = speedupfactor*_proxcut;
    bool faraway = true;
    bool check = true;
    while(check){
      pos = trk->getHit(i)->cluster()->pos();
      dis = (posX-pos).Mag();
      //std::cout<<"i "<<i<<"   dis "<<dis<<std::endl;
      if(dis<largecut) {
        faraway=false;
	closest = i;
        check=false;//break loop
      }
      if(i==trksize-1) check=false;  // last hit was checked
      i+=(int)(dis/_proxcut);// step at least speedupfactor  hits
      if(i>trksize-1) i=trksize-1;
    }

    if(faraway){
      matchQuality=largecut;
      survive=false;
      return true;
    }
  }
  
  // get closest hit from track
  double l;
  trk->getClosestHit(rhit,l,closest-2*speedupfactor,closest+3*speedupfactor);
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

