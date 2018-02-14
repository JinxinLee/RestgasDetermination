/*
 * PndEmcWaveformRingSorter.cxx
 */

#include <PndEmcWaveformRingSorter.h>
#include "PndEmcWaveform.h"


ClassImp(PndEmcWaveformRingSorter);


PndEmcWaveformRingSorter::~PndEmcWaveformRingSorter() {
	// TODO Auto-generated destructor stub
}

FairTimeStamp* PndEmcWaveformRingSorter::CreateElement(FairTimeStamp* data) {
	return new PndEmcWaveform(*(PndEmcWaveform*)data);
}
