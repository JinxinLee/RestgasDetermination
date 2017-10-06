/*
 * PndSttSkewStrawPzFinderTask.cxx
 *
 *  Created on: Feb 8, 2016
 *      Author: walan603
 */

#include <PndSttSkewStrawPzFinderTask.h>
#include <PndSttSkewStrawPzFinderData.h>

#include <FairEventHeader.h>
#include <FairRootManager.h>
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"
#include "FairMultiLinkedData.h"

#include "PndSttMapCreator.h"
#include "PndGeoSttPar.h"
#include "PndSttTube.h"
#include <PndRiemannTrack.h>

#include "TMultiGraph.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TH2D.h"

#include <iostream>


ClassImp (PndSttSkewStrawPzFinderTask);

using namespace std;

PndSttSkewStrawPzFinderTask::PndSttSkewStrawPzFinderTask() : fStoredata(false){
	FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

PndSttSkewStrawPzFinderTask::~PndSttSkewStrawPzFinderTask() {
}

//For sorting skew straws according to layer
struct sort_pred {
    bool operator()(const std::pair<PndSttHit,int> &left,
    		const std::pair<PndSttHit,int> &right) {
        return left.second < right.second;
    }
};

InitStatus PndSttSkewStrawPzFinderTask::Init() {
	FairRootManager *ioman = FairRootManager::Instance();

	FairField* Field = FairRunAna::Instance()->GetField();
	Double_t po[3], BB[3];
	po[0]=0.;
	po[1]=0.;
	po[2]=0.;
	Field->GetFieldValue(po,BB);

	if (!ioman) {
		cout << "-E- PndSttSkewStrawPzFinderTask::Init: RootManager not instantiated!"
				<< endl;
		return kFATAL;
	}

	fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
	if (!fEventHeader) {
		cout << "-W-  PndSttSkewStrawPzFinderTask::Init: No EventHeader array! Needed for EventNumber"
				<< endl;
		return kERROR;
	}

	fFirstTrackCand = (TClonesArray*) ioman->GetObject("FirstTrackCand");
	if (!fFirstTrackCand) {
		cout << "-W-  PndSttSkewStrawPzFinderTask::Init: No FirstTrackCand array!"
				<< endl;
		return kERROR;
	}

	fFirstTrack = (TClonesArray*) ioman->GetObject("FirstTrack");
	if (!fFirstTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderTask::Init: No FirstTrack array!"
				<< endl;
		return kERROR;
	}

	fFirstRiemannTrack = (TClonesArray*) ioman->GetObject("FirstRiemannTrack");
	if (!fFirstRiemannTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderTask::Init: No FirstRiemannTrack array!"
				<< endl;
		return kERROR;
	}

	fCombiTrackCand = (TClonesArray*) ioman->GetObject("CombiTrackCand");
	if (!fCombiTrackCand) {
		cout << "-W-  PndSttSkewStrawPzFinderTask::Init: No CombiTrackCand array!"
				<< endl;
		return kERROR;
	}

	fCombiTrack = (TClonesArray*) ioman->GetObject("CombiTrack");
	if (!fCombiTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderTask::Init: No CombiTrack array!"
				<< endl;
		return kERROR;
	}

	fCombiRiemannTrack = (TClonesArray*) ioman->GetObject("CombiRiemannTrack");
	if (!fCombiRiemannTrack) {
		cout << "-W-  PndSttSkewStrawPzFinderTask::Init: No CombiRiemannTrack array!"
				<< endl;
		return kERROR;
	}

	fSTTHits = (TClonesArray*) ioman->GetObject("STTHit");
	if ( ! fSTTHits ) {
		cout << "-W- PndSttSkewStrawPzFinderTask::Init: fSTTHits not instantiated,"
				" return!"
				<< endl;
		return kERROR;
	}

	fFinalTrackCand = ioman->Register("FinalTrackCand", "PndTrackCand", "STT",
			kTRUE);

	fFinalTrack = ioman->Register("FinalTrack", "PndTrack", "STT", kTRUE);

	fFinalRiemannTrack = ioman->Register("FinalRiemannTrack", "PndRiemannTrack",
			"STT", kTRUE);

	if(fStoredata) {
		fFinalPzData = ioman->Register("FinalPzData", "PndSttSkewStrawPzFinderData",
				"STT", kTRUE);
	}

	//Get tube array
	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
	fTubeArray = mapper->FillTubeArray();

	//Instantiate PzFinder
	PzFinder = new PndSttSkewStrawPzFinder(fTubeArray,fSTTHits);

	return kSUCCESS;
}

void PndSttSkewStrawPzFinderTask::Exec(Option_t* opt) {
	FairRootManager* ioman = FairRootManager::Instance();

	PzFinder->Reset();

	FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader;
	Int_t eventNumber = myEventHeader->GetMCEntryNumber();

	cout<<"---"<<endl;
	cout<<"Exec event: "<<eventNumber<<endl;
	cout<<"---"<<endl;

	/* Filling std vectors with PndSttHits of each track, the PndTrack and PndRiemannTrack
	 * for each corresponding track candidate produced by the SttCellTrackFinder.
	 */

	vector<vector<PndSttHit> > AllSkewedHits;
	vector<PndTrackCand> AllTrackCands;
	vector<PndRiemannTrack> AllRiemannTracks;
	vector<PndTrack> AllTracks;

	for (int i = 0; i < fCombiTrack->GetEntriesFast(); ++i) {
		AllTracks.push_back(*(PndTrack*) (fCombiTrack->At(i)));
	}

	for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {
		AllTrackCands.push_back(*(PndTrackCand*) (fCombiTrackCand->At(i)));
	}

	for (int i = 0; i < fCombiRiemannTrack->GetEntriesFast(); ++i) {
		AllRiemannTracks.push_back(*(PndRiemannTrack*) (fCombiRiemannTrack->At(i)) );
	}

/*
	PndTrackCand myTrackCand;
	vector<PndTrackCandHit> tracklist;
	PndSttHit hit;
	PndSttTube myTube;
	for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {
		myTrackCand = *(PndTrackCand*) fCombiTrackCand->At(i);

		tracklist = myTrackCand.GetSortedHits();
		vector<pair<PndSttHit,int> > hitPairs;
		for (int j = 0; j < tracklist.size(); ++j)
		{
			hit = *(PndSttHit*) fSTTHits->At(tracklist.at(j).GetHitId());
			myTube = *(PndSttTube*) fTubeArray->At(hit.GetTubeID());

			if (myTube.IsSkew()) {
				hitPairs.push_back(pair<PndSttHit,int> (hit,hit.GetTubeID()));
			}

			//Sorting the skewed hits based on their layer ID
			std::sort(hitPairs.begin(), hitPairs.end(), sort_pred());
		}

		vector<PndSttHit> skewHits;
		for (int j = 0; j < hitPairs.size(); j++){
			skewHits.push_back(hitPairs.at(j).first);
		}

		AllSkewedHits.push_back(skewHits);
	}
*/

/*
 * Add all PndTracks in one event. Add corresponding Riemann track for each track.
 * Add also list of skewed Stt hits for each track.
 */
	PzFinder->AddPndTracks(AllTracks);
	PzFinder->AddPndTrackCands(AllTrackCands);
	PzFinder->AddPndRiemannTracks(AllRiemannTracks);
	PzFinder->ExtractPz();
	//PzFinder->AddPndSttHits(AllSkewedHits);

	/*
	 * Get all tracks where z component was successfully found
	 */

	for (int i = 0; i < PzFinder->NumPndTrackCands(); ++i) {
		PndTrackCand* myCand = new ((*fFinalTrackCand)[i]) PndTrackCand(
				PzFinder->GetTrackCand(i));

		PndTrack* myTrack = new ((*fFinalTrack)[i]) PndTrack(
				PzFinder->GetTrack(i));
		myTrack->SetTrackCandRef(myCand);
		myTrack->SetTrackCand(*myCand);
		myTrack->SetLinks(*myCand->GetPointerToLinks());

	}

	for (int i = 0; i < PzFinder->NumPndRiemannTracks(); ++i) {
		PndRiemannTrack* myRiemannTrack =
				new ((*fFinalRiemannTrack)[i]) PndRiemannTrack(
						PzFinder->GetRiemannTrack(i));
	}

	if (fStoredata) {
		for (int i = 0; i < PzFinder->NumPndPzData(); ++i) {
			PndSttSkewStrawPzFinderData* myPzData =
					new ((*fFinalPzData)[i]) PndSttSkewStrawPzFinderData(
							PzFinder->GetPzData(i));
		}
	}

}

void PndSttSkewStrawPzFinderTask::FinishEvent() {
	fFirstTrackCand->Delete();
	fFirstTrack->Delete();
	fFirstRiemannTrack->Delete();

	fCombiTrackCand->Delete();
	fCombiTrack->Delete();
	fCombiRiemannTrack->Delete();

	fVectorPndTrackCand.clear();
	fVectorPndTrack.clear();

	fFinalTrackCand->Delete();
	fFinalTrack->Delete();
	fFinalRiemannTrack->Delete();
	if (fStoredata) fFinalPzData->Delete();

}
