/*
 *      Author: stockman
 * PndRichHitRingSorter.h
 *
 */
// -------------------------------------------------------------------------
// -----                   PndRichHitRingSorter header file              -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHHITRINGSORTER_H_
#define PNDRICHHITRINGSORTER_H_

#include <FairRingSorter.h>

class PndRichHitRingSorter: public FairRingSorter {
public:
	PndRichHitRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndRichHitRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndRichHitRingSorter,1);
};

#endif /* PNDRICHHITRINGSORTER_H_ */
