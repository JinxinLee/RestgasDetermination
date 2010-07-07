/*
 * PndSdsClusterPixel.h
 *
 *  Created on: Dec 15, 2009
 *      Author: stockmanns, kliemt, michel
 */

#ifndef PNDSDSCLUSTERPIXEL_H_
#define PNDSDSCLUSTERPIXEL_H_

#include "PndSdsCluster.h"

class PndSdsClusterPixel: public PndSdsCluster {
public:
	PndSdsClusterPixel();
	PndSdsClusterPixel(Int_t DigiType, std::vector<Int_t> list){
		SetClusterList(DigiType, list);
	};
	virtual ~PndSdsClusterPixel();

	void SetClusterList(Int_t DigiType, std::vector<Int_t> list){
		fClusterList = list;
		SetLinks(FairMultiLinkedData(DigiType, list));
	}

	ClassDef(PndSdsClusterPixel, 1);
};

#endif /* PNDSDSCLUSTERPixel_H_ */
