/** @addtogroup genfit 
 * @{ */


/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */

#ifndef RECOHITPRODUCER_H
#define RECOHITPRODUCER_H

#include<vector>
#include<map>
#include<iostream>

#include "TClonesArray.h"

#include "FitterExceptions.h"

class AbsRecoHit;

/** @brief Abstract interface class for RecoHitProducer
 *
 * Defines the very basic interface of a producer.
 */
class AbsRecoHitProducer {
public:
  /** @brief Virtual abstract method to produce a RecoHit. 
   * Implemented in RecoHitProducer
   */
  virtual AbsRecoHit* produce(int index)=0;
  virtual ~AbsRecoHitProducer();
};


/** @brief Template class for a hit producer module
 *
 * A RecoHitProducer module is used by RecoHitFactory to create RecoHits for
 * one specific detector type. 
 *
 * It is assumed that each detector has as output of its digitization /
 * clustering some sort of cluster class which stores all information that
 * corresponds to a measured hit in that detector. The RecoHit producer 
 * converts this information into a class that can be handled by genfit.
 * This class is realized as a RecoHit (a class inherting from AbsRecoHit).
 *
 * In order to use the RecoHitProducer facility a
 * RecoHit has to implement a constructor which takes as an argument 
 * a pointer to the cluster class. This constructor serves as the initializing
 * constructor for the RecoHit.
 *
 * The RecoHitProducer will fetch the cluster objects from a TClonesArray and
 * use the initializing constructor to build the corresponding RecoHit. 
 *
 * @param hit_t template parameter specifying cluster class
 * @param recoHit_t template parameter specifying recoHit
 */
template <class hit_T,class recoHit_T>
class RecoHitProducer : public AbsRecoHitProducer {
 private:
  /** @brief pointer to array with cluster data */
  TClonesArray* hitArray;
 public:

  /** @brief Constructor takes pointer to the cluster array */
  RecoHitProducer(TClonesArray*);
  virtual ~RecoHitProducer();

  /** @brief Create a RecoHit from the cluster at position index 
   * in TClonesArray
   */
  virtual AbsRecoHit* produce(int index);	
};

template <class hit_T,class recoHit_T>
RecoHitProducer<hit_T,recoHit_T>::RecoHitProducer(TClonesArray* theArr) {
  hitArray = theArr;
}

template <class hit_T,class recoHit_T>
RecoHitProducer<hit_T,recoHit_T>::~RecoHitProducer() {
}


template <class hit_T,class recoHit_T>
AbsRecoHit* RecoHitProducer<hit_T,recoHit_T>::produce(int index) {
  //the ROOT guys really use 0 and not NULL grrr...
  if(hitArray->At(index) == 0) {
	throw FitterException("In RecoHitProducer: index for hit in TClonesArray out of bounds",__LINE__,__FILE__);
  }
  return ( new recoHit_T( (hit_T*) hitArray->At(index) ) );
}


#endif 

/** @} */
