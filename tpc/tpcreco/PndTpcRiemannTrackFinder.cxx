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
//      Johannes Rauch       TUM
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
//#include "PndTpcClusterRadius.h"
#include "PndTpcClusterZ.h"
#include "GFTrackCand.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcRiemannHTCorrelator.h"
#include "PndTpcRiemannTTCorrelator.h"

using namespace std;

#define DEBUG

// Class Member definitions -----------
PndTpcRiemannTrackFinder::PndTpcRiemannTrackFinder()
  : _minHitsForFit(5), _sortingMode(false), 
    _sorting(3), _interactionZ(0.), _MaxNumHitsForPR(2147483646), fRiemannScale(24.6)
{   
}

PndTpcRiemannTrackFinder::PndTpcRiemannTrackFinder(double scale)
  : _minHitsForFit(5), _sortingMode(false), 
    _sorting(3), _interactionZ(0.), _MaxNumHitsForPR(2147483646), fRiemannScale(scale)
{   
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

  for(int i=0;i<_TTcorrelators.size();++i){
    if(_TTcorrelators[i]!=NULL){
      delete _TTcorrelators[i];
      _TTcorrelators[i]=NULL;
    }
  }
  _TTcorrelators.clear();
}

void
PndTpcRiemannTrackFinder::addCorrelator(PndTpcAbsHitTrackCorrelator* c)
{
  _correlators.push_back(c);
  _found.push_back(false);
  _bestMatchQuality.push_back(9999);
  _bestMatchIndex.push_back(0);
}

void
PndTpcRiemannTrackFinder::addTTCorrelator(PndTpcAbsTrackTrackCorrelator* c)
{
  _TTcorrelators.push_back(c);
}


