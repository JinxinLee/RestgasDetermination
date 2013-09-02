/*
 * PndEmcWaveformRingSorter.h
 */

#ifndef PNDEMCWAVEFORMRINGSORTER_H_
#define PNDEMCWAVEFORMRINGSORTER_H_

#include <FairRingSorter.h>

class PndEmcWaveformRingSorter: public FairRingSorter {
public:
	PndEmcWaveformRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndEmcWaveformRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndEmcWaveformRingSorter,1);
};

#endif /* PNDEMCWAVEFORMRINGSORTER_H_ */
