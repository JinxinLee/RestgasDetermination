//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcConfTrackFinder
//      see PndTpcConfTrackFinder.hh for details
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
#include "PndTpcConfTrackFinder.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <algorithm>
#include <list>
#include <cmath>

using std::cout;
using std::endl;

// Collaborating Class Headers --------
#include "TClonesArray.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "GFKalman.h"
#include "PndTpcConfMapRecoHit.h"
#include "PndTpcConfMapFit.h"
#include "PndTpcZSFit.h"
#include "PndTpcClusterRadius.h"
#include "MatrixChi2.h"
#include "DebugLogger.h"

#include "TH2D.h"

// Class Member definitions -----------
PndTpcConfTrackFinder::PndTpcConfTrackFinder()
  : _conffitresults(NULL), _conffitcov(NULL), _maxHitsInTrack(100000)
{}

PndTpcConfTrackFinder::~PndTpcConfTrackFinder()
{}


void 
PndTpcConfTrackFinder::configure(double xcut, double ycut, double zcut,
			      double chi2cut,
			      unsigned int minHitsForFit,
			      bool domerge,
			      double radiusMergeCut,  // fractional cut
			      double dipMergeCut,     // fractional cut
			      double distMergeCut)
{
  _xcut=xcut;_ycut=ycut;_zcut=zcut;
  _minHitsForFit=minHitsForFit;
  _chi2cut=chi2cut;
  _radiusMergeCut=radiusMergeCut;  // fractional cut
  _dipMergeCut=dipMergeCut;
  _distMergeCut=distMergeCut;
  _domerge=domerge;
}

void
PndTpcConfTrackFinder::buildTracks(TClonesArray* clusterarray,
				std::vector<GFTrackCand*>& candlist)
{
  throw "PndTpcConfTrackFinder::not implemented";
}