unsigned int
PndTpcRiemannTrackFinder::buildTracks(std::vector<PndTpcCluster*>& cll,
				   std::vector<PndTpcRiemannTrack*>& candlist)
{
  unsigned int ncl=cll.size();
  if(ncl<3) return 0;

  sortClusters(cll);
  int ncor = _correlators.size();

  #ifdef DEBUG
    if(_MaxNumHitsForPR<ncl) ncl=_MaxNumHitsForPR;
  #endif

  for(unsigned int icl=0;icl<ncl;++icl){ // loop over hits
    #ifdef DEBUG
        if(icl%1000==0){
          cout << "At cluster " << icl << endl;
          cout << "Active Tracklets: "<< candlist.size() << endl;
          cout << "Mean number of hits/track: "<< (double)icl/(double)candlist.size() << endl;
        }
    #endif

    PndTpcRiemannHit* rhit=new PndTpcRiemannHit(cll[icl],fRiemannScale);
    unsigned int ntrks=candlist.size();
    unsigned int maxlevel=0; // index of track with highest number of applicable correlators
    bool foundAtAll=false; 
    for(int i=0;i<ncor;++i) _bestMatchQuality[i] = 99999.;// reset 

    for(unsigned int itrk=0;itrk<ntrks;++itrk){ // loop over tracks
      PndTpcRiemannTrack* trk=candlist[itrk];
      if(trk==NULL)continue;

      // WE STEP THROUGH THE INDIVIDUAL CORRELATORS
      // IF A TRACK SURVIVES EACH CORRELATOR
      // THE HIT IS ASSIGNED TO THE BEST (smallest!) MATCH 
      
      bool trksurvive = false;
      std::vector<double> matchQualities(ncor, 99999.); // for saving the match qualities for each correlator
      int level = 0; // number of survived correlators
      #ifdef DEBUG
        if(icl==_MaxNumHitsForPR-1) std::cout<<"Testing hit "<<icl<<" with track "<<itrk<<std::endl;
      #endif

      for(int icor=0;icor<ncor;++icor){ // loop through correlators
        // CORRELATE HIT WITH TRACK
        double matchQuality = 99999;
        bool survive=false;
        bool applicable=_correlators[icor]->corr(trk,rhit,survive,matchQuality);
        #ifdef DEBUG
          if(icl==_MaxNumHitsForPR-1){
            if(!applicable){std::cout<<"  correlator "<<icor<<" NOT applicable"<<std::endl;}
            else{std::cout<<"  correlator "<<icor<<"  IS applicable; survived "<<survive<<" with MatchQuality "<<matchQuality<<std::endl;}
          }
        #endif
        if(!applicable)continue; // try the next correlator
        if(!survive){
          trksurvive=false;
          break; // track has failed this level --> can be excluded
        }
        // track survived this correlator
        level = icor;
        trksurvive = true;
        matchQualities[icor] = matchQuality;
      } // end loop over correlator


      if(trksurvive){ // update best values
        if(level>maxlevel) maxlevel=level;
        for(unsigned int i=0; i<=level; ++i){
          if(matchQualities[i]<_bestMatchQuality[i]){
            _bestMatchQuality[i]=matchQualities[i];
            _bestMatchIndex[i]=itrk;
          }
        }
      }

      #ifdef DEBUG
      if(icl==_MaxNumHitsForPR-1 && trksurvive) std::cout<<" Track "<<itrk<<" survived with level "<<level<<std::endl;
        if(icl==_MaxNumHitsForPR-1) std::cout<<std::endl;
      #endif

      foundAtAll|=trksurvive; // foundAtAll will be true if at least one track survived
    } // end loop over tracks


    #ifdef DEBUG
      if(icl==_MaxNumHitsForPR-1){
        std::cout<<"maxlevel "<< maxlevel <<std::endl;
        std::cout<<"_bestMatchIndex[maxlevel] "<<_bestMatchIndex[maxlevel] <<std::endl;
        std::cout<<"_bestMatchQuality[maxlevel] "<<_bestMatchQuality[maxlevel] <<std::endl;
      }
    #endif


    if(!foundAtAll){ // new track if no track survived
      PndTpcRiemannTrack* trk=new PndTpcRiemannTrack(fRiemannScale);
      trk->setSort(_sortingMode);
      candlist.push_back(trk);
      //std::cout<<"Creating new track"<<std::endl;
      trk->addHit(rhit);
      #ifdef DEBUG
        if(icl==_MaxNumHitsForPR-1) std::cout<<"-> creating new track Nr "<<candlist.size()-1<<std::endl;
      #endif
    }
    else {
      // add hit to best match
      // use the bestMatch from deepest level
      // std::cout<<"bestMatch[0]="<<_bestMatchIndex[0]
// 	       <<"   bestMatch[1]="<<_bestMatchIndex[1]<<std::endl;
//       std::cout<<"choosing "<<_bestMatchIndex[maxlevel]<<std::endl;
      PndTpcRiemannTrack* theTrk=candlist[_bestMatchIndex[maxlevel]];
      #ifdef DEBUG
        if(icl==_MaxNumHitsForPR-1) std::cout<<"-> adding hit to track"<<_bestMatchIndex[maxlevel]<<std::endl;
      #endif
      theTrk->addHit(rhit);
      if(theTrk->getNumHits()>=_minHitsForFit){
        theTrk->refit();
        theTrk->szFit();
      #ifdef DEBUG
        if(icl==_MaxNumHitsForPR-1){
         std::cout<<" track parameters: _c="<<theTrk->c()<<"  R="<<theTrk->r()<<"  dip="<<theTrk->dip()<<std::endl;
        } 
      #endif
      }
      // TODO: this is a test
      /*if(theTrk->getNumHits()>=2*_minHitsForFit && 1){
        theTrk->coolDown(_planecut, _szcut);
      }*/
    }
    resetFlags();
  } // end loop over hits
  
  std::cout<<candlist.size()<<" Riemann Tracks found."<<std::endl;
  /*for(int i=0;i<candlist.size();++i){
    std::cout<<"Track"<<i<<": "
  	     <<candlist[i]->getNumHits()<<" hits  R=";
    if(candlist[i]->getNumHits()>=_minHitsForFit)std::cout<<candlist[i]->r();
    std::cout<<std::endl;
    //candlist[i]->Plot(1);
  }*/

 return candlist.size();
}

