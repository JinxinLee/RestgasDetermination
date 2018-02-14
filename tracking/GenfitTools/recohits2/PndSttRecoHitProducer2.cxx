// ========================================
// PndSttRecoHitProducer2
//
// Class for kalman fitting with genfit for STT
// 
// This STT specific reco hit producer was created to be able 
// to access the tube array TCA from in addition to the hit TCA
// when constructing the reco hit.
//
// It inherits from AbsRecoHitProducer and has been written
// following the RecoHitProducer
// ========================================

#include "PndSttRecoHitProducer2.h"

#include<iostream>

// template <class hit_T,class recoHit_T>
//   PndSttRecoHitProducer<hit_T,recoHit_T>::PndSttRecoHitProducer(TClonesArray* theArr, TClonesArray* theTubeArr) {
//   hitArrayTClones = theArr;
//   tubeArrayTClones = theTubeArr;
// }

// template <class hit_T,class recoHit_T>
//   PndSttRecoHitProducer<hit_T,recoHit_T>::~PndSttRecoHitProducer() {
// }


// template <class hit_T,class recoHit_T>
//   GFAbsRecoHit* PndSttRecoHitProducer<hit_T,recoHit_T>::produce(int index) {
//   assert(hitArrayTClones!=NULL);
//   assert(tubeArrayTClones!=NULL);
//   if(hitArrayTClones->At(index) == 0) {
//     GFException e("In PndSttRecoHitProducer: index for hit in TClonesArray out of bounds",__LINE__,__FILE__);
//     e.setFatal();
//     throw e;
//   }

//   return ( new recoHit_T( (hit_T*) hitArrayTClones->At(index), tubeArrayTClones ) );
// }