void
PndTpcConfTrackFinder::buildTracks(std::vector<PndTpcCluster*>& cll,
				std::vector<GFTrackCand*>& candlist)
{
  std::cout<<"PndTpcConfTrackFinder::buildTracks:: starting."<<std::endl;

  // clean up from last event
  unsigned int ntrkold=_trks.size();
  for(unsigned int i=0;i<ntrkold;++i){
    if(_trks[i]!=NULL){
      delete _trks[i];
      _trks[i]=NULL;
    }
  }
  _trks.clear();

  std::sort(cll.begin(),cll.end(),PndTpcClusterRadius());
  unsigned int ncl=cll.size();
  std::list<PndTpcConfMapRecoHit*> hits;
  //std::vector<GFTrack*> trks; // all tracks

  for(unsigned int icl=0;icl<ncl;++icl){//loop over clusters from large radius inwards
    
    hits.push_back(new PndTpcConfMapRecoHit(cll[icl]));
  }
  
  // two passes:
  for(unsigned int pass=0;pass<2;++pass){
    std::list<PndTpcConfMapRecoHit*>::iterator hitIt=hits.begin();
    while(hitIt!=hits.end()){
    
    PndTpcConfMapRecoHit* hit=*hitIt;
    ++hitIt;
   

    bool foundProx=false;
    bool foundConf=false;

    unsigned int bestHitMatchIndex=0;
    double bestHitMatchQuality=999;
    unsigned int bestConfMatchIndex=0;
    double bestConfMatchQuality=999;
    
    unsigned int ntrks=_trks.size();
    for(unsigned int itrk=0;itrk<ntrks;++itrk){//loop over track candidates
      GFTrack* trk=_trks[itrk];
      double hitMatchQuality;
      double confMatchQuality;
      if(trk==NULL)continue;
      if(trk->getNumHits()>_maxHitsInTrack)continue;
      if(trk->getNumHits()<_minHitsForFit){
	foundProx|=matchHitTrack(hit,trk,hitMatchQuality);
	if(hitMatchQuality<bestHitMatchQuality){
	  bestHitMatchQuality=hitMatchQuality;
	  bestHitMatchIndex=itrk;
	}
      }
      else{
	foundConf|=matchConfTrack(hit,trk,confMatchQuality);
	if(confMatchQuality<bestConfMatchQuality){
	  bestConfMatchQuality=confMatchQuality;
	  bestConfMatchIndex=itrk;
	}
      }
     
      } // end loop over track candidates
    

    if(!foundProx && !foundConf){//create new track candidate
      //std::cout<<"create new track"<<std::endl;
      //std::cout<<"hitMatchQuality="<<bestHitMatchQuality<<std::endl;
      //std::cout<<"confMatchQuality="<<bestConfMatchQuality<<std::endl;
      GFTrack* newtrk=new GFTrack(new PndTpcConfMapFit());
      GFAbsTrackRep* zsfit=new PndTpcZSFit();
      newtrk->addTrackRep(zsfit);
      _trks.push_back(newtrk);
      addHit2Track(hit,newtrk);
    }
    else {// add hit to bestMatch
      if(foundConf)addHit2Track(hit,_trks[bestConfMatchIndex]);
      else addHit2Track(hit,_trks[bestHitMatchIndex]);
    }
  }// end loop over clusters
  
    //----------------------------------
    // put hits from small tracks back into list for next pass;
    hits.clear();
    unsigned int ntrks=_trks.size();
    for(unsigned int itrk=0;itrk<ntrks;++itrk){// loop over tracks
      GFTrack* trk=_trks[itrk];
      
      if(trk==NULL)continue;
      unsigned int nhits_intrk=trk->getNumHits();
      if(nhits_intrk>=_minHitsForFit)continue;
      for(unsigned int ih=0;ih<nhits_intrk;++ih){
	GFAbsRecoHit* abshit=trk->getHit(ih);
	PndTpcConfMapRecoHit* hitpointer=dynamic_cast<PndTpcConfMapRecoHit*>(abshit);
	hits.push_back(hitpointer);
      }// end loop over hits in track
      trk->releaseHits();
      delete trk;
      _trks[itrk]=NULL;
    }// end loop over tracks
    std::cout<<hits.size()<<" hits left after pass "<<pass<<std::endl;
  }
  // ---------------------------------------
  // merge track pieces
  if(_domerge)merge(_trks); // be carefull this might delete tracks!!!

  // ----------------------------------------
  // output of GFTrackCandidates
  unsigned int nfound=0;
  
  unsigned int ntrks=_trks.size();
  for(unsigned int itrk=0;itrk<ntrks;++itrk){
    GFTrack* trk=_trks[itrk];
    if(trk==NULL)continue;
    ++nfound;
    if(trk->getNumHits()>=_minHitsForFit){
      double r=((PndTpcConfMapFit*)trk->getTrackRep(0))->getR();
      double d=((PndTpcZSFit*)trk->getTrackRep(1))->getDip();
      if(_conffitresults!=NULL){
	_conffitresults->push_back(((PndTpcConfMapFit*)trk->getTrackRep(0))->getState());
      }
      if(_conffitcov!=NULL){
	_conffitcov->push_back(((PndTpcConfMapFit*)trk->getTrackRep(0))->getCov());
      }
      //std::cout<<"Track with radius r="<<r<<std::endl;
      //std::cout<<"Hits in cand="<<trk->getCand().getNHits()<<std::endl;
      GFTrackCand* cand=new GFTrackCand(trk->getCand());
      if(r==0)r=1E-12;
      cand->setCurv(1./r);
      cand->setDip(d);
      candlist.push_back(cand);
    }
  } // end output loop over tracks
 
  std::cout<<nfound<<" tracks pieces found"<<std::endl;
  std::cout<<candlist.size()<<" candidates of appropriate length"<<std::endl;
  std::cout<<"PndTpcConfTrackFinder::buildTracks:: finished."<<std::endl;
} // end method build tracks


