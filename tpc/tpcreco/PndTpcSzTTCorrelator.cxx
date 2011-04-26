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
#include "PndTpcSzTTCorrelator.h"

// C/C++ Headers ----------------------
#include "TVector3.h"
#include "TH1D.h"
#include "TMath.h"

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"
#include "DebugLogger.h"

// Class Member definitions -----------

PndTpcSzTTCorrelator::PndTpcSzTTCorrelator(double szcut)
  : _szcut(szcut)
{}


bool
PndTpcSzTTCorrelator::corr(PndTpcRiemannTrack* trk1,
        PndTpcRiemannTrack* trk2,
				bool& survive,
				double& matchQuality)
{
  //std::cout<<" PndTpcSzTTCorrelator::corr"<<std::endl;
  if(!trk1->isFitted())return false;

  if(trk2->isFitted()){
    double phi1 = trk1->dip();
    double phi2 = trk2->dip();

    // check if tracks are sorted the same way
    TVector3 t1h1 = trk1->getFirstHit()->cluster()->pos();
    TVector3 t1hn = trk1->getLastHit()->cluster()->pos();
    TVector3 t2h1 = trk2->getFirstHit()->cluster()->pos();
    TVector3 t2hn = trk2->getLastHit()->cluster()->pos();

    double dist = (t1hn - t2h1).Mag();
    bool back2back=false;
    bool back=false;
    double d = (t1hn - t2hn).Mag();
    if (d<dist){dist = d; back2back=true; back=true;}
    d = (t1h1 - t2h1).Mag();
    if (d<dist){dist = d; back2back=true; back=false;}
    d = (t1h1 - t2hn).Mag();
    if (d<dist){dist = d; back2back=false; back=true;}

    // if tracks are not sorted the same way, "flip" the dip
    if(back2back){
      if(phi1>phi2) phi1=TMath::Pi()-phi1;
      else phi2=TMath::Pi()-phi2;
    }

    // compare phi of the s-z line
    double dphi = TMath::Abs(phi2-phi1);
    //dphi*=57.295779513; // rad to °

    matchQuality=TMath::Abs(dphi);
    DebugLogger::Instance()->Histo("TT_sz_dphi",dphi,-4,4,100);

    // check if tracks have equal dip
    if(TMath::Abs(dphi)>_szcut){
      DebugLogger::Instance()->Histo("TT_riemanncuts",3,0,20,20);
      survive=false;
      return true;
    }


    // now also check if sz distance matches
    double szDist;
    if(back)
      szDist = TMath::Abs(trk1->szDist(trk2->getLastHit(),true));
    else
      szDist = TMath::Abs(trk1->szDist(trk2->getFirstHit(),true));

    matchQuality=szDist;
    DebugLogger::Instance()->Histo("TT_sz_szDist",szDist,-4,4,100);

    // check if sz distace small enough
    if(szDist>_szcut){
      DebugLogger::Instance()->Histo("TT_riemanncuts",3,0,20,20);
      survive=false;
      return true;
    }

    survive=true;
    return true;
  }


  // trk2 not fitted: test hit by hit
  double maxSzDist = 0;
  for(unsigned int i=0; i<trk2->getNumHits(); ++i){
    double szDist = TMath::Abs(trk1->szDist(trk2->getHit(i),true));
    if(szDist > maxSzDist) maxSzDist=szDist;
    if(maxSzDist>_szcut) break; // track did not survive!
  }

  matchQuality=maxSzDist;
  DebugLogger::Instance()->Histo("TT_sz_maxSzDist",maxSzDist,-4,4,100);

  if(maxSzDist>_szcut){
    DebugLogger::Instance()->Histo("TT_riemanncuts",3,0,20,20);
    survive=false;
    return true;
  }

  survive=true;
  return true;

}

