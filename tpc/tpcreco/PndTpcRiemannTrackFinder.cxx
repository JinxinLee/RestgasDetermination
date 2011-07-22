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
#include "PndTpcRiemannTTCorrelator.h"

#include "TMath.h"

using namespace std;

//#define DEBUGHT
//#define DEBUGTT
#define UPTOHIT

// Class Member definitions -----------
PndTpcRiemannTrackFinder::PndTpcRiemannTrackFinder()
  : _minHitsForFit(5), _sortingMode(true),
    _sorting(3), _interactionZ(0.), _MaxNumHitsForPR(2147483646), _TTproxcut(500), _proxcut(5),
    fRiemannScale(24.6), _maxR(0), _minHits(100),
    _initTrks(false), _skipAndDelete(false), _initDip(0), _initCurv(0), _skipCrossingAreas(false)
{   
}

PndTpcRiemannTrackFinder::PndTpcRiemannTrackFinder(double scale)
  : _minHitsForFit(5), _sortingMode(true),
    _sorting(3), _interactionZ(0.), _MaxNumHitsForPR(2147483646), _TTproxcut(500), _proxcut(5),
    fRiemannScale(scale), _maxR(0), _minHits(100),
    _initTrks(false), _skipAndDelete(false), _initDip(0), _initCurv(0), _skipCrossingAreas(false)
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
  #ifdef DEBUGHT
    std::cout<<"PndTpcRiemannTrackFinder::buildTracks"<<std::endl;
  #endif

  unsigned int nFinishedTrks(0);
  unsigned int ncl(cll.size());
  if(ncl<3) return 0;

  sortClusters(cll);
  const double phi0(cll[0]->pos().Phi());

  int ncor = _correlators.size();

  #ifdef UPTOHIT
    if(_MaxNumHitsForPR<ncl) ncl=_MaxNumHitsForPR;
  #endif

  for(unsigned int icl=0;icl<ncl;++icl){ // loop over hits
    #ifdef DEBUGHT
        if(icl%1000==0 && icl > 1){
          unsigned int clInTrks(0);
          for (unsigned int itrklt=0; itrklt<candlist.size(); ++itrklt){
            clInTrks += candlist[itrklt]->getNumHits();
          }
          cout << "At cluster " << icl << endl;
          cout << "Tracklets: "<< candlist.size() << endl;
          cout << "Finished Tracklets: "<< nFinishedTrks << endl;
          cout << "Mean number of hits/track: "<< (double)clInTrks/(double)candlist.size() << endl;
        }
    #endif


    PndTpcRiemannHit* rhit=new PndTpcRiemannHit(cll[icl],fRiemannScale);
    unsigned int matchTrks(0);
    unsigned int maxlevel=0; // index of track with highest number of applicable correlators
    bool foundAtAll=false; 
    for(int i=0;i<ncor;++i) _bestMatchQuality[i] = 99999.;// reset 

    for(unsigned int itrk=0;itrk<candlist.size();++itrk){ // loop over tracks
      PndTpcRiemannTrack* trk=candlist[itrk];
      if (trk->isFinished()) continue;

      // check if track can be deleted
      double Perp(rhit->cluster()->pos().Perp());
      if(_skipAndDelete) {
        bool deleet(true), finished(false);

        if (trk->getNumHits() > _minHits){ // do not delete
          deleet=false;
        }

        if (_sorting==3){
          Perp += 3*_proxcut;
          if (trk->getFirstHit()->cluster()->pos().Perp() > Perp &&
              trk->getLastHit()->cluster()->pos().Perp() > Perp)
            finished = true;
        }
        else if (_sorting==5){
          double Phi(rhit->cluster()->pos().Phi());
          double dPhi = 2*_proxcut/Perp; // approx for small angles
          if (Phi - trk->getFirstHit()->cluster()->pos().Phi() > dPhi &&
              Phi - trk->getLastHit()->cluster()->pos().Phi() > dPhi)
            finished = true;
        }
        else if (_sorting==-5){
          double Phi(rhit->cluster()->pos().Phi());
          double dPhi = -2*_proxcut/Perp; // approx for small angles
          if (Phi - trk->getFirstHit()->cluster()->pos().Phi() < dPhi &&
              Phi - trk->getLastHit()->cluster()->pos().Phi() < dPhi)
            finished = true;
        }
        else if (_sorting==2){
          double Z(rhit->cluster()->pos().Z());
          double dZ = 3*_proxcut;
          if (trk->getFirstHit()->cluster()->pos().Z() - Z > dZ &&
              trk->getLastHit()->cluster()->pos().Z() - Z > dZ)
            finished = true;
        }


        if(finished && deleet){
          trk->deleteHits();
          delete trk;
          candlist.erase(candlist.begin()+itrk);
          --itrk;
          continue;
        }

        if(finished) {
          trk->setFinished();
          ++nFinishedTrks;
          continue;
        }
      }
      // check if track is finished
      else if (_sorting==3){
        Perp += 4*_proxcut;
        if (trk->getFirstHit()->cluster()->pos().Perp() > Perp &&
            trk->getLastHit()->cluster()->pos().Perp() > Perp){
          trk->setFinished();
          ++nFinishedTrks;
          continue;
        }
      }



      // WE STEP THROUGH THE INDIVIDUAL CORRELATORS
      // IF A TRACK SURVIVES EACH CORRELATOR
      // THE HIT IS ASSIGNED TO THE BEST (smallest!) MATCH 
      
      bool trksurvive = false;
      std::vector<double> matchQualities(ncor, 99999.); // for saving the match qualities for each correlator
      int level = 0; // number of survived correlators
      #ifdef DEBUGHT
        if(icl==_MaxNumHitsForPR-1) std::cout<<"Testing hit "<<icl<<" with track "<<itrk<< "; trk quality: " << trk->quality() << std::endl;
      #endif

      for(int icor=0;icor<ncor;++icor){ // loop through correlators
        // CORRELATE HIT WITH TRACK
        double matchQuality = 99999;
        bool survive=false;
        bool applicable=_correlators[icor]->corr(trk,rhit,survive,matchQuality);
        #ifdef DEBUGHT
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
        if(level==ncor-1 && !trk->isInitialized()) ++matchTrks; // number matching fitted tracks that survived all corrs
        
        if(level>maxlevel) maxlevel=level;
        for(unsigned int i=0; i<=level; ++i){
          if(matchQualities[i]<_bestMatchQuality[i]){
            _bestMatchQuality[i]=matchQualities[i];
            _bestMatchIndex[i]=itrk;
          }
        }
      }

      #ifdef DEBUGHT
      if(icl==_MaxNumHitsForPR-1 && trksurvive) std::cout<<" Track "<<itrk<<" survived with level "<<level<<std::endl;
        if(icl==_MaxNumHitsForPR-1) std::cout<<std::endl;
      #endif

      foundAtAll|=trksurvive; // foundAtAll will be true if at least one track survived
    } // end loop over tracks


    #ifdef DEBUGHT
      if(icl==_MaxNumHitsForPR-1){
        std::cout<<"maxlevel "<< maxlevel <<std::endl;
        std::cout<<"_bestMatchIndex[maxlevel] "<<_bestMatchIndex[maxlevel] <<std::endl;
        std::cout<<"_bestMatchQuality[maxlevel] "<<_bestMatchQuality[maxlevel] <<std::endl;
      }
    #endif


    if(!foundAtAll){ // new track if no track survived
      if(_skipAndDelete) {
        double R(rhit->cluster()->pos().Perp());
        if (_sorting==3 && R<_maxR){
          #ifdef DEBUGHT
            if(icl==_MaxNumHitsForPR-1) std::cout<<"-> hit perp < " << _maxR << ", skipping hit "<<icl<<std::endl;
          #endif
          continue;
        }
        else if (_sorting==5 && (rhit->cluster()->pos().Phi()-phi0) * R > 1.){ // todo: hardcoded 2
          #ifdef DEBUGHT
            if(icl==_MaxNumHitsForPR-1) std::cout<<"-> hit (phi-phi0)*R > 1" << ", skipping hit "<<icl<<std::endl;
          #endif
          continue;
        }
        else if (_sorting==-5 && (rhit->cluster()->pos().Phi()-phi0) * R < -1.){ // todo: hardcoded 2
          #ifdef DEBUGHT
            if(icl==_MaxNumHitsForPR-1) std::cout<<"-> hit (phi-phi0)*R < -1" << ", skipping hit "<<icl<<std::endl;
          #endif
          continue;
        }
      }

      PndTpcRiemannTrack* trk=new PndTpcRiemannTrack(fRiemannScale);
      trk->setSort(_sortingMode);
      candlist.push_back(trk);
      //std::cout<<"Creating new track"<<std::endl;
      trk->addHit(rhit);
      #ifdef DEBUGHT
        if(icl==_MaxNumHitsForPR-1) std::cout<<"-> creating new track Nr "<<candlist.size()-1<<std::endl;
      #endif

      if(_initTrks) {
        if (_sorting==3) trk->initTargetTrack(_initDip, _initCurv);
        if (_sorting==5 || _sorting==-5) trk->initCircle(0);
      }

    }
    else {
      // if more than one track matches, hit lies in crossing section -> skip
      if(_skipCrossingAreas && matchTrks>1) {
        #ifdef DEBUGHT
          if(icl==_MaxNumHitsForPR-1){
           std::cout<<" "<<matchTrks<<" tracks match -> hit lies in crossing area -> skip hit"<<std::endl;
          }
        #endif        
        resetFlags();
        delete rhit;
        cll.erase(cll.begin() + icl);
        --icl;
        --ncl;
        continue;
      }
      // add hit to best match
      // use the bestMatch from deepest level
      PndTpcRiemannTrack* theTrk=candlist[_bestMatchIndex[maxlevel]];
      #ifdef DEBUGHT
        if(icl==_MaxNumHitsForPR-1) std::cout<<"-> adding hit to track"<<_bestMatchIndex[maxlevel]<<std::endl;
      #endif
      theTrk->addHit(rhit);
      if(theTrk->getNumHits()>=_minHitsForFit){
        theTrk->fitAndSort();
        #ifdef DEBUGHT
          if(icl==_MaxNumHitsForPR-1){
           std::cout<<" track parameters: _c="<<theTrk->c()<<"  R="<<theTrk->r()<<"  dip °="<<(theTrk->dip())*180/TMath::Pi()<<std::endl;
           std::cout<<" center: "; theTrk->center().Print();
          }
        #endif
      }
    }
    resetFlags();
  } // end loop over hits
  

  #ifdef DEBUGHT
    std::cout<<candlist.size()<<" Riemann Tracks found."<<std::endl;
  #endif

 return candlist.size();
}

