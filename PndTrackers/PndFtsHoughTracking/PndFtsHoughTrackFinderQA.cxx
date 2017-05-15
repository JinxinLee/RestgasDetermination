#include "PndFtsHoughTrackFinderQA.h"



ClassImp(PndFtsHoughTrackFinderQA);


PndFtsHoughTrackFinderQA::PndFtsHoughTrackFinderQA(PndFtsHoughTrackerTask *trackerTask)
: PndFtsHoughTrackFinder(trackerTask)
{
	if (0==fTrackerTask){
		std::cout << "PndFtsHoughTrackFinderQA FATAL ERROR Tracker task not set.\n";
	} else {
		if(3<fTrackerTask->GetVerbose()) std::cout << "PndFtsHoughTrackFinderQA called with tracker ptr " << fTrackerTask << '\n';
	}
}


PndFtsHoughTrackFinderQA::~PndFtsHoughTrackFinderQA()
{
	if(3<fTrackerTask->GetVerbose()) fTrackerTask->fLogger->Info(MESSAGE_ORIGIN,"Destructor of PndFtsHoughTrackFinderQA");
}


void PndFtsHoughTrackFinderQA::FindTracks() {

	PndFtsHoughTrackFinder::FindTracks();



}
