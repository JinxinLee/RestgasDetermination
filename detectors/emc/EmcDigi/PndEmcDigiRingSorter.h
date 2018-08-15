/*
 * PndEmcDigiRingSorter.h
 */

#ifndef PNDEMCDIGIRINGSORTER_H_
#define PNDEMCDIGIRINGSORTER_H_

#include <FairRingSorter.h>

class PndEmcDigiRingSorter: public FairRingSorter {
public:
	PndEmcDigiRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndEmcDigiRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndEmcDigiRingSorter,1);
};

#endif /* PNDEMCDIGIRINGSORTER_H_ */
