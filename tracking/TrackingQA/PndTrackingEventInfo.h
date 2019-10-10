#ifndef PNDTRACKINGEventINFO_H
#define PNDTRACKINGEventINFO_H

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"
#include <vector>
#include "FairLink.h"
#include "PndTrack.h"
#include <TClonesArray.h>

class PndTrackingEventInfo : public TObject {


public:

    	PndTrackingEventInfo(PndTrack*, FairLink);

	~PndTrackingEventInfo();

	void CalcEventPurity(PndTrack*, FairLink);

	double GetEventPurity(){return fEventPurity;}

protected:

	double fEventPurity;

	ClassDef(PndTrackingEventInfo,2);
};

#endif
