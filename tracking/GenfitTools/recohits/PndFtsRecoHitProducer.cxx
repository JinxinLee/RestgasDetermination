// ========================================
// PndFtsRecoHitProducer
//
// Class for kalman fitting with genfit for FTS
// 
// This FTS specific reco hit producer was created to be able 
// to access the tube array TCA from in addition to the hit TCA
// when constructing the reco hit.
//
// It inherits from GFAbsRecoHitProducer and has been written
// following the GFRecoHitProducer
// ========================================

#include "PndFtsRecoHitProducer.h"

#include<iostream>

// template <class hit_T,class recoHit_T>
//   PndFtsRecoHitProducer<hit_T,recoHit_T>::PndFtsRecoHitProducer(TClonesArray* theArr, TClonesArray* theTubeArr) {
//   hitArrayTClones = theArr;
//   tubeArrayTClones = theTubeArr;
// }

// template <class hit_T,class recoHit_T>
//   PndFtsRecoHitProducer<hit_T,recoHit_T>::~PndFtsRecoHitProducer() {
// }


// template <class hit_T,class recoHit_T>
//   GFAbsRecoHit* PndFtsRecoHitProducer<hit_T,recoHit_T>::produce(int index) {
//   assert(hitArrayTClones!=NULL);
//   assert(tubeArrayTClones!=NULL);
//   if(hitArrayTClones->At(index) == 0) {
//     GFException e("In PndFtsRecoHitProducer: index for hit in TClonesArray out of bounds",__LINE__,__FILE__);
//     e.setFatal();
//     throw e;
//   }

//   return ( new recoHit_T( (hit_T*) hitArrayTClones->At(index), tubeArrayTClones ) );
// }