bool
PndTpcConfTrackFinder::matchHitTrack(PndTpcConfMapRecoHit* hit,
				  GFTrack* trk,
				  double& matchQuality)
{
  if(trk==NULL)return false;
  unsigned int nhitsInTrk=trk->getNumHits();
  bool alive=true;
  PndTpcConfMapRecoHit* hitontrk=dynamic_cast<PndTpcConfMapRecoHit*>(trk->getHit(nhitsInTrk-1)); // last hit
  alive=applyProximityCuts(hit,hitontrk);
  matchQuality=hitDist(hit,hitontrk);
  if(!alive)DebugLogger::Instance()->Histo("ConfTrackCutCode",4,0,20,20);
  return alive;
}


bool
PndTpcConfTrackFinder::matchConfTrack(PndTpcConfMapRecoHit* hit,
				  GFTrack* trk,
				  double& matchQuality)
{
  if(trk==NULL)return false;
  PndTpcConfMapFit* fit=dynamic_cast<PndTpcConfMapFit*>(trk->getTrackRep(0));
  hit->setRotated(fit->isRotated());
  hit->setReferencePoint(fit->getOrigin().X(),
			 fit->getOrigin().Y());
  GFKalman myfitter;
  double ConfMatchQuality=myfitter.getChi2Hit(hit,trk->getTrackRep(0));
  DebugLogger::Instance()->Histo("ConfMatchQuality",ConfMatchQuality,0,200,100);
  double ZSmatchQuality=999.;
  if(ConfMatchQuality>_chi2cut){
    //std::cout<<"bad match: "<<matchQuality<<std::endl;
    matchQuality=ConfMatchQuality;
    DebugLogger::Instance()->Histo("ConfTrackCutCode",5,0,20,20);
    return false;
  }
  else {
    // do zs-check
    // set s for this candidate
    hit->calc_s(dynamic_cast<PndTpcConfMapFit*>(trk->getTrackRep(0))->getR());
    ZSmatchQuality=myfitter.getChi2Hit(hit,trk->getTrackRep(1));
    DebugLogger::Instance()->Histo("ZSmatchQuality",ZSmatchQuality,0,200,100);
    if(ZSmatchQuality>_chi2cut){
      matchQuality=ZSmatchQuality;
      DebugLogger::Instance()->Histo("ConfTrackCutCode",6,0,20,20);
      return false;
    }
  }
  //std::cout<<"ZSmatchQuality="<<ZSmatchQuality<<std::endl;
  //std::cout<<"ConfMatchQuality="<<ConfMatchQuality<<std::endl;
  matchQuality=TMath::Max(ConfMatchQuality,ZSmatchQuality);
  DebugLogger::Instance()->Histo("matchQuality",matchQuality,0,20,100);
  //std::cout<<"matchConfTrack::chi2="<<matchQuality<<std::endl;
  return true;
}

