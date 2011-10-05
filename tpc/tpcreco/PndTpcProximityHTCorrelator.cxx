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
#define SPEEDUP 5


PndTpcProximityHTCorrelator::PndTpcProximityHTCorrelator(double cut, double zStretch, double helixcut)
  : _proxcut(cut), _zStretch(zStretch), _meandist(0.7), _helixcut(2.*helixcut)
{}


bool
PndTpcProximityHTCorrelator::corr(PndTpcRiemannTrack* trk,
				  PndTpcRiemannHit* rhit,
				  bool& survive,
				  double& matchQuality)
{

  TVector3 posX(rhit->cluster()->pos());

  // fast estimation: distance to circle in 2D
  if (trk->isFitted()){
    double circDist = fabs( (posX - trk->center()).Perp() - trk->r() );
    if ( circDist > _helixcut ){
      matchQuality = circDist;
      survive = false;
      return true;
    }
  }




  unsigned int trksize(trk->getNumHits());

  //scale proxcut with track quality (makes it looser for better defined tracks)
  double proxcut(_proxcut);
  double quality(trk->quality());
  proxcut *= 1 + (3 * quality);


  TVector3 pos, dis3;
  double dis;
  
    
  // check last and first hit for match
  for (unsigned int i=trksize-1; true; i-=i){
    pos = trk->getHit(i)->cluster()->pos();
    dis3 = posX - pos;
    dis3.SetZ(dis3.Z()/_zStretch);
    dis = dis3.Mag();
    if (dis < proxcut){
      matchQuality = dis;
      survive = true;
      return true;
    }
    if (i==0) break;
  }
  
  if (trksize < 3) { // the hit (trksize=1), resp. both hits  (trksize=2) have been checked and did not survive
    matchQuality = dis;
    survive = false;
    return true;
  }




  int closest(-1);

  // now check every SPEEDUP hit    
  double largecut(0.6*SPEEDUP*_meandist + proxcut*_zStretch), mindis(1.E10);
  
  for (unsigned int i=2; i<trksize-1; i+=SPEEDUP){
    pos = trk->getHit(i)->cluster()->pos();
    dis = (posX-pos).Mag();
    if(dis<mindis) {
      mindis = dis;
      if (mindis < largecut) closest = i;
      if (mindis < proxcut && trk->isFitted()){
        matchQuality = mindis;
        survive = true;
        return true;
      }
    }
  }

  if(closest == -1){ // no hit closer than largecut has been found
    matchQuality=mindis;
    survive=false;
    return true;
  }

  

  double l;
  TVector3 dist(posX - trk->getHit( trk->getClosestHit(rhit, l, closest-SPEEDUP-2, closest+SPEEDUP+2) )->cluster()->pos());


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

