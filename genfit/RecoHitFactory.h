#ifndef FITTER_RECOHITFACTORY_H
#define FITTER_RECOHITFACTORY_H
// Original Authors: Christian Hoeppner (TUM)

#include<vector>
#include<map>

#include "RecoHitProducer.h"
#include "TrackCand.h"

class AbsRecoHit;

class RecoHitFactory{
private:
  std::map<int,AbsRecoHitProducer*> hitProdMap;


public:
  RecoHitFactory();
  virtual ~RecoHitFactory();

  void addProducer(int detID, AbsRecoHitProducer* hitProd);

  AbsRecoHit*              createOne (int detID,int index);
  std::vector<AbsRecoHit*> createMany(const TrackCand& cand);
					   

};


#endif // FITTER_RECOHITFACTORY_H
