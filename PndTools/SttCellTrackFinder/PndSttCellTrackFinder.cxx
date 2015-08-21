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

ClassImp(PndSttCellTrackFinder)
;

void PndSttCellTrackFinder::SetSttTubeArray(TClonesArray* sttTubeArray) {

	//Generate TrackFinderData-Object
	fTrackFinderData= new PndSttCellTrackFinderData(sttTubeArray);

}

void PndSttCellTrackFinder::AddHits(TClonesArray* hits, Int_t branchId) {

	if(branchId==-1) return;

	TString geoPath;
	PndSttHit* myHit;
	FairLink myID;

	if (branchId == FairRootManager::Instance()->GetBranchId("STTHit")) {

		for (int i = 0; i < hits->GetEntries(); i++) {
			myHit = (PndSttHit*) (hits->At(i));

			if (myHit->GetEntryNr().GetIndex() < 0) {
				myID = FairLink(branchId, i);
				myHit->SetEntryNr(FairLink(branchId, i));
			} else
				myID = myHit->GetEntryNr();
			myHit->SetDxyz(myHit->GetIsochrone(), myHit->GetIsochrone(), 100);
			fMapHitToFairLink[i] = myID;
			fHits.push_back((FairHit*) myHit);

		}

	} else if (branchId
			== FairRootManager::Instance()->GetBranchId(
					"STTCombinedSkewedHits")) {
		for (int i = 0; i < hits->GetEntries(); i++) {
			PndSttSkewedHit* skewedHit = (PndSttSkewedHit*) (hits->At(i));
			int tubeId = skewedHit->GetTubeIDs().first;
			fCombinedSkewedHits.insert(
					std::pair<int, PndSttSkewedHit*>(tubeId, skewedHit));
			if (skewedHit->GetEntryNr().GetIndex() < 0) {
				myID = FairLink(branchId, i);
				skewedHit->SetEntryNr(FairLink(branchId, i));
			} else
				myID = skewedHit->GetEntryNr();
		}
	}

}

void PndSttCellTrackFinder::FindTracks(){
	// init TrackFinderData
	vector<Double_t> timesGenerateNeighorhoodData;

	//store timestamps for runtime analysis
	timesGenerateNeighorhoodData.push_back(TTimeStamp());
	fTrackFinderData->GenerateNeighborhoodData(fHits, fCombinedSkewedHits, false);
	timesGenerateNeighorhoodData.push_back(TTimeStamp());

	//fTrackFinderData->PrintInfo();

	fTrackletGenerator = new PndSttCellTrackletGenerator(fTrackFinderData, fMapHitToFairLink);
	fTrackletGenerator->SetCalcFirstTrackletInf(fCalcFirstTrackletInf);
	fTrackletGenerator->FindTracks();

	//fTrackletGenerator->PrintInfo();

	StoreTrackData();

	fHitCorrector = new PndSttHitCorrector(fTrackFinderData);
	fHitCorrector->CorrectHits();
	map<int, FairHit*> correctedIsochrones=fHitCorrector->GetCorrectedHits();

	//Get tracks with corrected hits
	fTrackletGenerator->SetCorrectedHits(correctedIsochrones);
	fTrackletGenerator->RefitTracks();

	// cpu time measurement
	fTimeStampsTrackletGen.push_back(fTrackletGenerator->GetTimeStamps());
	fTimeStampsGenerateNeighborhoodData.push_back(timesGenerateNeighorhoodData);

	StoreTrackData();

}

;
