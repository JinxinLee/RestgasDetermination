/*
 * PndSttHitRingSorter.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PndSttHitRINGSORTER_H_
#define PndSttHitRINGSORTER_H_

#include <FairRingSorter.h>

class PndSttHitRingSorter: public FairRingSorter {
public:
	PndSttHitRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndSttHitRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndSttHitRingSorter,1);
};

#endif /* PndSttHitRINGSORTER_H_ */
