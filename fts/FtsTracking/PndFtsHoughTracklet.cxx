#include "PndFtsHoughTracklet.h"

#include <iostream>
#include "math.h"

#include "FairRootManager.h"
#include "PndFtsHit.h"



ClassImp(PndFtsHoughTracklet);

PndFtsHoughTracklet::PndFtsHoughTracklet() :
						fVerbose(0),

						fIsSet(kFALSE),

						fPeakHeightFromPeakFinder(0.),

						fThetaVal(0.),
						fThetaHw(0.),

						fSecondVal(0.),
						fSecondHw(0.)
{

}

PndFtsHoughTracklet::~PndFtsHoughTracklet()
{

}




void PndFtsHoughTracklet::SetHoughTransformResults(const Double_t thetaVal, const Double_t secondVal, const Double_t peakHeight, const Double_t thetaHw, const Double_t secondHw){
	fPeakHeightFromPeakFinder = peakHeight;

	fThetaVal = thetaVal;
	fSecondVal = secondVal;
	fThetaHw = thetaHw;
	fSecondHw = secondHw;

	fIsSet = kTRUE;
}


UInt_t PndFtsHoughTracklet::getNumberOfSharedHits(PndFtsHoughTracklet& rhs){
	UInt_t numberOfSharedHits = 0;
	// go through all hits in this and check if they are also in rhs, if yes, increase numberOfSharedHits by 1 per shared hit
	for (UInt_t iHit = 0; iHit < GetNHits(); ++iHit){
		PndTrackCandHit thisHit = GetSortedHit(iHit);
		Int_t thisHitId = thisHit.GetHitId();
		Int_t thisDetId = thisHit.GetDetId();
		// if hit is NOT in track -1 is returned, otherwise the index in the HitId vector is returned
		if (-1<rhs.HitInTrack(thisDetId,thisHitId)){
			// hit is in track
			++numberOfSharedHits;
		}
	}
	return numberOfSharedHits;
}


void PndFtsHoughTracklet::Print() {
	std::cout << "=========== PndFtsHoughTracklet::Print() ==========" << std::endl;
	if (kTRUE==fIsSet){
		std::cout << "theta = " << fThetaVal << "  second = " << fSecondVal << std::endl << std::endl;
		std::cout << "theta = " << fThetaHw << "  second = " << fSecondHw << std::endl << std::endl;
	}
}




const PndFtsHit* PndFtsHoughTracklet::getHit(UInt_t index) {
	// Make sure we have a complete track candidate before we try to access any hits
	if (kTRUE==fIsSet) return 0;
	if (index < GetNHits()){
		TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
		const PndFtsHit *myHit = (PndFtsHit*) ftsHitArray->At(GetSortedHit(index).GetHitId());
		return myHit;
	} else {
		return 0;
	}
}


