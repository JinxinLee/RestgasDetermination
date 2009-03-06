/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

#include "Track.h"
#include "TVirtualGeoTrack.h"
#include "VirtSpacePointRecoHit.h"
#include <assert.h>
#include <iostream>

Track::Track(AbsTrackRep* defaultRep) 
  : _cardinal_rep(0), _nextHitToFit(0)
{
  trackReps = new TObjArray(defNumTrackReps);
  trackReps->Add(defaultRep);
}

Track::Track() 
  : trackReps(0), _cardinal_rep(0), _nextHitToFit(0)
{
  //trackReps = new TObjArray(defNumTrackReps);
}

Track::~Track() {
  if(trackReps!=NULL){
    for(int i=0;i<getNumReps();i++) {
      delete trackReps->At(i);
    }
  }
  for(unsigned int i=0;i<hits.size();i++) {
    delete hits[i];
  }
}

Track::Track(const Track& _tr) {
  _cand=_tr._cand;
  _cardinal_rep=_tr._cardinal_rep;
  _nextHitToFit=_tr._nextHitToFit;
  for(int i=0;i<_tr.getNumHits();i++) {
    hits.push_back((_tr.getHit(i))->clone());
  }
  trackReps = new TObjArray(defNumTrackReps);
  for(int i=0; i<_tr.getNumReps();i++) {
    trackReps->Add( (_tr.getTrackRep(i))->clone() );
  }
}


void
Track::reset(){
  if(trackReps!=NULL){
    for(int i=0;i<getNumReps();i++) {
      delete trackReps->At(i);
    }
  }
  for(unsigned int i=0;i<hits.size();i++) {
    delete hits[i];
  }
  hits.clear();
}

void
Track::mergeHits(Track* trk){
  unsigned int nhits=trk->getNumHits();
  for(unsigned int i=0;i<nhits;++i){
    unsigned int detId;
    unsigned int hitId;
    trk->getCand().getHit(i,detId,hitId);
    AbsRecoHit* hit=trk->getHit(i);
    addHit(hit,detId,hitId);
  }
  trk->hits.clear();
}


void 
Track::setCandidate(const TrackCand& cand, bool reset)
{
_cand=cand;
// reset fits
 if(reset) {
   for(int i=0;i<getNumReps();i++) {
     ((AbsTrackRep*)trackReps->At(i))->reset();
   }
 }
}

void 
Track::fillGeoTrack(TVirtualGeoTrack* geotrk,unsigned int repid) const
{
  AbsTrackRep* rep=getTrackRep(repid);
  unsigned int n=_cand.getNHits();
  rep->getState().Print();
  rep->getStartState().Print();
  for(unsigned int i=0; i<n; ++i){// loop over hits
    DetPlane pl=hits[i]->getDetPlane(rep);
    TVector3 pos=rep->getPos(pl);
    std::cout<<pos.X()<<","<<pos.Y()<<","<<pos.Z()<<std::endl;
    geotrk->AddPoint(pos.X(),pos.Y(),pos.Z(),0);
  }// end loop over hits
}


void 
Track::getResiduals(unsigned int detId, // which detector?
		    unsigned int dim,   // which projection?
		    unsigned int repid,   // which trackrep ?
		    std::vector<double>& result)
{
  unsigned int nhits=getNumHits();
  if(repid>=getNumReps())return;
  AbsTrackRep* rep=getTrackRep(repid);//->clone();
  assert(rep->getState()==getTrackRep(repid)->getState());
  for(unsigned int ih=0; ih<nhits; ++ih){// loop over hits
    unsigned int anid;
    unsigned int dummy;
    _cand.getHit(ih,anid,dummy); // check if this is a hit we want to look at
    if(anid==detId){
      AbsRecoHit* hit=getHit(ih);
      // extrapolate trackrep there
      int repDim=rep->getDim();
      TMatrixT<double> state(repDim,1);
      DetPlane pl=hit->getDetPlane(rep);
      
      rep->extrapolate(pl,state);
      //rep->setState(state);
      //rep->setReferencePlane(pl);
      double res=hit->residualVector(rep,state)[dim][0];

      //std::cout<<res<<std::endl;

      result.push_back(res);
    } 
  }
}


void
Track::gotoPoint(const TVector3& pos)
{
  // create virtual space point hit
  VirtSpacePointRecoHit virtHit(pos);
  // for each trackrep get detplane for that hit
  for(unsigned int i=0;i<getNumReps();++i){
    AbsTrackRep* rep=getTrackRep(i);
    DetPlane pl=virtHit.getDetPlane(rep);
    rep->extrapolate(pl);
  }
}

void
Track::stepalong(double h){
  getCardinalRep()->stepalong(h);
  TVector3 pos=getCardinalRep()->getPos();
  // take the other reps with you!
  if(getNumReps()>1)gotoPoint(pos);
}



ClassImp(Track)



