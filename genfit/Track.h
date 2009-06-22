/** @addtogroup genfit
 * @{
 */


#ifndef TRACK_H 
#define TRACK_H

#include"assert.h"
#include<map>

#include "AbsTrackRep.h"
#include "AbsRecoHit.h"

#include "TClonesArray.h"
#include "TObjArray.h"

#include "TrackCand.h"
#include "Bookkeeping.h"

class TVirtualGeoTrack;

/** @brief Track object for genfit. genfit algorithms work on these objects. 
 *
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 * Can be used as transient (only in memory) or 
 * persistent (written to ROOT-file) object.
 *
 * A Track contains a collection of RecoHits plus a collection of 
 * track representations. The TrackCand member is a helper object to store the
 * indices of the hits in the Track.
 *
 * For a Track one so called "cardinal representation" can be defined. It is 
 * that track representation that is used to access the fit results. Usually 
 * one will after the fit choose the best fitting representation to be 
 * the cardinal rep.
 */
class Track : public TObject {   
private:
  
  
  /** @brief Collection of track representations
   * 
   * this array is only to be added to in the addTrackRep method
   * because the synchronized construction of bookkeeping objects
   * and repAtHit array is ensured there. NEVER delete elements from
   * this array!
   * If this functionality will be need, it has to be done synchronized
   * with bookkeeping!!
   */
  TObjArray* trackReps; //->

  /** @brief Collection of RecoHits
   */
  std::vector<AbsRecoHit*>      hits;//!
  
  /** @brief Collection of Bookeeping objects for failed hits
   * in every trackrep
   */
  std::vector< Bookkeeping* > _bookkeeping;

  /** @brief repAtHit keeps track of at which hit index which rep
   * is currently defined, to avoid null extrapolations
   */
  std::vector<int> _repAtHit;

  /** @brief Helper to store the indices of the hits in the track. 
   * See TrackCand for details.
   */
  TrackCand _cand; // list of hits
    
  static const int defNumTrackReps = 10; //!
  unsigned int _cardinal_rep; // THE selected rep, default=0;

  unsigned int _nextHitToFit;
  
public:
  
  /** @brief Default constructor -- needed for compatibility with ROOT */
  Track(); 

  /** @brief Copy constructor */
  Track(const Track&); 

  /** @brief Initializing constructor
   *
   * A track needs at least one track representation to be useable. 
   * The given track representation has to contain starting values for the fit!
   */
  Track(AbsTrackRep*); 
  
  virtual ~Track();

  // -----------------------
  // Accessors
  // -----------------------

  /** @brief Resets the Track -- deletes RecoHits!
   */
  void reset();  // deletes the RecoHits!

  /** @brief return the number of failed Hits in track fit
   * detId == -1 will just use all detIds in the failedHits map,
   * repId == -1 will use cardinal rep
  */
  int getFailedHits(int detId=-1,int repId=-1){
	int theRep;
	if(repId==-1) theRep=_cardinal_rep;
	else theRep = repId;
	return _bookkeeping.at(theRep)->getNumFailed(detId);
  }

  /** @brief return the number of outlier Hits in track fit
   * detId == -1 will just use all detIds in the failedHits map,
   * repId == -1 will use cardinal rep
  */
  int getNumOutlierHits(int detId=-1,int repId=-1){
	int theRep;
	if(repId==-1) theRep=_cardinal_rep;
	else theRep = repId;
	return _bookkeeping.at(theRep)->getNumOutlier(detId);
  }

  std::vector<AbsRecoHit*> getHits() {return hits;}

  const TrackCand& getCand() const {return _cand;}

  AbsRecoHit* getHit(int id) const {
    return hits.at(id);
  }

  unsigned int getNumHits() const {
    return hits.size();
  }

  /** @brief Merge two Tracks. Only hits will be merged.
   *
   * All hits from trk will be merged into this Track. 
   * trk will be empty afterwards.
   *
   * Kalman::continueTrack can be used to include the newly added hits 
   * in the fit.
   *
   * Note that the new hits are inserted at the end of the present track!
   */
  void mergeHits(Track* trk); 

  /** @brief Clear hit vector. Note that hits will not be deleted!
   * 
   * Be carefull not to create memory leaks here. 
   */
  void releaseHits(){hits.clear();} 

  /** @brief Accessor for _nextHitToFit
   */
  unsigned int getNextHitToFit() const {return _nextHitToFit;}

  /** @brief Set next hit to be used in a fit
   */
  void setNextHitToFit(unsigned int i) {_nextHitToFit=i;}

  /** @brief Accessor for track representations
   */
  AbsTrackRep* getTrackRep(int id) const {
    return reinterpret_cast<AbsTrackRep*>(trackReps->At(id));
  }

  /** @brief Get number of track represenatations
   */
  unsigned int getNumReps() const {
    return trackReps->GetEntriesFast();
  }

  /** @brief Get cardinal track representation
   *
   * The user has to choose which track rep should be considered the
   * best one after the fit. Usually the track representation giving the 
   * smallest chi2 is choosen. By default the first in the list is returned.
   */
  AbsTrackRep* getCardinalRep() const {return ((AbsTrackRep*)trackReps->At(_cardinal_rep));}
  

  /** @brief Get momentum at the present position
   *
   * Cardinal representation is used.
   */
  TVector3 getMom() const {return getCardinalRep()->getMom();}

  /** @brief Get momentum at DetPlane
   *
   * The track will be extrapolated to DetPlane to get the momentum there.
   * The track will not be modified. Cardinal representation is used.
   */
  TVector3 getMom(const DetPlane& pl) const {return getCardinalRep()->getMom(pl);}

