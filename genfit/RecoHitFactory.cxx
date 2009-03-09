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

