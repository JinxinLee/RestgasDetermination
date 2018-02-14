/*
 * PndSciTHitRingSorter.h
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#ifndef PNDSCITHITRINGSORTER_H
#define PNDSCITHITRINGSORTER_H

#include "FairRingSorter.h"

class PndSciTHitRingSorter: public FairRingSorter {
public:
	PndSciTHitRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndSciTHitRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndSciTHitRingSorter,1);
};

#endif /* PndSciTHitRingSorter_H */
