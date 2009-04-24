/** @addtogroup genfit
 * @{ */

/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */

#ifndef TRACKCAND_H
#define TRACKCAND_H

#include <vector>
#include <set>
#include "assert.h"

#include "TObject.h"

/** @brief Track candidate -- a list of cluster indices
 *
 * The main task of the TrackCand object is to store a list of indices to
 * cluster objects. Each cluster in the Track is identified by it's
 * detector ID and it's index in the corresponding TClonesArray.
 * This information is used by the RecoHitFactory to automatically load
 * RecoHits into a Track. Through this it is possible to define Tracks over
 * an arbitrary number of different detectors.
 *
 * In addition TrackCand offers members to store starting values for the fit.
 * However this information is not autmatically used inside genfit!!!
 *
 * @sa RecoHitFactory
 */

class TrackCand : public TObject {
public:

  // Constructors/Destructors ---------
  TrackCand();
  ~TrackCand();

  /** @brief Initializing constructor
   *
   * @param curv Curvature from prefit. There is no stringent definition what
   * this parameter means at the moment.
   * @param dip Dip angle from prefit. There is no stringent definition what
   * this parameter means at the moment.
   * @param inv Dummy paramter. Has been used to mark inverted tracks 
   * in the past.
   * @param detIDs collection of detector IDs. Each detector ID needs
   * a corresponding RecoHitProducer. See RecoHitFactory for details.
   * @param hitIDs collection of hit indices. 
   */
  TrackCand(double curv, double dip, double inv, std::vector<unsigned int> detIDs, std::vector<unsigned int> hitIDs);

  // operators 
  friend bool operator== (const TrackCand& lhs, const TrackCand& rhs);

  // Accessors -----------------------
  /** @brief Get detector ID and cluster index (hitId) for hit number i 
   */
  void getHit(unsigned int i, 
	      unsigned int& detId,
			  unsigned int& hitId) const {
	assert(i<getNHits());
	detId=_detId.at(i);hitId=_hitId.at(i);
  }
  unsigned int getNHits() const {return _detId.size();}
  double getCurv() const {return _curv;}
  double getDip() const {return _dip;}
  bool inverted() const {return _inv;}
  std::vector<unsigned int> GetHitIDs(int detId=-1);
  std::vector<unsigned int> GetDetIDs() const {return _detId;}
  std::set<unsigned int> GetUniqueDetIDs() const {
    std::set<unsigned int> retVal;
    for(unsigned int i=0;i<_detId.size();++i){
      retVal.insert(_detId.at(i));
    }
    return retVal;
  }

  // Modifiers -----------------------
  void addHit(unsigned int detId, unsigned int hitId);
  void setCurv(double c){_curv=c;}
  void setDip(double d){_dip=d;}
  void setInverted(bool f=true) {_inv=f;}
  /** @brief Test if hit already is part of this track candidate
   */
  bool HitInTrack(unsigned int detId, unsigned int hitId);

  // Operations ----------------------
  void reset();

private:

  // Private Data Members ------------
  std::vector<unsigned int> _detId;
  std::vector<unsigned int> _hitId;

  double _curv; // curvature from pattern reco
  double _dip;  // dip angle from pattern reco
  bool _inv;  // true if inverted track
  // Private Methods -----------------

public:
  ClassDef(TrackCand,2)
};

#endif

/** @} */
