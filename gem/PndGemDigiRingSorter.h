/*
 * PndGemDigiRingSorter.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDGEMDIGIRINGSORTER_H_
#define PNDGEMDIGIRINGSORTER_H_

#include <FairRingSorter.h>

class PndGemDigiRingSorter: public FairRingSorter {
public:
	PndGemDigiRingSorter(int size = 100, double width = 10):FairRingSorter(size, width){};
	virtual ~PndGemDigiRingSorter();

	virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

	ClassDef (PndGemDigiRingSorter,1);
};

#endif /* PNDGEMDIGIRINGSORTER_H_ */
