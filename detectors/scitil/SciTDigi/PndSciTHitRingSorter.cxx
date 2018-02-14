/*
 * PndSciTHitRingSorter.cxx
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#include "PndSciTHitRingSorter.h"
#include "PndSciTHit.h"


ClassImp(PndSciTHitRingSorter);


PndSciTHitRingSorter::~PndSciTHitRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndSciTHitRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndSciTHit(*(PndSciTHit*)data);
}