void
PndTpcConfTrackFinder::merge(std::vector<GFTrack*>& trks)
  // careful: will delete tracks!!!
{
  //std::cout<<"Beginning track merge"<<std::endl;
  // for the moment do the combinatorics brute force
  unsigned int ntrks=trks.size();
  if(ntrks<2)return;
  for(unsigned int itrk=0;itrk<ntrks;++itrk){
    GFTrack* thetrk=trks[itrk];
    if(thetrk==NULL) continue;
    if(thetrk->getNumHits()<_minHitsForFit)continue;
    //chi2
    TMatrixT<double> conf1=thetrk->getTrackRep(0)->getState();
    TMatrixT<double> cov1=thetrk->getTrackRep(0)->getCov();

    double r=dynamic_cast<PndTpcConfMapFit*>(thetrk->getTrackRep(0))->getR();
    double dip=dynamic_cast<PndTpcZSFit*>(thetrk->getTrackRep(1))->getDip();
    PndTpcConfMapRecoHit* hit1a=dynamic_cast<PndTpcConfMapRecoHit*>(thetrk->getHit(0));
    PndTpcConfMapRecoHit* hit1b=dynamic_cast<PndTpcConfMapRecoHit*>(thetrk->getHit(thetrk->getNumHits()-1));
    for(unsigned int j=itrk+1;j<ntrks;++j){
      GFTrack* atrk=trks[j];
      if(atrk==NULL)continue;
      if(atrk->getNumHits()<_minHitsForFit)continue;
      // chi2
      TMatrixT<double> conf2=atrk->getTrackRep(0)->getState();
      TMatrixT<double> cov2=atrk->getTrackRep(0)->getCov();
      MatrixChi2 chif;
      double chi2=chif(conf1,cov1,conf2,cov2);
      std::cout<<"merge chi2="<<chi2<<std::endl;
      DebugLogger::Instance()->Histo("mergechi2",chi2,0,10000,100);
      

      // check radius
      double ar=dynamic_cast<PndTpcConfMapFit*>(atrk->getTrackRep(0))->getR();
      if(fabs(ar-r)>_radiusMergeCut*r){continue;}
      // check dip
      double adip=dynamic_cast<PndTpcZSFit*>(atrk->getTrackRep(1))->getDip();
      if(fabs(adip-dip)>_dipMergeCut*dip){continue;}
      // check proximity
      PndTpcConfMapRecoHit* hit2a=dynamic_cast<PndTpcConfMapRecoHit*>(atrk->getHit(0));
      PndTpcConfMapRecoHit* hit2b=dynamic_cast<PndTpcConfMapRecoHit*>(atrk->getHit(atrk->getNumHits()-1));
      // check beginning - end
      // check end - beginning
      double dist1=hitDist(hit1a,hit2b);
      double dist2=hitDist(hit1b,hit2a);
      double checkquality;
      bool check1=matchConfTrack(hit2a,thetrk,checkquality);
      bool check2=matchConfTrack(hit2b,thetrk,checkquality);
      if(!check1 && !check2){continue;}
      // if survived so far -> merge!
      if(dist1<dist2){
	mergeTracks(atrk,thetrk); // thetrk will disappear
	delete thetrk;trks[itrk]=NULL;
      }
      else {
	mergeTracks(thetrk,atrk);
	delete atrk; trks[j]=NULL;
      }
      std::cout<<"Tracks merged!"<<std::endl;
      DebugLogger::Instance()->Histo("mergechi2_merged",chi2,0,10000,100);
      if(trks[itrk]==NULL)break; // break inner look oer tracks
    } // end inner loop over trks
  }// end outer loop over trks
}

void
PndTpcConfTrackFinder::mergeTracks(GFTrack* trkA,GFTrack* trkB)
  // trkB will be deleted!
{
  GFKalman myfitter;
  // remember to remap the confhits!!!
  trkA->mergeHits(trkB); // thetrk will disappear
  
  myfitter.fittingPass(trkA,1);
}

// ---------------------------------------------------------------------
// --------- Helper Functions ------------------------------------------
// ---------------------------------------------------------------------

bool
PndTpcConfTrackFinder::applyProximityCuts(PndTpcConfMapRecoHit* hit,
				       PndTpcConfMapRecoHit* hitontrk)
{
  bool alive=true;
  if(fabs(hitontrk->z()-hit->z())>_zcut){alive=false;}
  if(alive){if(fabs(hitontrk->x()-hit->x())>_xcut){alive=false;}}
  if(alive){if(fabs(hitontrk->y()-hit->y())>_ycut){alive=false;}}
  return alive;
}

double
PndTpcConfTrackFinder::hitDist(PndTpcConfMapRecoHit* hit1,
			    PndTpcConfMapRecoHit* hit2)
{
  double dz=hit2->z()-hit1->z();
  double dx=hit2->x()-hit1->x();
  double dy=hit2->y()-hit1->y();
  double d=sqrt(dz*dz+dx*dx+dy*dy);
  return d;
}