  /** @brief Get present position
   *
   * Cardinal representation is used.
   */
  TVector3 getPos() const {return getCardinalRep()->getPos();}

  /** @brief Get position at DetPlane
   *
   * The track will be extrapolated to DetPlane to get the position there.
   * The track will not be modified. Cardinal representation is used.
   */
  TVector3 getPos(const DetPlane& pl) const {return getCardinalRep()->getPos(pl);}

  /** @brief Get position, momentum, and 6x6 covariance at current position
   *
   * Cardinal representation is used.
   */
  void getPosMomCov(TVector3& pos,TVector3& mom,TMatrixT<double>& cov){
    getCardinalRep()->getPosMomCov(pos,mom,cov);
  }

  /** @brief Get position, momentum, and 6x6 covariance at DetPlane
   *
   * The track will be extrapolated to DetPlane to get everything there.
   * The track will not be modified. Cardinal representation is used.
   */
  void getPosMomCov(const DetPlane& pl,TVector3& pos,TVector3& mom,TMatrixT<double>& cov){
    getCardinalRep()->getPosMomCov(pl,pos,mom,cov);
  }

  /** @brief Get chi2
   *
   * Cardinal representation is used.
   */
  double getChiSqu() const {return getCardinalRep()->getChiSqu();}

  /** @brief Get NDF
   *
   * Cardinal representation is used.
   */
  double getNDF() const {return getCardinalRep()->getNDF();}

  /** @brief Get chi2/NDF
   *
   * Cardinal representation is used.
   */
  double getRedChiSqu() const {return getCardinalRep()->getRedChiSqu();}

  /** @brief Get charge from fit
   *
   * Cardinal representation is used.
   */
  double getCharge() const {return getCardinalRep()->getCharge();}

  /** @brief Fill TVirtualGeoTrack object Cardinal representation is used.
   */
  void fillGeoTrack(TVirtualGeoTrack* tr) const {fillGeoTrack(tr,_cardinal_rep);} 

  /** @brief Fill TVirtualGeoTrack object with data from specified track rep 
   */
  void fillGeoTrack(TVirtualGeoTrack* tr,unsigned int repid) const;
  
  // ---------------------
  // Modifiers
  // ---------------------

  void addFailedHit(unsigned int irep,unsigned int ihit){
	assert(irep<_bookkeeping.size());
	unsigned int detId,hitId;
	_cand.getHit(ihit,detId,hitId);
	_bookkeeping.at(irep)->addFailed(hitId,detId);
  }

  void addOutlier(unsigned int irep,unsigned int ihit){
	assert(irep<_bookkeeping.size());
	unsigned int detId,hitId;
	_cand.getHit(ihit,detId,hitId);
	_bookkeeping.at(irep)->addOutlier(hitId,detId);
  }

  void clearBookkeeping(){
    for(unsigned int i=0;i<_bookkeeping.size();++i){
      _bookkeeping.at(i)->clear();
    }
  }

  /** @brief deprecated!
   */
  inline void addHit(AbsRecoHit* theHit) { 
    hits.push_back(theHit);
  }
  
  /** @brief Add single hit. Updates the TrackCand
   */
  void addHit(AbsRecoHit* theHit, 
	      unsigned int detId,
	      unsigned int hitId){
    hits.push_back(theHit);
    _cand.addHit(detId,hitId);
  }

  /** @brief Add collection of hits
   *
   * This is the standard way to fill the track with hit data
   */
  void addHitVector(std::vector<AbsRecoHit*> __vec) {
    hits = __vec;
  }

  /** @brief Add track represenation
   *
   * The given track representation has to contain starting values for fit!
   */
  void addTrackRep(AbsTrackRep* theTrackRep) {
    if(trackReps==NULL)trackReps=new TObjArray(defNumTrackReps);
    trackReps->Add(theTrackRep);
    _bookkeeping.push_back( new Bookkeeping() );
    _repAtHit.push_back(-1);
  }
    
  void setCandidate(const TrackCand& cand, bool reset=false);
  
  /** @brief Choose cardinal track represenatation
   *
   * @sa getCardinalRep
   */
  void setCardinalRep(unsigned int r){if(r<trackReps->GetEntriesFast())_cardinal_rep=r;}
  

  /** @brief Get residuals
   *
   * @param detId which detector?
   * @param dim = index of coordinate to choose from resiudal vector
   * @param rep which track representation?
   * @param result results are written to this vector
   */
  void getResiduals(unsigned int detId, // which detector?
		    unsigned int dim,   // which projection?
		    unsigned int rep,   // which trackrep ?
		    std::vector<double>& result);
		    

  /** @brief Extrapolate all track representations to the point of closest
   * approach of pos
   */
  void gotoPoint(const TVector3& pos); // extrapolate track to a point

  /** @brief Make a step of h cm along the track
   *
   * to be used with care. Behaviour depends on track representation
   */
  void stepalong(double h); // extrapolate track by s cm (using cardinal rep)
  

  /** @brief set the hit index at which plane,state&cov of rep irep is defined
   */
  void setRepAtHit(int irep,int ihit){
    assert(irep<getNumReps());
    _repAtHit.at(irep) = ihit;
  }

  /** @brief get the hit index at which plane,state&cov of rep irep is defined
   */
  int getRepAtHit(int irep){
    assert(irep<getNumReps());
    return _repAtHit.at(irep);
  }

  /** @brief clear the hit indeces at which plane,state&cov of reps are defined
   */
  void clearRepAtHit(){
    for(unsigned int i=0;i<getNumReps();++i){
      _repAtHit.at(i)=-1;
    }
  }

  /** @brief print bookkeeping on failed hits and outliers
   */
  void printBookkeeping();
public:
  ClassDef(Track,1)
};


/* @} **/

#endif 
