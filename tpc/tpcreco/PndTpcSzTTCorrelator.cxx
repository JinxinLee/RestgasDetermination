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

  if(false){
  //if(trk2->isFitted()){

    double m1 = trk1->m();
    double m2 = trk2->m();

    // compare phi of the s-z line
    double phi1 = TMath::ATan(m1);
    double phi2 = TMath::ATan(m2);
    double dphi = TMath::Abs(phi2-phi1);
    dphi*=57.295779513; // rad to °

    DebugLogger::Instance()->Histo("TT_sz_dphi",dphi,-4,4,100);

    if(TMath::Abs(dphi)>_szcut){
      DebugLogger::Instance()->Histo("TT_riemanncuts",3,0,20,20);
      survive=false;
      return true;
    }

    matchQuality=TMath::Abs(dphi);
    survive=true;
    return true;

  }
  else{ // if trk2 is not fitted yet, see if its hits are compatible

    double maxl2 = 1.E5;
    // loop over hits of trk2
    for(unsigned int i=0; i<trk2->getNumHits(); ++i){
      double l2=trk1->szDist(trk2->getHit(i) ,false);
      if(l2<maxl2) maxl2 = l2;
      if(TMath::Abs(l2)>_szcut){
        DebugLogger::Instance()->Histo("TT_riemanncuts",4,0,20,20);
        survive=false;
        return true;
      }
    }
    DebugLogger::Instance()->Histo("TT_sz_szDist",maxl2,-4,4,100);

    matchQuality=TMath::Abs(maxl2);
    survive=true;
    return true;

  }

}

