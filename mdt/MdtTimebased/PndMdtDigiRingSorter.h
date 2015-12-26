/*
 * PndMdtDigiRingSorter.h
 */

#ifndef PNDMDTDIGIRINGSORTER_H_
#define PNDMDTDIGIRINGSORTER_H_

#include <FairRingSorter.h>

class PndMdtDigiRingSorter: public FairRingSorter {
public:
	PndMdtDigiRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndMdtDigiRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndMdtDigiRingSorter,1);
};

#endif /* PNDMDTDIGIRINGSORTER_H_ */
