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
#include "PndTpcRiemannTTCorrelator.h"

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

PndTpcRiemannTTCorrelator::PndTpcRiemannTTCorrelator(double planecut, int minHitsForFit)
  : _planecut(planecut), _minHitsForFit(minHitsForFit)
{}


bool
PndTpcRiemannTTCorrelator::corr(PndTpcRiemannTrack* trk1,
        PndTpcRiemannTrack* trk2,
				bool& survive,
				double& matchQuality)
{
  //std::cout<<" PndTpcRiemannTTCorrelator::corr"<<std::endl;

  // check scale
  if(trk1->getScale()!=trk2->getScale()){
    survive=false;
    return false;
  }

  unsigned int nhits1 = trk1->getNumHits();
  unsigned int nhits2 = trk2->getNumHits();
  unsigned int nhits = nhits1+nhits2;

  if(nhits<_minHitsForFit)
    return false;

  // fill hits into new RiemannTrack and refit
  PndTpcRiemannTrack* mergedTrack = new PndTpcRiemannTrack(trk1->getScale());
  mergedTrack->setSort(false); // it's faster and we don't need it anyway
  for(unsigned int i=0; i<nhits1; ++i){
    mergedTrack->addHit(trk1->getHit(i));
  }
  for(unsigned int i=0; i<nhits2; ++i){
    mergedTrack->addHit(trk2->getHit(i));
  }
  mergedTrack->refit();

  double rms = mergedTrack->planeRMS();

  DebugLogger::Instance()->Histo("TT_riem_rms",rms,0,0.005,100);

  if(rms>_planecut){
    DebugLogger::Instance()->Histo("TT_riemanncuts",6,0,20,20);
    survive=false;
    return true;
  }

  matchQuality=rms;
  survive=true;

  delete mergedTrack;

  return true;
}

