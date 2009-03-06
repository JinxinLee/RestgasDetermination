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

#include "RecoHitFactory.h"
#include<iostream>


RecoHitFactory::RecoHitFactory(){
}

RecoHitFactory::~RecoHitFactory(){
  std::map<int, AbsRecoHitProducer*>::iterator it=hitProdMap.begin();
  while(it!=hitProdMap.end()){
	delete it->second;
	++it;
  }
}

void RecoHitFactory::addProducer(int detID, AbsRecoHitProducer* hitProd) {
  if(hitProdMap[detID] != NULL) {
	FitterException exc("RecoHitFactory: detID already in use",__LINE__,__FILE__);
	std::vector<double> numbers;
	numbers.push_back(detID);
	exc.setNumbers("detID",numbers);
	throw exc;
  }
  else {
	hitProdMap[detID] = hitProd;
  }
}

AbsRecoHit* RecoHitFactory::createOne(int detID, int index) {
  if(hitProdMap[detID] != NULL) {
	return hitProdMap[detID]->produce(index);
  }


  else {
	FitterException exc("RecoHitFactory: no hitProducer for this detID available",__LINE__,__FILE__);
	std::vector<double> numbers;
	numbers.push_back(detID);
	exc.setNumbers("detID",numbers);
	throw exc;
  }
}

std::vector<AbsRecoHit*> RecoHitFactory::createMany(const TrackCand& cand){
  std::vector<AbsRecoHit*> hitVec;
  unsigned int nHits=cand.getNHits();
  for(unsigned int i=0;i<nHits;i++) {
    unsigned int detID;
    unsigned int index;
    cand.getHit(i,detID,index);
	hitVec.push_back( createOne(detID,index) );
  }
  return hitVec;
}

