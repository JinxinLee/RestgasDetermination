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

//#define MCCORR

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
#ifdef MCCORR 
  double Tweight, Hweight;
  double minWeight=0.8;

  double weight1 = trk1->mcid().MaxRelWeight();
  double weight2 = trk2->mcid().MaxRelWeight();

  //std::cout<<trk->mcid().DominantID()<<"\n"
  //    <<rhit->cluster()->mcId().DominantID()<<"\n\n";

  if(trk1->mcid().DominantID()==trk2->mcid().DominantID() &&
     weight1>minWeight && weight2>minWeight){
    survive = true;
    matchQuality=1-0.5*(weight1+weight2);
  }
  else survive=false;
  return true;
#endif

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

