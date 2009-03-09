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


#endif
