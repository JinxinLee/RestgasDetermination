/** @addtogroup genfit
 * @{
 */


#ifndef TRACK_H 
#define TRACK_H

#include "AbsTrackRep.h"
#include "AbsRecoHit.h"

#include "TClonesArray.h"
#include "TObjArray.h"

#include "TrackCand.h"

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
   */
  TObjArray* trackReps; //->

  /** @brief Collection of REcoHits
   */
  std::vector<AbsRecoHit*>      hits;//!
  
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

  /** @brief Get present position
   *
   * Cardinal representation is used.
   */
  TVector3 getPos() const {return getCardinalRep()->getPos();}

  /** @brief Get chi2
   *
   * Cardinal representation is used.
   */
  double getChiSqu() const {return getCardinalRep()->getChiSqu();}

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
   * The given track represenatation has to contain starting values for fit!
   */
  void addTrackRep(AbsTrackRep* theTrackRep) {
    if(trackReps==NULL)trackReps=new TObjArray(defNumTrackReps);
    trackReps->Add(theTrackRep);
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
   * @param index of coordinate to choose from resiudal vector
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
  
public:
  ClassDef(Track,1)
};


/* @} **/

#endif 
