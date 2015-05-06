/*
 * PndMvdTopix4RingSorter.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include "PndSdsDigiTopix4.h"
#include "PndMvdTopix4RingSorter.h"


ClassImp(PndMvdTopix4RingSorter);


PndMvdTopix4RingSorter::~PndMvdTopix4RingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndMvdTopix4RingSorter::CreateElement(FairTimeStamp* data) {
	return new PndSdsDigiTopix4(*(PndSdsDigiTopix4*)data);
}
