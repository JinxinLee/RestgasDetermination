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

//#define MCCORR // use ideal correlation for adjusting cuts!!
#define SPEEDUP 8


PndTpcProximityHTCorrelator::PndTpcProximityHTCorrelator(double cut, double zStretch)
  : _proxcut(cut), _zStretch(zStretch), _meandist(0.7)
{}


bool
PndTpcProximityHTCorrelator::corr(PndTpcRiemannTrack* trk,
				  PndTpcRiemannHit* rhit,
				  bool& survive,
				  double& matchQuality)
{

  TVector3 posX(rhit->cluster()->pos());


  // fast estimation
  /*double hitZ(posX.Z());
  double dz0(hitZ - trk->getFirstHit()->z());
  double dz1(hitZ - trk->getLastHit()->z());

  if (dz0*dz1 > 0){ // hit does not lie in track in z space
    if( fabs(dz0) > 2*_proxcut && fabs(dz1) > 2*_proxcut ){
      matchQuality = 2*_proxcut;
      survive = false;
      //std::cout<<"failed 1st check\n";
      return true;
    }
    else if (trk->isFitted()){
      survive=true;
      return true;
    }
  }*/


  unsigned int trksize(trk->getNumHits());

  // fast estimation: distance to cirlce in 2D
  /*if (trk->isFitted() && trksize > 7){
    double radius(trk->r());
    if (radius > 2. && radius < 100){
      double circDist = fabs( (posX - trk->center()).Perp() - radius );
      if ( circDist > 2*_proxcut ){
        matchQuality=circDist;
        survive = false;
        std::cout<<"failed 2nd check\n";
        return true;
      }
    }
  }*/



  double quality(trk->quality());

  //scale proxcut with track quality (makes it looser for better defined tracks)
  double proxcut(_proxcut);
  proxcut *= 1 + (2 * quality);

  unsigned int i(0);
  int closest(0);
  int step(0);

  // do some fast approximation to speed things up
  if(trksize > 3){
    TVector3 pos;
    double dis;
    double largecut(SPEEDUP*_meandist);
    //if (largecut < 1.5*proxcut) largecut = 1.5*proxcut;

    bool faraway(true);

    while(true){
      pos = trk->getHit(i)->cluster()->pos();
      dis = (posX-pos).Mag();
      //std::cout<<"i "<<i<<"   dis "<<dis<<std::endl;
      if(dis<largecut) {
        faraway=false;
        closest = i;
        break;
      }
      if(i==trksize-1) break;  // last hit was checked
      step=(int)(dis/_meandist);
      i+=step;
      if(i>trksize-1) i=trksize-1;
    }

    if(faraway){
      matchQuality=largecut;
      survive=false;
      //std::cout<<"failed 2nd check\n";
      return true;
    }
  }
  
  if(step<SPEEDUP) step=SPEEDUP;

  // get closest hit from track
  double l;
  TVector3 dist(posX - trk->getHit( trk->getClosestHit(rhit, l, closest-2*step, closest+2*step) )->cluster()->pos());


// use ideal correlation for adjusting cuts!!
#ifdef MCCORR

  double minWeight(0.8);

  //dist.Print();

  if (dist.Mag()>proxcut){
    survive=false;
    return true;
  }

  double Tweight, Hweight;

  Tweight = trk->mcid().MaxRelWeight();
  Hweight = rhit->cluster()->mcId().MaxRelWeight();

  //std::cout<<trk->mcid().DominantID()<<"\n"
  //    <<rhit->cluster()->mcId().DominantID()<<"\n\n";

  if(trk->mcid().DominantID()==rhit->cluster()->mcId().DominantID() &&
     Tweight>minWeight && Hweight>minWeight){
    DebugLogger::Instance()->Histo3D("HT_prox_true",dist.X(),dist.Y(),dist.Z(), -5,5,100, -5,5,100, -5,5,100);
    DebugLogger::Instance()->Histo("HT_prox_true_X",dist.X(),-10,10,1000);
    DebugLogger::Instance()->Histo("HT_prox_true_Y",dist.Y(),-10,10,1000);
    DebugLogger::Instance()->Histo("HT_prox_true_Z",dist.Z(),-10,10,1000);
    matchQuality=1-Tweight;
    survive=true;
    //std::cout<<"    survive!"<<std::endl;
  }
  else{
    DebugLogger::Instance()->Histo3D("HT_prox_false",dist.X(),dist.Y(),dist.Z(), -5,5,100, -5,5,100, -5,5,100);
    DebugLogger::Instance()->Histo("HT_prox_false_X",dist.X(),-10,10,1000);
    DebugLogger::Instance()->Histo("HT_prox_false_Y",dist.Y(),-10,10,1000);
    DebugLogger::Instance()->Histo("HT_prox_false_Z",dist.Z(),-10,10,1000);
    survive=false;
  }
  return true;
#endif

  matchQuality=l; // unaltered distance in 3D

  dist.SetZ(dist.Z()/_zStretch);
  l = dist.Mag();

  //std::cout<<"distance in 3D: "<<l<<std::endl;
  DebugLogger::Instance()->Histo("HT_prox_l",l,0,5,100);
  if(l>proxcut){
    DebugLogger::Instance()->Histo("HT_riemanncuts",1,0,20,20);
    survive=false;
    //std::cout<<"failed 4th check\n";
    return true;
  }
  survive=true;
  //std::cout<<"passed\n";
  return true;
}

