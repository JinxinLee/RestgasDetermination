/*
 * PndGemDigiRingSorter.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include <PndGemDigiRingSorter.h>
#include "PndGemDigi.h"


ClassImp(PndGemDigiRingSorter);


PndGemDigiRingSorter::~PndGemDigiRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndGemDigiRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndGemDigi(*(PndGemDigi*)data);
}
