/*
 * PndSdsDigiStripRingSorter.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include <PndSdsDigiStripRingSorter.h>
#include "PndSdsDigiStrip.h"


ClassImp(PndSdsDigiStripRingSorter);


PndSdsDigiStripRingSorter::~PndSdsDigiStripRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndSdsDigiStripRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndSdsDigiStrip(*(PndSdsDigiStrip*)data);
}
