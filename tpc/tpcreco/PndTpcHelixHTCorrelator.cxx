//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcHelixHTCorrelator
//      see PndTpcHelixHTCorrelator.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Johanness Rauch    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcHelixHTCorrelator.h"

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

//#define MCCORR


PndTpcHelixHTCorrelator::PndTpcHelixHTCorrelator(double hdistcut)
  : _hdistcut(hdistcut)
{}


bool
PndTpcHelixHTCorrelator::corr(PndTpcRiemannTrack* trk,
				PndTpcRiemannHit* rhit,
				bool& survive,
				double& matchQuality)
{

  // use ideal correlation for adjusting cuts!!
#ifdef MCCORR

  if(trk->getNumHits()>5)trk->fitAndSort();

  double minWeight(0.8);
  double Tweight, Hweight;
  
  Tweight = trk->mcid().MaxRelWeight();
  Hweight = rhit->cluster()->mcId().MaxRelWeight();
  
  if(trk->mcid().DominantID()==rhit->cluster()->mcId().DominantID() &&
     Tweight>minWeight && Hweight>minWeight){
    survive=true;
    matchQuality=1-Tweight;
  }
  else survive=false;
  return true;
#endif
 
  // check if we have a fit:
  if(!trk->isFitted()) return false;

  double d=trk->distHelix(rhit);
  //std::cout<<"distance to plane: "<< d<<std::endl;
  DebugLogger::Instance()->Histo("HT_helix_dist",d,0,0.2,100);
  d = TMath::Abs(d);
  matchQuality=d;

  double stat = 1.+ 1/TMath::Sqrt(trk->getNumHits()); // "statistical error" of the fit

  if(d>_hdistcut*stat){
    DebugLogger::Instance()->Histo("HT_riemanncuts",8,0,20,20);
    survive=false;
    return true;
  }

  survive=true;
  return true;
}

