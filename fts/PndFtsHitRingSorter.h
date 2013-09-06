/*
 * PndFtsHitRingSorter.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PndFtsHitRINGSORTER_H_
#define PndFtsHitRINGSORTER_H_

#include <FairRingSorter.h>

class PndFtsHitRingSorter: public FairRingSorter {
public:
	PndFtsHitRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndFtsHitRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndFtsHitRingSorter,1);
};

#endif /* PndFtsHitRINGSORTER_H_ */