void
PndTpcRiemannTrackFinder::mergeTracks(std::vector<PndTpcRiemannTrack*>& candlist){
  std::cout<<"PndTpcRiemannTrackFinder::mergeTracks"<<std::endl;

  unsigned int ntr=candlist.size();
  if (ntr<2) return; // need at least 2 trackcands to merge

  // sort tracklets, but use different sorting than for clusters!
  sortTracklets(candlist);


  for(unsigned int itrk1=0; itrk1<ntr-1; ++itrk1){ // loop over tracks
    if(candlist[itrk1]==NULL)continue;
    PndTpcRiemannTrack* trk1=candlist[itrk1];

    for(unsigned int itrk2=itrk1+1; itrk2<ntr; ++itrk2){ // loop over the other tracks to be tested
      if(candlist[itrk2]==NULL)continue;

      /*#ifdef DEBUG
        std::cout<<"Testing track "<<itrk1<<" with track "<<itrk2<<std::endl;
      #endif*/

      PndTpcRiemannTrack* trk2=candlist[itrk2];

      // WE STEP THROUGH THE INDIVIDUAL CORRELATORS
      // IF THE TRACK trk2 SURVIVES EACH CORRELATOR
      // IT IS MERGED WITH THE TRACK trk1
      bool survive=true;
      for(unsigned int icor=0; icor<_TTcorrelators.size(); ++icor){ // loop through correlators
        // CORRELATE trk1 WITH trk2
        // all correlators must be applicable and must be survived!
        double matchQuality;
        bool applicable;

        // make sure that the first track in the correlator is the bigger one
        if (trk1->getNumHits()>=trk2->getNumHits())
          applicable=_TTcorrelators[icor]->corr(trk1,trk2,survive,matchQuality);
        else
          applicable=_TTcorrelators[icor]->corr(trk2,trk1,survive,matchQuality);

        /*#ifdef DEBUG
          if(!applicable){std::cout<<"  correlator "<<icor<<" NOT applicable"<<std::endl;}
          else{std::cout<<"  correlator "<<icor<<"  IS applicable; survived "<<survive<<" with MatchQuality "<<matchQuality<<std::endl;}
        #endif*/

        if(!applicable) {
          survive = false;
          break;
        }
        if(!survive) break;
      } // end loop through correlators
      if (!survive) continue; // test next trk2

      // merge tracks if survived
      #ifdef DEBUG
        std::cout<<"merge track "<<itrk1<<" with track "<<itrk2<<std::endl;
      #endif
      unsigned int nhits1 = trk1->getNumHits();
      unsigned int nhits2 = trk2->getNumHits();

      if(!_sortingMode){ // we have to collect clusters from both tracks, sort them and build a new track
        // collect clusters from both tracks and sort
        std::vector<PndTpcCluster*> clusters;
        for(unsigned int i=0; i<nhits1; ++i){
          clusters.push_back(trk1->getHit(i)->cluster());
        }
        for(unsigned int i=0; i<nhits2; ++i){
          clusters.push_back(trk2->getHit(i)->cluster());
        }
        sortClusters(clusters);

        // fill clusters into new RiemannTrack and refit
        PndTpcRiemannTrack* mergedTrack = new PndTpcRiemannTrack(fRiemannScale);
        mergedTrack->setSort(false);
        for(unsigned int i=0; i<clusters.size(); ++i){
          PndTpcRiemannHit* rhit = new PndTpcRiemannHit(clusters[i],
							fRiemannScale);
          mergedTrack->addHit(rhit);
        }
        if(mergedTrack->getNumHits()>=_minHitsForFit){
          mergedTrack->refit();
          mergedTrack->szFit();
        }

        // delete old trackcands and store new trackcand
        delete candlist[itrk1];
        trk1=mergedTrack;
        candlist[itrk1]=mergedTrack;
      }
      else{ // we can just add the hits from trk2 to trk1 and the sorting is done internally
        // check if beginning or end of trk2 is closer to trk1
        TVector3 t1h1 = trk1->getFirstHit()->cluster()->pos();
        TVector3 t1hn = trk1->getLastHit()->cluster()->pos();
        TVector3 t2h1 = trk2->getFirstHit()->cluster()->pos();
        TVector3 t2hn = trk2->getLastHit()->cluster()->pos();

        double dist = (t1hn - t2h1).Mag();
        bool back=false;
        double d = (t1hn - t2hn).Mag();
        if (d<dist){dist = d; back=true;}
        d = (t1h1 - t2h1).Mag();
        if (d<dist){dist = d; back=false;}
        d = (t1h1 - t2hn).Mag();
        if (d<dist) back=true;

        if(back){
          for(unsigned int i=nhits2; i>0; --i)
            trk1->addHit(trk2->getHit(i-1));
        }
        else{
          for(unsigned int i=0; i<nhits2; ++i)
            trk1->addHit(trk2->getHit(i));
        }

        // refit if we have enough hits
        if(trk1->getNumHits()>=_minHitsForFit){
          trk1->refit();
          trk1->szFit();
        }
      }

      // delete trk2
      delete candlist[itrk2];
      candlist[itrk2]=NULL;

    } // end loop over the other tracks to be tested
  } // end loop over tracks

  // clean up candlist
  for(int i=0;i<candlist.size();++i){
    if (candlist[i]==NULL) {
      candlist.erase(candlist.begin()+i);
      --i; // go one step back because "erase" shifts back the rest
    }
  }

  std::cout<<candlist.size()<<" Merged Riemann Tracks: "<<std::endl;
  /*for(int i=0;i<candlist.size();++i){
    std::cout<<"Track "<<i<<": "
         <<candlist[i]->getNumHits()<<" hits  R=";
    if(candlist[i]->getNumHits()>=_minHitsForFit)std::cout<<candlist[i]->r();
    std::cout<<std::endl;
  }*/

}


