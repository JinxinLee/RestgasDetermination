//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannTrackFinder
//      see PndTpcRiemannTrackFinder.h for details
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
#include "PndTpcRiemannTrackFinder.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "DebugLogger.h"
#include "PndTpcClusterRadius.h"
#include "PndTpcClusterZ.h"
#include "GFTrackCand.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcRiemannHTCorrelator.h"

// Class Member definitions -----------


PndTpcRiemannTrackFinder::PndTpcRiemannTrackFinder()
  : _minHitsForFit(5)
{
  // correlators in decreasing priority!
  
  //addCorrelator(new PndTpcProximityHTCorrelator(1.));
  //addCorrelator(new PndTpcRiemannHTCorrelator(5.E-4));
}


PndTpcRiemannTrackFinder::~PndTpcRiemannTrackFinder()
{
  for(int i=0;i<_correlators.size();++i){
    if(_correlators[i]!=NULL){
      delete _correlators[i];
      _correlators[i]=NULL;
    }
  }
  _correlators.clear();

}

void
PndTpcRiemannTrackFinder::addCorrelator(PndTpcAbsHitTrackCorrelator* c)
{
  _correlators.push_back(c);
  _found.push_back(false);
  _bestMatchQuality.push_back(9999);
  _bestMatchIndex.push_back(0);
}



unsigned int
PndTpcRiemannTrackFinder::buildTracks(std::vector<PndTpcCluster*>& cll,
				   std::vector<PndTpcRiemannTrack*>& candlist)
{
  //std::sort(cll.begin(),cll.end(),PndTpcClusterRadius());
  unsigned int ncl=cll.size();

  for(unsigned int icl=0;icl<ncl;++icl){ // loop over clusters
    //if(icl<30)cll[icl]->pos().Print();
    PndTpcRiemannHit* rhit=new PndTpcRiemannHit(cll[icl]);
    unsigned int ntrks=candlist.size();
    unsigned int maxlevel=0; // index of deepest correlator reached
    bool foundAtAll=false;
    //std::cout<<"hit"<<icl<<" testing "<<ntrks<<" tracks"<<std::endl;
    for(unsigned int itrk=0;itrk<ntrks;++itrk){ // loop over tracks
      PndTpcRiemannTrack* trk=candlist[itrk];
      if(trk==NULL)continue;
      // WE STEP THROUGH THE INDIVIDUAL CORRELATORS
      // IF A TRACK SURVIVES EACH CORRELATOR
      // THE HIT IS ASSIGNED TO THE BEST (smallest!) MATCH 
      bool trksurvive=false;
      for(int icor=0;icor<_correlators.size();++icor){ // loop through correlators
	// CORRELATE HIT WITH TRACK
	double matchQuality=99999;
	bool survive=false;
	bool applicable=_correlators[icor]->corr(trk,rhit,survive,matchQuality);
	if(!applicable)continue; // try the next correlator
	if(!survive){
	  trksurvive=false;
	  break; // track has failed this level --> can be excluded
	}
	if(icor<maxlevel)continue; // there are cands that reached deeper level
	maxlevel=icor;
	if(_bestMatchQuality[icor]>matchQuality){
	  _bestMatchQuality[icor]=matchQuality;
	  _bestMatchIndex[icor]=itrk;
	  trksurvive=true;
	}
      } // end loop over correlator
      foundAtAll|=trksurvive;
    } // end loop over tracks

    if(!foundAtAll)// new track
      {
	PndTpcRiemannTrack* trk=new PndTpcRiemannTrack();
	candlist.push_back(trk);
	//std::cout<<"Creating new track"<<std::endl;
	trk->addHit(rhit);
      }
    else {
      // add hit to best match
      // use the bestMatch from deepest level
      // std::cout<<"bestMatch[0]="<<_bestMatchIndex[0]
// 	       <<"   bestMatch[1]="<<_bestMatchIndex[1]<<std::endl;
//       std::cout<<"choosing "<<_bestMatchIndex[maxlevel]<<std::endl;
      PndTpcRiemannTrack* theTrk=candlist[_bestMatchIndex[maxlevel]];
      theTrk->addHit(rhit);
      if(theTrk->getNumHits()>=_minHitsForFit){
	theTrk->refit();
	theTrk->szFit();
      }
    }
    resetFlags();
  } // end loop over cluster
  

  std::cout<<candlist.size()<<" Riemann Tracks found."<<std::endl;
  for(int i=0;i<candlist.size();++i){
    std::cout<<"Track"<<i<<": "
  	     <<candlist[i]->getNumHits()<<" hits  R=";
    if(candlist[i]->getNumHits()>=_minHitsForFit)std::cout<<candlist[i]->r();
    std::cout<<std::endl;
    candlist[i]->Plot(1);
  }
  
 return candlist.size();
}

void
PndTpcRiemannTrackFinder::resetFlags(){
// reset all flags
  for(int k=0;k<_correlators.size();++k){
    _found[k]=false;
    _bestMatchQuality[k]=99999;
    _bestMatchIndex[k]=0;
  }
}

