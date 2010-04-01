/*
 * PndMvdClusterStrip.h
 *
 *  Created on: Dec 15, 2009
 *      Author: stockman
 */

#ifndef PNDMVDCLUSTERSTRIP_H_
#define PNDMVDCLUSTERSTRIP_H_

#include "PndMvdCluster.h"

class PndMvdClusterStrip: public PndMvdCluster {
public:
	PndMvdClusterStrip();
	PndMvdClusterStrip(std::vector<Int_t> list){
		SetClusterList(list);
	};
	virtual ~PndMvdClusterStrip();

	void SetClusterList(std::vector<Int_t> list){
		fClusterList = list;
		SetLinks(FairMultiLinkedData(kMVDDigiStrip, list));
	}

	SensorSide GetSensorSide() const {return fSide;}
	void SetSensorSide(SensorSide s) {fSide = s;}

private:
    SensorSide fSide;

	ClassDef(PndMvdClusterStrip, 1);
};

#endif /* PNDMVDCLUSTERSTRIP_H_ */
