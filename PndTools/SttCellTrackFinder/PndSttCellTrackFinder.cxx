/*
 * PndSttCellTrackFinder.cxx
 *
 *  Created on: May 20, 2014
 *      Author: schumann
 */

#include "PndSttCellTrackFinder.h"
#include "TString.h"
#include "PndSttHit.h"
#include "FairRootManager.h"
#include "PndSttSkewedHit.h"
#include "PndSttCellTrackletGenerator.h"
#include "TTimeStamp.h"

using namespace std;

ClassImp(PndSttCellTrackFinder);

void PndSttCellTrackFinder::AddHits(TClonesArray* hits, Int_t branchId) {

	if (branchId == -1)
		return;

	fTrackFinderData->AddHits(hits, branchId);

}

void PndSttCellTrackFinder::FindTracks() {

	// init TrackFinderData
	vector<Double_t> timesGenerateNeighorhoodData;

	//store timestamps for runtime analysis
	timesGenerateNeighorhoodData.push_back(TTimeStamp());
	fTrackFinderData->GenerateNeighborhoodData();
	timesGenerateNeighorhoodData.push_back(TTimeStamp());

	//fTrackFinderData->PrintInfo();

	fTrackletGenerator = new PndSttCellTrackletGenerator(fTrackFinderData);
	fTrackletGenerator->SetCalcFirstTrackletInf(fCalcFirstTrackletInf);
	fTrackletGenerator->SetVerbose(fVerbose);

	//calculation on GPU is only possible without multiple STTHits and a maximum number of MAX_THREADS_PER_BLOCK
	if (fUseGPU && (fTrackFinderData->GetAllowDoubleHits() == kFALSE)
			&& (fTrackFinderData->GetNumHitsWithoutDouble()
					< MAX_THREADS_PER_BLOCK)) {
		fTrackletGenerator->SetUseGPU(fUseGPU);
		fTrackletGenerator->SetDevTubeNeighboringsPointer(
				fDev_tubeNeighborings);
	}

	if (fCalcWithCorrectedIsochrones) {
		fHitCorrector = new PndSttHitCorrector(fTrackFinderData);
		fHitCorrector->CorrectHits();
		map<int, FairHit*> correctedIsochrones =
				fHitCorrector->GetCorrectedHits();

		//Get tracks with corrected hits
		fTrackletGenerator->SetCorrectedHits(correctedIsochrones);
	}

	fTrackletGenerator->FindTracks();
	//fTrackletGenerator->PrintInfo();

	StoreTrackData();

	// cpu time measurement
	fTimeStampsTrackletGen.push_back(fTrackletGenerator->GetTimeStamps());
	fTimeStampsGenerateNeighborhoodData.push_back(timesGenerateNeighorhoodData);
}

;
