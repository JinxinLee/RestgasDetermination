#include "PndSttCellTrackFinderTask.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

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
#include "FairEventHeader.h"

// PndMvd includes
#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndSttMapCreator.h"
#include "PndSttCellTrackFinder.h"

using std::cout;
using std::endl;

//Macro for printing the calculation times into files called calcTimesCPU*.txt
//#define PRINT_CALC_TIMES

ClassImp(PndSttCellTrackFinderTask);

void PndSttCellTrackFinderTask::SetParContainers() {
	FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndSttCellTrackFinderTask::Init() {
	FairRootManager* ioman = FairRootManager::Instance();

	if (!ioman) {
		std::cout << "-E- PndSttCellTrackFinderTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
	if (!fEventHeader) {
		cout
				<< "-W-  PndSttCellTrackFinderTask::Init: No EventHeader array! Needed for EventNumber"
				<< endl;
		return kERROR;
	}

	if (fHitBranch.size() == 0) {
		std::cout << "-W- PndSttCellTrackFinderTask::Init: "
				<< "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!"
				<< std::endl;
		fHitBranch.push_back("STTHit");
		fHitBranch.push_back("STTCombinedSkewedHits");
	}

	for (int i = 0; i < (int) fHitBranch.size(); i++) {
		InitHitArray(fHitBranch[i]);
	}

	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);

	fTubeArray = mapper->FillTubeArray();
	fTrackFinder = new PndSttCellTrackFinder(fTubeArray);

	fTrackFinder->SetCalcFirstTrackletInf(fAnalyseSteps);
	fTrackFinder->SetUseGPU(fUseGPU);
	fTrackFinder->SetVerbose(fVerbose);
	fTrackFinder->SetCalcWithCorrectedIsochrones(fCalcWithCorrectedIsochrones);

	if (fUseGPU) {
		//Copy static data to device

		//Macros were defined in PndSttCellTrackletGenerator.h
		int numElements = NUM_SKEWED_STRAWS * MAX_SKEWED_NEIGHBORS
				+ NUM_UNSKEWED_STRAWS * MAX_UNSKEWED_NEIGHBORS;
		//allocate cpu memory, initialize with 0 (important for gpu algorithm)
		int *tubeNeighborings = (int*) calloc(numElements, sizeof(int));

		PndSttGeometryMap* tmpGeometryMap =
				fTrackFinder->GetTrackFinderDataObject()->GetGeometryMap();

		PndSttTube* tube;
		TVector3 pos;
		TArrayI neighbors;
		int tubeID;
		int skewedOffset = NUM_UNSKEWED_STRAWS * MAX_UNSKEWED_NEIGHBORS;

		//fill array with neighborhood data
		for (int i = 1; i < fTubeArray->GetEntriesFast(); ++i) {
			tube = (PndSttTube*) fTubeArray->At(i);
			tubeID = tube->GetTubeID();
			neighbors = tmpGeometryMap->GetNeighboringsByMap(tubeID);

			//store at first data of unskewed tubes in array, at the end of the array: skewed tubes neighborings
			if (tubeID >= START_TUBE_ID_SKEWED && tubeID <= END_TUBE_ID_SKEWED) {
				//inner unskewed tube
				for (int j = 0; j < neighbors.GetSize(); ++j) {
					tubeNeighborings[skewedOffset + j * NUM_SKEWED_STRAWS
							+ (tubeID - START_TUBE_ID_SKEWED)] = neighbors[j];

				}

			} else if (tubeID < START_TUBE_ID_SKEWED) {
				//middle skewed tube
				for (int j = 0; j < neighbors.GetSize(); ++j) {
					tubeNeighborings[j * NUM_UNSKEWED_STRAWS + (tubeID - 1)] =
							neighbors[j];
				}
			} else if (tubeID > END_TUBE_ID_SKEWED) {
				//outer unskewed tube
				for (int j = 0; j < neighbors.GetSize(); ++j) {
					tubeNeighborings[j * NUM_UNSKEWED_STRAWS
							+ (tubeID
									- (END_TUBE_ID_SKEWED - START_TUBE_ID_SKEWED
											+ 1) - 1)] = neighbors[j];
				}
			}
		}

		fDev_tubeNeighborings = AllocateStaticData(tubeNeighborings,
				numElements);
		fTrackFinder->SetDevTubeNeighboringsPointer(fDev_tubeNeighborings);
		free(tubeNeighborings);

	}

	if (fAnalyseSteps) {
		//store information of primary tracklets
		fFirstTrackCandArray = ioman->Register("FirstTrackCand", "PndTrackCand",
				"STT", fPersistence);
		fFirstTrackArray = ioman->Register("FirstTrack", "PndTrack", "STT",
				fPersistence);
		fFirstRiemannTrackArray = ioman->Register("FirstRiemannTrack",
				"PndRiemannTrack", "STT", fPersistence);

	}

	fCombiTrackCandArray = ioman->Register("CombiTrackCand", "PndTrackCand",
			"STT", fPersistence);
	fCombiTrackArray = ioman->Register("CombiTrack", "PndTrack", "STT",
			fPersistence);
	fCombiRiemannTrackArray = ioman->Register("CombiRiemannTrack",
			"PndRiemannTrack", "STT", fPersistence);

	if (fCalcWithCorrectedIsochrones) {

		fCorrectedIsochronesArray = ioman->Register("CorrectedIsochrones",
				"FairHit", "STT", fPersistence);
	}

	std::cout << "-I- PndSttCellTrackFinderTask: Initialisation successfull"
			<< std::endl;

	//fInitDone = kTRUE;
	return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndSttCellTrackFinderTask::Exec(Option_t* opt) {

	FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader;
	int eventNumber = myEventHeader->GetMCEntryNumber();

	if (fVerbose > 0) {
		cout
				<< "====================Begin PndSttCellTrackFinderTask::Exec======================="
				<< endl;

	}
	//cout << "Event #" << eventNumber << endl;

// Reset output array
	if (!fFirstTrackCandArray)
		Fatal("Exec", "No trackCandArray");

	fTrackFinder->Reset();

	FairRootManager *ioman = FairRootManager::Instance();

	for (int i = 0; i < (int) fHitBranch.size(); i++) {
		fTrackFinder->AddHits(fHitArray[i], ioman->GetBranchId(fHitBranch[i]));
	}

	fTrackFinder->FindTracks();

	if (fAnalyseSteps) {
		for (int i = 0; i < fTrackFinder->NumFirstTrackCands(); i++) {
			PndTrackCand* myCand =
					new ((*fFirstTrackCandArray)[i]) PndTrackCand(
							fTrackFinder->GetFirstTrackCand(i));
		}

		for (int i = 0; i < fTrackFinder->NumFirstRiemannTracks(); ++i) {
			PndRiemannTrack* myTrack =
					new ((*fFirstRiemannTrackArray)[i]) PndRiemannTrack(
							fTrackFinder->GetFirstRiemannTrack(i));
		}
	}

	for (int i = 0; i < fTrackFinder->NumCombinedTracks(); ++i) {
		PndTrackCand* myCand = new ((*fCombiTrackCandArray)[i]) PndTrackCand(
				fTrackFinder->GetCombiTrackCand(i));
		PndTrack* myTrack = new ((*fCombiTrackArray)[i]) PndTrack(
				fTrackFinder->GetCombiTrack(i));
		myTrack->SetTrackCandRef(myCand);
		myTrack->SetTrackCand(*myCand);

	}

	for (int i = 0; i < fTrackFinder->NumCombinedRiemannTracks(); ++i) {
		PndRiemannTrack* myRiemannTrack =
				new ((*fCombiRiemannTrackArray)[i]) PndRiemannTrack(
						fTrackFinder->GetCombiRiemannTrack(i));
	}

	if (fCalcWithCorrectedIsochrones) {
		std::map<int, FairHit*> correctedIsochrones =
				fTrackFinder->GetCorrectedIsochrones();

		int indexCounter = 0;
		for (std::map<int, FairHit*>::iterator iter =
				correctedIsochrones.begin(); iter != correctedIsochrones.end();
				iter++) {
			FairHit* myCorrectedHit =
					new (
							(*fCorrectedIsochronesArray)[fCorrectedIsochronesArray->GetEntries()]) FairHit(
							*iter->second);
		}
	}

	if (fVerbose > 0)
		cout << "#FirstTracklets: " << fTrackFinder->GetNumPrimaryTracklets()
				<< ", #CombinedTracklets: " << fTrackFinder->NumCombinedTracks()
				<< endl;

	fCombiTrackCandArray->Sort();
	fCombiTrackArray->Sort();

}

void PndSttCellTrackFinderTask::FinishEvent() {
	if (fAnalyseSteps) {
		fFirstTrackCandArray->Delete();
		fFirstRiemannTrackArray->Delete();
	}
	fCombiTrackCandArray->Delete();
	fCombiTrackArray->Delete();
	fCombiRiemannTrackArray->Delete();
	if (fCalcWithCorrectedIsochrones)
		fCorrectedIsochronesArray->Delete();

	vector<int> numHits;
	numHits.push_back(fTrackFinder->NumHits());
	numHits.push_back(fTrackFinder->NumHitsWithoutDouble());
	numHits.push_back(fTrackFinder->NumUnambiguousNeighbors());
	fNumHitsPerEvent.push_back(numHits);

}

void PndSttCellTrackFinderTask::FinishTask() {

	if (fUseGPU) {
		FreeStaticData(fDev_tubeNeighborings);
	}

#ifdef PRINT_CALC_TIMES
	// write calculation times, calcTimesTrackletGen.size()=#Events, calcTimesTrackletGen[i].size()=#measured times for Event #i
	vector<vector<Double_t> > calcTimesTrackletGen =
	fTrackFinder->GetTimeStampsTrackletGen();
	vector<vector<Double_t> > calcTimesNeighborhood = fTrackFinder->GetTimeStampsGenerateNeighborhoodData();

	FILE* fp = fopen("calcTimesCPU.txt", "w");
	fprintf(fp,
			"%3s \t %3s \t %3s \t %3s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \n",
			"#E", "#H", "#HWD", "#UHWD", "EvaluateState()", "InitStartTracklets()",
			"EvaluateMultiState()", "GenerateTracklets()",
			"CombineTrackletsMultiStages()", "AssignAmbiguousHits()",
			"SplitData()", "AddMissingHits()", "CreatePndTrackCands()",
			"FindTracks()", "CalcNeighborData");
	for (int i = 0; i < calcTimesTrackletGen.size(); ++i) {
		fprintf(fp, "%3i \t %3i \t %3i \t %3i", i, fNumHitsPerEvent.at(i).at(0), fNumHitsPerEvent.at(i).at(1), fNumHitsPerEvent.at(i).at(2));
		//trackletGen data
		for (int j = 0; j < calcTimesTrackletGen[i].size(); j += 2) {
			fprintf(fp, "\t %.15f",
					(calcTimesTrackletGen[i].at(j + 1) - calcTimesTrackletGen[i].at(j)) * 1000);
		}
		//neighborhood data
		fprintf(fp, "\t %.15f",
				(calcTimesNeighborhood[i].at(1) - calcTimesNeighborhood[i].at(0)) * 1000);

		fprintf(fp, "\n");
	}

	fclose(fp);
#endif

}

void PndSttCellTrackFinderTask::InitHitArray(TString branchName) {
	TClonesArray* tempArray =
			(TClonesArray*) FairRootManager::Instance()->GetObject(branchName);
	if (tempArray == 0) {
		std::cout << "-W- PndSttCellTrackFinderTask::Init: "
				<< "No hitArray for BranchName " << branchName.Data()
				<< std::endl;
	}
	fHitArray.push_back(tempArray);
}

