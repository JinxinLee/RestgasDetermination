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

#ifndef FITTER_RECOHITPRODUCER_H
#define FITTER_RECOHITPRODUCER_H
// Original Authors: Christian Hoeppner (TUM)

#include<vector>
#include<map>
#include<iostream>

#include "TClonesArray.h"

#include "FitterExceptions.h"

class AbsRecoHit;

class AbsRecoHitProducer {
public:
  virtual AbsRecoHit* produce(int index)=0;
  virtual ~AbsRecoHitProducer();
};



template <class hit_T,class recoHit_T>
class RecoHitProducer : public AbsRecoHitProducer {
 private:
  TClonesArray* hitArray;
 public:
  RecoHitProducer(TClonesArray*);
  virtual ~RecoHitProducer();

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


#endif // FITTER_RECOHITPRODUCER_H
