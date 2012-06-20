/*
 * PndEmcDigiRingSorter.cxx
 */

#include <PndEmcDigiRingSorter.h>
#include "PndEmcDigi.h"


ClassImp(PndEmcDigiRingSorter);


PndEmcDigiRingSorter::~PndEmcDigiRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndEmcDigiRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndEmcDigi(*(PndEmcDigi*)data);
}
