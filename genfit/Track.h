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

#ifndef FITTER_TRACK_H 
#define FITTER_TRACK_H

#include "AbsTrackRep.h"
#include "AbsRecoHit.h"

#include "TClonesArray.h"
#include "TObjArray.h"

#include "TrackCand.h"

class TVirtualGeoTrack;


class Track : public TObject {   
private:
  //std::vector<AbsTrackRep*> trackReps;
  TObjArray* trackReps; //->
  std::vector<AbsRecoHit*>      hits;//!
  
  
  TrackCand _cand; // list of hits
  //std::vector<int> detID;
  //std::vector<int> hitIndex;
  
  static const int defNumTrackReps = 10; //!
  unsigned int _cardinal_rep; // THE selected rep, default=0;

  unsigned int _nextHitToFit;
  
public:
  Track(); 
  Track(const Track&); 
  Track(AbsTrackRep*); 
  
  virtual ~Track();

  // -----------------------
  // Accessors
  // -----------------------

  //inline std::vector<int> getDetIDs() const {
  //  return detID;
  //}
  //inline std::vector<int> getHitIndices() const {
  //  return hitIndex;
  //}

  void reset();  // deletes the RecoHits!

  std::vector<AbsRecoHit*> getHits() {return hits;}

  const TrackCand& getCand() const {return _cand;}

  AbsRecoHit* getHit(int id) const {
    return hits.at(id);
  }

  unsigned int getNumHits() const {
    return hits.size();
  }

  void mergeHits(Track* trk); // will merge the hits in trk
                              // ownership of hits changes!
                              // trk will be empty afterwards!

  void releaseHits(){hits.clear();} // gives up ownership over hits;
                      // hits will be cleared;

  unsigned int getNextHitToFit() const {return _nextHitToFit;}
  void setNextHitToFit(unsigned int i) {_nextHitToFit=i;}

  
  AbsTrackRep* getTrackRep(int id) const {
    return reinterpret_cast<AbsTrackRep*>(trackReps->At(id));
  }

  unsigned int getNumReps() const {
    return trackReps->GetEntriesFast();
  }

  AbsTrackRep* getCardinalRep() const {return ((AbsTrackRep*)trackReps->At(_cardinal_rep));}
  
  TVector3 getMom() const {return getCardinalRep()->getMom();}
  TVector3 getPos() const {return getCardinalRep()->getPos();}
  double getChiSqu() const {return getCardinalRep()->getChiSqu();}
  double getCharge() const {return getCardinalRep()->getCharge();}

  //double refinedChi2(); not implemented yet

  void fillGeoTrack(TVirtualGeoTrack* tr) const {fillGeoTrack(tr,_cardinal_rep);} 
  void fillGeoTrack(TVirtualGeoTrack* tr,unsigned int repid) const;
  
  // ---------------------
  // Modifiers
  // ---------------------

  inline void addHit(AbsRecoHit* theHit) {  // deprecated!!!
    hits.push_back(theHit);
  }
  
  void addHit(AbsRecoHit* theHit, 
	      unsigned int detId,
	      unsigned int hitId){
    hits.push_back(theHit);
    _cand.addHit(detId,hitId);
  }

  //inline void addHitIndices(int __detID, int __index) {
  //  detID.push_back(__detID);
  //  hitIndex.push_back(__index);
  //}
  
  void addHitVector(std::vector<AbsRecoHit*> __vec) {
    hits = __vec;
  }

  void addTrackRep(AbsTrackRep* theTrackRep) {
    if(trackReps==NULL)trackReps=new TObjArray(defNumTrackReps);
    trackReps->Add(theTrackRep);
  }
    
  void setCandidate(const TrackCand& cand, bool reset=false);
  
  void setCardinalRep(unsigned int r){if(r<trackReps->GetEntriesFast())_cardinal_rep=r;}
  

  void getResiduals(unsigned int detId, // which detector?
		    unsigned int dim,   // which projection?
		    unsigned int rep,   // which trackrep ?
		    std::vector<double>& result);
		    

  void gotoPoint(const TVector3& pos); // extrapolate track to a point
  void stepalong(double h); // extrapolate track by s cm (using cardinal rep)
  
public:
  ClassDef(Track,1)
};




#endif 
