/*
 * PndSdsDigiStripRingSorter.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDigiStripRINGSORTER_H_
#define PNDSDSDigiStripRINGSORTER_H_

#include <FairRingSorter.h>

class PndSdsDigiStripRingSorter: public FairRingSorter {
public:
	PndSdsDigiStripRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndSdsDigiStripRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndSdsDigiStripRingSorter,1);
};

#endif /* PNDSDSDigiStripRINGSORTER_H_ */
