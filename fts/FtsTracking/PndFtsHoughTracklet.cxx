#include "PndFtsHoughTracklet.h"

#include "PndFtsHoughTrackerTask.h"

#include <iostream>
#include "math.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndFtsHit.h"
#include "PndTrack.h"




ClassImp(PndFtsHoughTracklet);

PndFtsHoughTracklet::PndFtsHoughTracklet(Double_t zRefLabSys, PndFtsHoughTrackerTask *trackerTask) :
						fTrackerTask(trackerTask),

						fVerbose(0),

						fIsSet(kFALSE),

						fZRefLabSys(zRefLabSys),

						fPeakHeightFromPeakFinder(0.),

						fThetaRadVal(0.),
						fThetaRadHw(0.),

						fSecondVal(0.),
						fSecondHw(0.)
{
	if (0==fTrackerTask){
		std::cout << "PndFtsHoughTracklet FATAL ERROR Tracker task pointer not set in constructor.\n";
	} else {
		fVerbose = fTrackerTask->GetVerbose();
		if(3<fVerbose) std::cout << "PndFtsHoughTracklet called with tracker ptr " << fTrackerTask << '\n';
	}
}

PndFtsHoughTracklet::~PndFtsHoughTracklet()
{

}




void PndFtsHoughTracklet::SetHoughTransformResults(
		const Double_t thetaRadVal,
		const Double_t secondVal,

		const Double_t peakHeight,

		const Double_t thetaRadHw,
		const Double_t secondHw
){
	fPeakHeightFromPeakFinder = peakHeight;

	// set values from Hough space peak
	fThetaRadVal = thetaRadVal;
	fSecondVal = secondVal;
	fThetaRadHw = thetaRadHw;
	fSecondHw = secondHw;

	//	addPeakHits();

	fIsSet = kTRUE;
}




UInt_t PndFtsHoughTracklet::getNSharedHits(const PndFtsHoughTracklet& rhs) {
	UInt_t nSharedHits = 0;
	// go through all hits in *this and check if they are also in rhs, if yes, increase numberOfSharedHits by 1 (per shared hit)
	for (UInt_t iHit = 0; iHit < GetNHits(); ++iHit){
		PndTrackCandHit thisHit = GetSortedHit(iHit);
		Int_t thisHitId = thisHit.GetHitId();
		Int_t thisDetId = thisHit.GetDetId();
		// if hit is NOT in track -1 is returned by HitInTrack, otherwise the index (>=0) in the HitId vector is returned
		if (-1<rhs.HitInTrack(thisDetId,thisHitId)){
			// hit is in track
			++nSharedHits;
		}
	}
	return nSharedHits;
}


void PndFtsHoughTracklet::Print() const {
	std::cout << "=========== PndFtsHoughTracklet::Print() ==========" << std::endl;
	if (kTRUE==fIsSet){
		std::cout << "theta = " << fThetaRadVal << "  second = " << fSecondVal << std::endl << std::endl;
		std::cout << "theta = " << fThetaRadHw << "  second = " << fSecondHw << std::endl << std::endl;
	}
}

