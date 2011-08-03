//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannTTCorrelator
//      see PndTpcDipTTCorrelator.hh for details
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
#include "PndTpcDipTTCorrelator.h"

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

PndTpcDipTTCorrelator::PndTpcDipTTCorrelator(double dipcut, double helixcut)
  : _dipcut(dipcut), _helixcut(helixcut)
{}


bool
PndTpcDipTTCorrelator::corr(PndTpcRiemannTrack* trk1,
        PndTpcRiemannTrack* trk2,
				bool& survive,
				double& matchQuality)
{
  //std::cout<<" PndTpcDipTTCorrelator::corr"<<std::endl;
  if(!trk1->isFitted())return false;

  if(trk2->isFitted()){
    double phi1 = trk1->dip();
    double phi2 = trk2->dip();

    unsigned int nh1 = trk1->getNumHits();
    unsigned int nh2 = trk2->getNumHits();

    // check if tracks are sorted the same way
    TVector3 t1h1 = trk1->getFirstHit()->cluster()->pos();
    TVector3 t1hn = trk1->getLastHit()->cluster()->pos();
    TVector3 t2h1 = trk2->getFirstHit()->cluster()->pos();
    TVector3 t2hn = trk2->getLastHit()->cluster()->pos();

    double d1n21 = (t1hn - t2h1).Mag();
    double d1n2n = (t1hn - t2hn).Mag();
    double d1121 = (t1h1 - t2h1).Mag();
    double d112n = (t1h1 - t2hn).Mag();
    double dist = d1n21;

    bool back1(true), back2(false);
    if (d1n2n<dist){dist = d1n2n; back1=true;  back2=true;}
    if (d1121<dist){dist = d1121; back1=false; back2=false;}
    if (d112n<dist){dist = d112n; back1=false; back2=true;}

    // check if we have to flip one dip
    TVector3 pos1, dir1, pos2, dir2;
    if(back1) trk1->getPosDirOnHelix(nh1-1, pos1, dir1);
    else trk1->getPosDirOnHelix(0, pos1, dir1);

    if(back2) trk2->getPosDirOnHelix(nh2-1, pos2, dir2);
    else trk2->getPosDirOnHelix(0, pos2, dir2);

    // if tracks are not sorted the same way, "flip" the dip
    if(dir1*dir2<0){
      if(phi1>phi2) phi1=TMath::Pi()-phi1;
      else phi2=TMath::Pi()-phi2;
    }

    double dphi = TMath::Abs(phi2-phi1);

    //std::cout<<"dphi "<<dphi<<"\n";

    DebugLogger::Instance()->Histo("TT_dip_dphi",dphi,-4,4,100);
    
    matchQuality=dphi;
    
    // check if tracks have equal dip
    if(dphi>_dipcut){
      DebugLogger::Instance()->Histo("TT_riemanncuts",3,0,20,20);
      survive=false;
      return true;
    }


    // now also check if helix distance matches
    PndTpcCluster* TestCluster = new PndTpcCluster(pos2,1,0);
    PndTpcRiemannHit* TestHit = new PndTpcRiemannHit(TestCluster);
    double hDist(trk1->distHelix(TestHit,true, true));

    delete TestHit;
    delete TestCluster;

    //std::cout<<"hDist "<<hDist<<"\n";

    DebugLogger::Instance()->Histo("TT_dip_hDist",hDist,-10,10,100);

    // check if sz distace small enough
    if(hDist>_helixcut){
      DebugLogger::Instance()->Histo("TT_riemanncuts",4,0,20,20);
      survive=false;
      return true;
    }

    survive=true;
    return true;
  }


  // trk2 not fitted: test hit by hit
  double maxhDist = 0;
  for(unsigned int i=0; i<trk2->getNumHits(); ++i){
    double hDist = TMath::Abs(trk1->distHelix(trk2->getHit(i),true));
    if(hDist > maxhDist) maxhDist=hDist;
    if(maxhDist>_helixcut) break; // track did not survive!
  }

  DebugLogger::Instance()->Histo("TT_dip_hDist",maxhDist,-10,10,100);
  
  matchQuality=maxhDist;

  if(maxhDist>_helixcut){
    DebugLogger::Instance()->Histo("TT_riemanncuts",4,0,20,20);
    survive=false;
    return true;
  }

  survive=true;
  return true;

}

