/*
 * PndRichHitRingSorter.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndRichHitRingSorter source file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include <PndRichHitRingSorter.h>
#include "PndRichDigi.h"


ClassImp(PndRichHitRingSorter);


PndRichHitRingSorter::~PndRichHitRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndRichHitRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndRichDigi(*(PndRichDigi*)data);
}