void
PndTpcRiemannTrackFinder::mergeTracks(std::vector<PndTpcRiemannTrack*>& candlist){
  #ifdef DEBUGTT
    std::cout<<"PndTpcRiemannTrackFinder::mergeTracks"<<std::endl;
  #endif

  unsigned int ntr=candlist.size();
  if (ntr<2) return; // need at least 2 trackcands to merge

  // sort tracklets, but use different sorting than for clusters!
  sortTracklets(candlist);

  double z1max, z2min, zTemp;

  for(unsigned int itrk1=0; itrk1<ntr-1; ++itrk1){ // loop over tracks
    if(candlist[itrk1]==NULL)continue;
    PndTpcRiemannTrack* trk1=candlist[itrk1];

    // find max z of trk1
    if(_sorting==3){
      z1max = trk1->getFirstHit()->cluster()->pos().Z();
      zTemp = trk1->getLastHit()->cluster()->pos().Z();
      if (zTemp>z1max) z1max=zTemp;
    }

    for(unsigned int itrk2=itrk1+1; itrk2<ntr; ++itrk2){ // loop over the other tracks to be tested
      if(candlist[itrk2]==NULL)continue;

      #ifdef DEBUGTT
        std::cout<<"Testing track "<<itrk1<<" with track "<<itrk2<<std::endl;
      #endif

      PndTpcRiemannTrack* trk2=candlist[itrk2];

      // find min z of trk2
      if(_sorting==3){
        z2min = trk2->getFirstHit()->cluster()->pos().Z();
        zTemp = trk2->getLastHit()->cluster()->pos().Z();
        if (zTemp<z2min) z2min=zTemp;
        // tracklets are sorted by z (from small to big), if the smallest z of the trk2 is bigger than the maximum z of trk1, skip all other tracks
        if (z2min > (z1max + _TTproxcut + 0.1) ) {
          #ifdef DEBUGTT
            std::cout<<" (z2min > (z1max + _TTproxcut + 0.1) ), skipping rest of trk2 tracklets (" << ntr-itrk2 << ")" <<std::endl;
          #endif
          break; // continue with next trk1
        }
      }

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

        #ifdef DEBUGTT
          if(!applicable){std::cout<<"  correlator "<<icor<<" NOT applicable"<<std::endl;}
          else{std::cout<<"  correlator "<<icor<<"  IS applicable; survived "<<survive<<" with MatchQuality "<<matchQuality<<std::endl;}
        #endif

        if(!applicable) {
          survive = false;
          break;
        }
        if(!survive) break;
      } // end loop through correlators
      if (!survive) continue; // test next trk2

      // merge tracks if survived
      #ifdef DEBUGTT
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
        if(mergedTrack->getNumHits()>=_minHitsForFit) mergedTrack->fitAndSort();

        // delete old trackcands and store new trackcand
        delete candlist[itrk1];
        trk1=mergedTrack;
        candlist[itrk1]=mergedTrack;
      }
      else { // we can just add the hits from trk2 to trk1 and the sorting is done internally
        for(unsigned int i=0; i<nhits2; ++i)
          trk1->addHit(trk2->getHit(i));

        // refit if we have enough hits
        if(trk1->getNumHits()>=_minHitsForFit) trk1->fitAndSort();
      }

      // update max z of trk1
      if(_sorting==3){
        z1max = trk1->getFirstHit()->cluster()->pos().Z();
        zTemp = trk1->getLastHit()->cluster()->pos().Z();
        if (zTemp>z1max) z1max=zTemp;
      }

      // delete trk2
      delete candlist[itrk2];
      candlist[itrk2]=NULL;

    } // end loop over the other tracks to be tested
  } // end loop over tracks

  // clean up candlist
  for(int i=0; i<candlist.size(); ++i){
    if (candlist[i]==NULL) {
      candlist.erase(candlist.begin()+i);
      --i; // go one step back because "erase" shifts back the rest
    }
  }

  #ifdef DEBUGTT
    std::cout<<candlist.size()<<" Merged Riemann Tracks: "<<std::endl;
  #endif
}


void
PndTpcRiemannTrackFinder::cleanTracks(std::vector<PndTpcRiemannTrack*>& candlist,
                                      double szcut, double planecut){

  std::cout<<"WARNING: PndTpcRiemannTrackFinder::cleanTracks - no functionality!"<<std::endl;
  /*PndTpcRiemannHit* hit;

  for(unsigned int i=0; i<candlist.size(); ++i){ // loop over trackcands
    if(!candlist[i]->isFitted() || !candlist[i]->isFitted()) continue; // skip track

    for(unsigned int j=0; j<candlist[i]->getNumHits(); ++j){ // loop over hits
      hit = candlist[i]->getHit(j);
      if (TMath::Abs(candlist[i]->dist(hit)) > planecut || TMath::Abs(candlist[i]->szDist(hit)) > szcut){
        candlist[i]->removeHit(j);
        candlist[i]->refit();
        --j;
      }
      if(!candlist[i]->isFitted() || !candlist[i]->isFitted()) break; // skip track
    } // end loop over hits

  } // end loop over trackcands*/

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
    case 5:
      a1=s1->pos().Phi();
      a2=s2->pos().Phi();
      return a1<a2;
      break;
    case -5:
      a1=s1->pos().Phi();
      a2=s2->pos().Phi();
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



