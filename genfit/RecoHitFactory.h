/** @addtogroup genfit 
 * @{ */

/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */

#ifndef RECOHITFACTORY_H
#define RECOHITFACTORY_H

#include<vector>
#include<map>

#include "RecoHitProducer.h"
#include "TrackCand.h"

class AbsRecoHit;


/** @brief Factory object to create RecoHits from cluster data
 *
 * The RecoHitFactory is used to automatically fill Track objects with 
 * hit data. For each detector type that is used one RecoHitProducer 
 * has to be registered in the factory. The factory can the use the index
 * information from a TrackCand object to load the indexed hits into
 * the Track. 
 *
 * @sa AbsRecoHitProducer
 * @sa TrackCand
 */
class RecoHitFactory{
private:
  std::map<int,AbsRecoHitProducer*> hitProdMap;


public:
  RecoHitFactory();
  virtual ~RecoHitFactory();

  /** @brief Register a producer module to the factory
   *
   * For each type of hit a separate producer is needed. The type of hit
   * is identified by the detector ID (detID). This index corresponds to the
   * detector ID that is stored in the TrackCand object
   */
  void addProducer(int detID, AbsRecoHitProducer* hitProd);

  /** @brief Creat a RecoHit
   *
   * RecoHits have to implement a Constructor which takes the cluster object
   * from which the RecoHit is build as the only parameter. 
   * See AbsRecoHitProducer for details
   */
  AbsRecoHit*              createOne (int detID,int index);

  /** @brief Creat a collection of RecoHits
   *
   * This is the standard way to prepare the hit collection for a Track. The
   * resulting collection can contain hits from several detectors. The order
   * of the hits is the same as in the TrackCand. It is assumed that this order
   * is already along the track.
   *
   * RecoHits have to implement a constructor which takes the cluster object
   * from which the RecoHit is build as the only parameter. 
   * See AbsRecoHitProducer for details
   */
  std::vector<AbsRecoHit*> createMany(const TrackCand& cand);
					 

};


#endif

/** @} */
