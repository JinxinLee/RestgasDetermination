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
