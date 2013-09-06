/*
 * PndFtsHitRingSorter.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include <PndFtsHitRingSorter.h>
#include "PndFtsHit.h"


ClassImp(PndFtsHitRingSorter);


PndFtsHitRingSorter::~PndFtsHitRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndFtsHitRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndFtsHit(*(PndFtsHit*)data);
}
