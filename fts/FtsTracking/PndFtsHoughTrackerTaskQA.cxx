#include "PndFtsHoughTrackerTaskQA.h"
#include "PndFtsHoughTrackerTask.h"

#include <iostream>
#include <math.h>

// FTS
#include "PndGeoFtsPar.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
#include "PndFtsHit.h"
#include "FairHit.h"


// magnetic field
#include "FairField.h"


// (Hough) tracking
#include "PndFtsHoughTrackFinder.h"
#include "PndFtsHoughTrackFinderQA.h"
#include "PndFtsHoughSpace.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "PndFtsHoughTrackCand.h"

// histogramming / plotting
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

// peak finder
#include "TSpectrum2.h"

// root IO

#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"



#include "TString.h"


// TODO this list can probably be shorter
//#include "PndDetectorList.h"

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"


#include "TObjectTable.h"

#include "PndFtsMapCreator.h"


using std::cout;
using std::endl;






// ---- Default constructor -------------------------------------------
PndFtsHoughTrackerTaskQA::PndFtsHoughTrackerTaskQA(Int_t verbose)
: PndFtsHoughTrackerTask(verbose, kTRUE)
{
	if(3<fVerbose) std::cout << "PndFtsHoughTrackerTaskQA is the tracker ptr " << this << '\n';
}

// ---- Destructor ----------------------------------------------------
PndFtsHoughTrackerTaskQA::~PndFtsHoughTrackerTaskQA()
{
	if(fVerbose>3) fLogger->Info(MESSAGE_ORIGIN,"Destructor of PndFtsHoughTrackerTaskQA");
}




// ---- Init ----------------------------------------------------------
InitStatus PndFtsHoughTrackerTaskQA::Init()
{
	if(fVerbose>3) Info("Init","Initilization of PndFtsHoughTrackerTaskQA");

	PndFtsHoughTrackerTask::Init();
}


// ---- ReInit  -------------------------------------------------------
InitStatus PndFtsHoughTrackerTaskQA::ReInit()
{
	InitStatus stat=kSUCCESS;
	if(3<fVerbose) fLogger->Info(MESSAGE_ORIGIN,"Re- Initilization of PndFtsHoughTrackerTaskQA");
	PndFtsHoughTrackerTask::ReInit();
	return stat;
}




// ---- Exec ----------------------------------------------------------
void PndFtsHoughTrackerTaskQA::Exec(Option_t* option)
{
	if(1<fVerbose) Info("Exec","Exec of PndFtsHoughTrackerTaskQA on event %i", fEventNr);

	if(3<fVerbose) std::cout << "PndFtsHoughTrackFinder::Exec tracker ptr " << this << '\n';
	PndFtsHoughTrackFinderQA trackFinder(this);
	//	trackFinder.SetMinPeakHeightZxLineParabola(4);
	//	trackFinder.SetMinPeakHeightZxParabola(6);
	//	trackFinder.SetMinPeakHeightZxParabolaLine(4);
	//	trackFinder.SetMinPeakHeightZyLine(4);
	trackFinder.FindTracks();

	if(0<fVerbose){
//		Int_t nEvtsWithParabolasFound = trackFinder.getNEvtsWithParabolasFound();
//		Int_t nEvtsWithTracksFound = trackFinder.getNEvtsWithTracksFound();
//		std::cout << nEvtsWithParabolasFound << " events have >= 1 parabola.\n";
//		std::cout << nEvtsWithTracksFound << " events have >= 1 track.\n";
	}


	if(3<fVerbose) Info("Exec","End eventloop.");
	++fEventNr;
}



void PndFtsHoughTrackerTaskQA::FinishEvent()
{
}


// ---- Finish --------------------------------------------------------
void PndFtsHoughTrackerTaskQA::Finish()
{
	if(3<fVerbose) Info("Finish","Found %i tracks.",fTrackCands->GetEntriesFast());

}





ClassImp(PndFtsHoughTrackerTaskQA)
