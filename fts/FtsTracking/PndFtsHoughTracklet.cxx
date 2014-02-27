#include "PndFtsHoughTracklet.h"

#include <iostream>
#include "math.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndFtsHit.h"
#include "PndTrack.h"




ClassImp(PndFtsHoughTracklet);

PndFtsHoughTracklet::PndFtsHoughTracklet(Double_t zRefLabSys, Int_t ftsBranchId, TClonesArray *ftsHitArray) :
				fVerbose(0),

				fIsSet(kFALSE),

				fZRefLabSys(zRefLabSys),

				fPeakHeightFromPeakFinder(0.),

				fThetaVal(0.),
				fThetaHw(0.),

				fSecondVal(0.),
				fSecondHw(0.),

				fFtsHitArray(ftsHitArray),
				fFtsBranchId(ftsBranchId)
{
	if (0==ftsHitArray){
		std::cout << "PndFtsHoughTracklet FATAL ERROR Hit array not set in constructor.\n";
	}
}

PndFtsHoughTracklet::~PndFtsHoughTracklet()
{

}




void PndFtsHoughTracklet::SetHoughTransformResults(
		const Double_t thetaVal,
		const Double_t secondVal,

		const Double_t peakHeight,

		const Double_t thetaHw,
		const Double_t secondHw
){
	fPeakHeightFromPeakFinder = peakHeight;

	// set values from Hough space peak
	fThetaVal = thetaVal;
	fSecondVal = secondVal;
	fThetaHw = thetaHw;
	fSecondHw = secondHw;

	//	addPeakHits();

	fIsSet = kTRUE;
}


//void PndFtsHoughTracklet::addPeakHits(){
//	// check which hits are within the peak region and add the hits
//	// functionality is not implemented here, because I don't know how to pass the equation into this class without copy pasting it
//
//}


UInt_t PndFtsHoughTracklet::getNSharedHits(PndFtsHoughTracklet& rhs){
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
		//TClonesArray *ftsHitArray= (TClonesArray *)FairRootManager::Instance()->GetObject("FTSHit");
		const PndFtsHit *myHit = (PndFtsHit*) fFtsHitArray->At(GetSortedHit(index).GetHitId());
		return myHit;
	} else {
		return 0;
	}
}


