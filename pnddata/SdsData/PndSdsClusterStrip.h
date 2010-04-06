/*
 * PndSdsClusterStrip.h
 *
 *  Created on: Dec 15, 2009
 *      Author: stockmanns, kliemt, michel
 */

#ifndef PNDSDSCLUSTERSTRIP_H_
#define PNDSDSCLUSTERSTRIP_H_

#include "PndSdsCluster.h"

class PndSdsClusterStrip: public PndSdsCluster {
public:
	PndSdsClusterStrip();
	PndSdsClusterStrip(std::vector<Int_t> list){
		SetClusterList(list);
	};
	virtual ~PndSdsClusterStrip();

	void SetClusterList(std::vector<Int_t> list){
		fClusterList = list;
		SetLinks(FairMultiLinkedData(kMVDDigiStrip, list));
	}

	SensorSide GetSensorSide() const {return fSide;}
	void SetSensorSide(SensorSide s) {fSide = s;}

private:
    SensorSide fSide;

	ClassDef(PndSdsClusterStrip, 1);
};

#endif /* PNDSDSCLUSTERSTRIP_H_ */