void
PndTpcRiemannTrackFinder::sortClusters(std::vector<PndTpcCluster*>& cll){
  if(_sorting==-1) return;
  sortClusterClass sortCluster;
  sortCluster.setSorting(_sorting);
  sortCluster.setInteractionZ(_interactionZ);
  std::sort(cll.begin(),cll.end(),sortCluster);
}

void
PndTpcRiemannTrackFinder::sortTracklets(std::vector<PndTpcRiemannTrack*>& tracklets){
  if(_sorting==-1) return;
  sortTrackletsClass sortTracklet;
  sortTracklet.setSorting(_sorting);
  std::sort(tracklets.begin(),tracklets.end(),sortTracklet);
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


bool
sortClusterClass::operator() (PndTpcCluster* s1, PndTpcCluster* s2){
  double a1;
  double a2;
  TVector3 d1;
  TVector3 d2;
  switch (sorting){
    case -1: //no sorting
      return false;
      break;
    case 0:
      a1=s1->pos().X();
      a2=s2->pos().X();
      return a1>a2;
      break;
    case 1:
      a1=s1->pos().Y();
      a2=s2->pos().Y();
      return a1>a2;
      break;
    case 2:
      a1=s1->pos().Z();
      a2=s2->pos().Z();
      return a1>a2;
      break;
    case 4:
      d1 = s1->pos();
      d1(2) -= interactionZ;
      d2 = s2->pos();
      d2(2) -= interactionZ;
      a1=d1.Mag();
      a2=d2.Mag();
      return a1>a2;
      break;
    case 3:
    default:
      a1=s1->pos().Perp();
      a2=s2->pos().Perp();
      return a1>a2;
  }
}


bool
sortTrackletsClass::operator() (PndTpcRiemannTrack* t1, PndTpcRiemannTrack* t2){
  double a1;
  double a12;
  double a2;
  double a22;
  TVector3 d1;
  TVector3 d2;
  switch (sorting){
    case -1: //no sorting
      return false;

    // if clusters are NOT sorted by R, sort tracklets by R
    case 0:
    case 1:
    case 2:
    case 4:
      a1=t1->getFirstHit()->cluster()->pos().Perp();
      a12=t1->getLastHit()->cluster()->pos().Perp();
      if (a12<a1) a1=a12;

      a2=t2->getFirstHit()->cluster()->pos().Perp();
      a22=t2->getLastHit()->cluster()->pos().Perp();
      if (a22<a2) a2=a22;

      return a1<a2;

    // if clusters are sorted by R, sort tracklets by Z
    case 3:
    default:
      a1=t1->getFirstHit()->cluster()->pos().Z();
      a12=t1->getLastHit()->cluster()->pos().Z();
      if (a12<a1) a1=a12;

      a2=t2->getFirstHit()->cluster()->pos().Z();
      a22=t2->getLastHit()->cluster()->pos().Z();
      if (a22<a2) a2=a22;

      return a1<a2;
  }
}



