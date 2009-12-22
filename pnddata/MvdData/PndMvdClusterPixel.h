/*
 * PndMvdClusterPixel.h
 *
 *  Created on: Dec 15, 2009
 *      Author: stockman
 */

#ifndef PNDMVDCLUSTERPIXEL_H_
#define PNDMVDCLUSTERPIXEL_H_

#include "PndMvdCluster.h"

class PndMvdClusterPixel: public PndMvdCluster {
public:
	PndMvdClusterPixel();
	PndMvdClusterPixel(std::vector<Int_t> list){
		SetClusterList(list);
	};
	virtual ~PndMvdClusterPixel();

	void SetClusterList(std::vector<Int_t> list){
		fClusterList = list;
		//SetLinks(kMVDDigiPixel, list);
	}

	ClassDef(PndMvdClusterPixel, 1);
};

#endif /* PNDMVDCLUSTERPixel_H_ */