bool
PndTpcConfTrackFinder::addHit2Track(PndTpcConfMapRecoHit* hit,
				 GFTrack* trk)
{
  hit->calc_s(dynamic_cast<PndTpcConfMapFit*>(trk->getTrackRep(0))->getR());
  // here is a problem because we cannot calculate s meaningfull 
  // if the fit has not been performed in conf-space first!
  //std::cout<<"adding hit to trk at position "<<trk->getNumHits()<<std::endl;
  PndTpcConfMapFit* fit=dynamic_cast<PndTpcConfMapFit*>(trk->getTrackRep(0));
  if(trk->getNumHits()==1){
    PndTpcConfMapRecoHit* chit0=dynamic_cast<PndTpcConfMapRecoHit*>(trk->getHit(0));
    double x0=(hit->x()+chit0->x())*0.5;
    double y0=(hit->y()+chit0->y())*0.5;
    fit->setOrigin(x0,y0);
    chit0->setReferencePoint(x0,y0);
    //trk->setNextHitToFit(1);// skip first hit from fit
  }
  hit->setReferencePoint(fit->getOrigin().X(),
			 fit->getOrigin().Y());

  trk->addHit(hit,2,hit->index());

  if(trk->getNumHits()==_minHitsForFit){ // first round of fits
    GFKalman myfitter;
    // for a first pass turn off zs-fit:
    trk->getTrackRep(1)->setStatusFlag(13);
    // set starting values
    PndTpcConfMapRecoHit* chit1=dynamic_cast<PndTpcConfMapRecoHit*>(trk->getHit(0));
    PndTpcConfMapRecoHit* chit2=dynamic_cast<PndTpcConfMapRecoHit*>(trk->getHit(1));
    double dyp=chit2->getHitCoord(GFDetPlane())[0][0]-chit1->getHitCoord(GFDetPlane())[0][0];
    double dxp=chit2->getXcf()-chit1->getXcf();
    if(dxp==0)dxp=1E-12;
    double m=dyp/dxp;
    // check wether m is not too large!
    // if so -> rotate this track!
    if(fabs(m)>10.){// rotate problem
      (dynamic_cast<PndTpcConfMapFit*>(trk->getTrackRep(0)))->setRotated();
      m=-1./m;
      // rotate all hits in trk!
      for(unsigned int i=0;i<_minHitsForFit;++i){
	PndTpcConfMapRecoHit* conhit=dynamic_cast<PndTpcConfMapRecoHit*>(trk->getHit(i));
	conhit->setRotated(true);
      }
      
    }
    
    double t=chit1->getHitCoord(GFDetPlane())[0][0]-m*chit1->getXcf();
    TMatrixT<double> state=trk->getTrackRep(0)->getState();
    GFDetPlane pl=trk->getTrackRep(0)->getReferencePlane();
    state[0][0]=m;
    state[1][0]=t;
    trk->getTrackRep(0)->setData(state,pl);
    myfitter.processTrack(trk);
    // now reset s for the first hits
    for(unsigned int i=0;i<_minHitsForFit;++i)
      {
	PndTpcConfMapRecoHit* conhit=dynamic_cast<PndTpcConfMapRecoHit*>(trk->getHit(i));
	conhit->calc_s(dynamic_cast<PndTpcConfMapFit*>(trk->getTrackRep(0))->getR());
      }
    // for second pass turn off confMapFit,turn on zs-fit
    trk->getTrackRep(0)->setStatusFlag(13);
    trk->getTrackRep(1)->setStatusFlag(0);
    myfitter.processTrack(trk);
    // turn on both fits again
    trk->getTrackRep(0)->setStatusFlag(0);
  }
  else if(trk->getNumHits()>_minHitsForFit){ // regular fits
    hit->setRotated((dynamic_cast<PndTpcConfMapFit*>(trk->getTrackRep(0))->isRotated()));
    GFKalman myfitter;
    myfitter.fittingPass(trk,1);
  }
  
  return true;
  // here some more adminstrative stuff has to go
}
