#include "PndSttCellTrackFinder.h"
#include "PndSttHit.h"
#include "TString.h"
#include "FairHit.h"

#include "PndSttStrawMap.h"
#include "PndSttGeometryMap.h"
#include "PndSttTube.h"
#include "PndRiemannHit.h"
#include "FairEventHeader.h"

#include <iostream>
#include <math.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

ClassImp(PndSttCellTrackFinder)
;

void PndSttCellTrackFinder::SetSttTubeArray(TClonesArray* sttTubeArray) {
	if (fGeometryMap == 0) {
		fStrawMap.GenerateStrawMap(sttTubeArray);
		fGeometryMap = new PndSttGeometryMap(sttTubeArray, 1);
	}

	PndSttTube* tube;
	TVector3 pos;

	for (int i = 1; i < sttTubeArray->GetEntriesFast(); ++i) {
		tube = (PndSttTube*) sttTubeArray->At(i);
		pos = tube->GetPosition();
		fMapTubeIdToPos[i] = pos;
	}

}

void PndSttCellTrackFinder::AddHits(TClonesArray* hits, Int_t branchId) {
	TString geoPath;
	FairHit* myHit;
	FairLink myID;

	for (int i = 0; i < hits->GetEntries(); i++) {
		myHit = (FairHit*) (hits->At(i));

		if (myHit->GetEntryNr().GetIndex() < 0) {
			myID = FairLink(branchId, i);
			myHit->SetEntryNr(FairLink(branchId, i));
		} else
			myID = myHit->GetEntryNr();

		//fMapHitToFairLink[fHits.size() - 1] = myID;
		fMapHitToFairLink[i] = myID;
		fHits.push_back(myHit);

	}

}

void PndSttCellTrackFinder::FindTracks() {

	if (fVerbose > 0) {
		cout << "PndSttCellTrackFinder::FindTracks()" << endl;
	}

	GenerateTracklets();

	CombineTracklets();

	CreateFurtherCombinations();

	if (fVerbose > 1) {

		cout << "#Combination of Tracklets: " << fCombinedData.size() << endl;
		for (int i = 0; i < fCombinedData.size(); ++i) {
			cout << "Combination #" << i << endl;
			cout << "state of tracklets: ";
			for (int j = 0;j < fCombinedData[i].tracklets.size(); ++j) {
				cout << fCombinedData[i].tracklets[j] << ", ";
			}
			cout << " radius: " << fCombinedData[i].trackletInf.riemannTrack.r()
					<< ", center: "
					<< fCombinedData[i].trackletInf.riemannTrack.orig()[0]
					<< ", "
					<< fCombinedData[i].trackletInf.riemannTrack.orig()[1]
					<< " , error: " << fCombinedData[i].trackletInf.error
					<< ", #wrong hits: "
					<< fCombinedData[i].trackletInf.numErrHits << endl;

			if (fStartTracklets[fCombinedData[i].tracklets[0]].riemannTrack.getNumHits()
					!= 0) {
				cout << "riemannTrack before:" << " , radius: "
						<< fStartTracklets[fCombinedData[i].tracklets[0]].riemannTrack.r()
						<< ", center: "
						<< fStartTracklets[fCombinedData[i].tracklets[0]].riemannTrack.orig()[0]
						<< ", "
						<< fStartTracklets[fCombinedData[i].tracklets[0]].riemannTrack.orig()[1]
						<< " , error: "
						<< fStartTracklets[fCombinedData[i].tracklets[0]].error
						<< ", #wrong hits: "
						<< fStartTracklets[fCombinedData[i].tracklets[0]].numErrHits
						<< endl;

			}
		}
	}

	FindTrackletsWithoutCombi();

	AddMissingHits();

	CreatePndTrackCands();

}

void PndSttCellTrackFinder::CreatePndTrackCands() {

	int state, numHits;

	// for each combination
	for (int i = 0; i < fCombinedData.size(); ++i) {
		PndTrackCand trackCand;

		// add all hits of combined tracklet to trackCand
		for (int j = 0; j < fCombinedData[i].trackletInf.hitIDs.size(); ++j) {
			trackCand.AddHit(
					fMapHitToFairLink[fCombinedData[i].trackletInf.hitIDs[j]],
					0);
		}

		fCombiTrackCand.push_back(trackCand);
	}

	// add tracklets without combi and more than 2 hits
	for (int i = 0; i < fTrackletsWithoutCombi.size(); ++i) {
		PndTrackCand trackCand;
		numHits = fStartTracklets[fTrackletsWithoutCombi[i]].hitIDs.size();

		if (numHits > 2) {
			for (int j = 0; j < numHits; ++j) {
				trackCand.AddHit(
						fMapHitToFairLink[fStartTracklets[fTrackletsWithoutCombi[i]].hitIDs[j]],
						0);
			}
			fCombiTrackCand.push_back(trackCand);
		}
	}
}

void PndSttCellTrackFinder::GenerateTracklets() {

	//cout << "================= Generate Tracklets ===============" << endl;

	// init states of cells with the id of the tube
	PndSttHit* sttHit;
	for (int i = 0; i < fHits.size(); ++i) {
		sttHit = (PndSttHit*) fHits[i];
		fStates[sttHit->GetTubeID()] = sttHit->GetTubeID();
		fMapTubeIdToHit[sttHit->GetTubeID()] = i;
	}

	FindHitNeighbors();
	SeparateNeighbors();
	EvaluateState();
	InitStartTracklets();

	if (fVerbose > 1) {

		cout << "Start-Tracklets#:" << fStartTracklets.size() << endl;

		cout << "Tracklet-Information: " << endl;
		map<int, TrackletInf_t>::iterator it;
		for (it = fStartTracklets.begin(); it != fStartTracklets.end(); ++it) {
			int state = it->first;
			TrackletInf_t inf = it->second;

			cout << "State: " << state << ", maxId: " << inf.maxID
					<< ", endId: " << inf.endID << ", straight: "
					<< inf.straight << ", numSkewed: " << inf.numSkewed;

			if (inf.riemannTrack.getNumHits() != 0) {
				cout << ", RiemannTrack created error: " << inf.error
						<< ", #wrong hits: " << inf.numErrHits;
			}

			cout << endl;
		}
	}

}

void PndSttCellTrackFinder::FindHitNeighbors() {

	/* Approach: At first create a set of the tubeIDs of all hits.
	 * Then get the neighbors of each hit/tube and store only those
	 * that are included in the set.*/

	PndSttHit* sttHit;
	int tubeId;
	set<int> hitIds;

	// init set with straw-ids of hits
	for (int i = 0; i < fHits.size(); ++i) {
		sttHit = (PndSttHit*) fHits[i];
		hitIds.insert(sttHit->GetTubeID());
	}

	// fill fHitNeighbors
	for (int i = 0; i < fHits.size(); ++i) {
		sttHit = (PndSttHit*) fHits[i];
		tubeId = sttHit->GetTubeID();
		//get neighbors
//		TArrayI tmp = fGeometryMap->FindNeighborings(tubeId);
		TArrayI tmp = fGeometryMap->GetNeighboringsByMap(tubeId);

		for (int j = 0; j < tmp.GetSize(); ++j) {

			// set contains neighbor?
			if (hitIds.find(tmp[j]) != hitIds.end()) {
				fHitNeighbors[tubeId].push_back(tmp[j]);

			}

		}
	}
}

void PndSttCellTrackFinder::SeparateNeighbors() {

	/* Separate the hits/active cells concerning the number of neighbors that made a signal, too.*/

	map<int, vector<int> >::iterator it;

	for (int i = 0; i < 8; ++i) {
		fSeparations[i].clear();
	}

	for (it = fHitNeighbors.begin(); it != fHitNeighbors.end(); ++it) {

		if (it->second.size() > 6) {
			// store cells with more than 6 neighbors in the same entry
			fSeparations[7].push_back(it->first);
		} else {
			// separation concerning 0-6 hit-neighbors
			fSeparations[it->second.size()].push_back(it->first);
		}
	}
}

void PndSttCellTrackFinder::EvaluateState() {

	/* Approach: The state has to be calculated for cells with 1 or 2 hit-neighbors (no ambiguities).
	 * Each cell starts with an unique state-ID (tubeID). At each step the cells analyzes similar cells
	 * (1/2 hit-neighbors) and calculate the minimum of all states. The states will be changed to
	 * this minimum simultaneously after the calculation. If there are no changes at consecutive steps,
	 * the evaluation of the states is finished. Cells with the same state-ID belongs to the same track
	 * and form a tracklet.*/

	int currentSum = 0;
	int priorSum = -1;
	int newState;
	set<int> plainHitIds;
	vector<int>::iterator it;
	map<int, int>::iterator itStates;
	vector<int> neighbors;
	map<int, int> tmpStates;

	// set with tube-ids of hits without ambiguity
	plainHitIds.insert(fSeparations[1].begin(), fSeparations[1].end());
	plainHitIds.insert(fSeparations[2].begin(), fSeparations[2].end());

	// calculate for tubes with only one or two hits in neighborings until the sum of states does not change anymore
	while (currentSum != priorSum) {

		priorSum = currentSum;
		currentSum = 0;

		// calculate new state of tubes with one hit in neighboring
		for (it = fSeparations[1].begin(); it < fSeparations[1].end(); ++it) {
			neighbors = fHitNeighbors[(*it)];

			// include only state of hits without ambiguity
			if (plainHitIds.find(neighbors[0]) != plainHitIds.end()) {
				newState = min(fStates[(*it)], fStates[neighbors[0]]);
				tmpStates[(*it)] = newState;
				currentSum += newState;
			} else {
				// no hit neighboring to consider
				tmpStates[(*it)] = fStates[(*it)];
				currentSum += newState;
			}

		}
		// calculate new state of tubes with two hits in neighboring
		for (it = fSeparations[2].begin(); it < fSeparations[2].end(); ++it) {
			neighbors = fHitNeighbors[(*it)];

			// include only state of hits without ambiguity
			if (plainHitIds.find(neighbors[1]) == plainHitIds.end())
				neighbors.erase(neighbors.begin() + 1);
			if (plainHitIds.find(neighbors[0]) == plainHitIds.end())
				neighbors.erase(neighbors.begin());

			// calculate minimum
			switch (neighbors.size()) {
			case 2:
				newState = min(fStates[(*it)],
						min(fStates[neighbors[0]], fStates[neighbors[1]]));
				break;
			case 1:
				newState = min(fStates[(*it)], fStates[neighbors[0]]);
				break;
			case 0:
				newState = fStates[(*it)];
			}

			tmpStates[(*it)] = newState;
			currentSum += newState;
		}

		// update states simultaneously
		fStates.clear();
		fStates.insert(tmpStates.begin(), tmpStates.end());

	}
}

void PndSttCellTrackFinder::InitStartTracklets() {

	vector<int>::iterator it;

	// get states + hitIDs of tracklets with 1 or 2 hit-neighbors
	for (it = fSeparations[1].begin(); it < fSeparations[1].end(); ++it) {
		fStartTracklets[fStates[(*it)]].hitIDs.push_back(
				fMapTubeIdToHit[(*it)]);

	}

	for (it = fSeparations[2].begin(); it < fSeparations[2].end(); ++it) {
		fStartTracklets[fStates[(*it)]].hitIDs.push_back(
				fMapTubeIdToHit[(*it)]);
	}

	vector<int> hitIDs;
	int state;
	PndSttHit* sttHit;
	int tubeID;

	map<int, TrackletInf_t>::iterator trackIt;
	for (trackIt = fStartTracklets.begin(); trackIt != fStartTracklets.end();
			++trackIt) {

		state = trackIt->first;
		TrackletInf_t trackletInf = trackIt->second;
		hitIDs = trackletInf.hitIDs;
		PndTrackCand trackCand;
		trackletInf.maxID = 0;
		trackletInf.endID = state;
		trackletInf.straight = false;
		trackletInf.numSkewed = 0;
		bool endID;

		for (int i = 0; i < hitIDs.size(); ++i) {

			sttHit = (PndSttHit*) fHits[hitIDs[i]];
			tubeID = sttHit->GetTubeID();

			if (fStrawMap.IsSkewedStraw(tubeID)) {
				++trackletInf.numSkewed;
			}

			endID = IsEndTubeOfTracklet(tubeID);

			// found tubeID that's bigger than maxID of tracklet?
			if (tubeID > trackletInf.maxID) {

				// if tube is end of tracklet --> straight tracklet
				if (fStrawMap.GetRow(tubeID)
						== fStrawMap.GetRow(trackletInf.maxID)) {
					// Hits in same row --> not a straight track
					trackletInf.straight = false;
				}
				if (endID) {
					// no hits in same row and hit in end-tube
					trackletInf.straight = true;
				} else {
					// hit was not in end-tube
					trackletInf.straight = false;
				}

				trackletInf.maxID = tubeID;
			}

			// safe ID of end-tube
			if (endID && tubeID != state) {
				trackletInf.endID = tubeID;
			}

			if (fCalcFirstTrackletInf) {
				// add hits to TrackCand
				trackCand.AddHit(fMapHitToFairLink[hitIDs[i]], 0);
			}
		}

		if (fCalcFirstTrackletInf) {
			// calc riemannTrack
			PndRiemannTrack riemannTrack = CreateRiemannTrack(hitIDs);

			// add riemannTrack to trackletInf if possible
			if (riemannTrack.getNumHits() > 2) {
				riemannTrack.refit(false);
				trackletInf.riemannTrack = riemannTrack;
				trackletInf.error = CalcDeviationOfRiemannTrack(riemannTrack);
				trackletInf.numErrHits = GetDeviationCount(riemannTrack);
				fFirstRiemannTrack.push_back(riemannTrack);

			}
			fFirstTrackCand.push_back(trackCand);
		}

		trackIt->second = trackletInf;

	}
}

void PndSttCellTrackFinder::CombineTracklets() {

	vector<int> leftTrackCands; // State of tracklets on the left side of stt
	vector<int> rightTrackCands; // State of tracklets on the right side of stt

	map<int, TrackletInf_t>::iterator trackletsIt;

	int tubeId;
	vector<int> hits;

	// devide tracklets into tracklets of left and right half of the stt to get less combination possibilities
	for (trackletsIt = fStartTracklets.begin();
			trackletsIt != fStartTracklets.end(); ++trackletsIt) {

		// state is equal to tubeID of on hit in tracklet
		tubeId = trackletsIt->first;
		hits = trackletsIt->second.hitIDs;

		// add only tracklets with more than two hits of not skewed tubes
		if ((hits.size() - trackletsIt->second.numSkewed) > 2) {

			// check the sector of the tube
			if (fStrawMap.GetSector(tubeId) <= 2) {
				// tracklet is located in the left half of stt
				leftTrackCands.push_back(tubeId);
			} else {
				// tracklet is located in the right half of stt
				rightTrackCands.push_back(tubeId);
			}

		}

	}

	// sort tracklets by state
	sort(leftTrackCands.begin(), leftTrackCands.end());
	sort(rightTrackCands.begin(), rightTrackCands.end());

	int firstState, secondState, sector;
	vector<int> tracklets;
	set<int> sectorNeighbors;

	// combine the leftTrackCands
	for (int i = 0; i < leftTrackCands.size(); ++i) {

		firstState = leftTrackCands[i];

		// combine straight and "unfinished" tracklets only
		if (fStartTracklets[firstState].straight
				&& fHitNeighbors[fStartTracklets[firstState].maxID].size()
						> 1) {

			sector = fStrawMap.GetSector(fStartTracklets[firstState].maxID);
			sectorNeighbors.clear();
			sectorNeighbors.insert(sector);
			sectorNeighbors.insert(fStrawMap.GetLeftSector(sector));
			sectorNeighbors.insert(fStrawMap.GetRightSector(sector));

			for (int j = i + 1; j < leftTrackCands.size(); ++j) {
				secondState = leftTrackCands[j];

				// combine with tracklets with a higher state than maxID of first tracklet
				if (secondState > fStartTracklets[firstState].maxID) {

					// is second tracklet in nearby sector?
					if (sectorNeighbors.find(fStrawMap.GetSector(secondState))
							!= sectorNeighbors.end()) {

						tracklets.clear();
						tracklets.push_back(firstState);
						tracklets.push_back(secondState);

						TrackletInf_t trackletInf = GetTrackletInf(tracklets);

						/* it is not necessary to check riemannTrack because only tracklet with
						 * more than 2 hits of unskewed tubes were combined.*/
						if (trackletInf.numErrHits == 0) {
							Combination_t combi;
							combi.tracklets = tracklets;
							combi.trackletInf = trackletInf;

							fCombinedData.push_back(combi);
						}

						/*	old code
						 *
						 * // get hit-ids of both tracklets
						 hitIDs = fStartTracklets[firstState].hitIDs;
						 hitIDs.insert(hitIDs.end(),
						 fStartTracklets[secondState].hitIDs.begin(),
						 fStartTracklets[secondState].hitIDs.end());

						 PndRiemannTrack riemannTrack = CreateRiemannTrack(hitIDs);

						 // at least 5 riemannHits? --> fit riemannTrack and save in map
						 if (riemannTrack.getNumHits() > 4) {
						 riemannTrack.refit(false);

						 numWrongHits = GetDeviationCount(riemannTrack);

						 if (numWrongHits < 2) {
						 Combination combi;

						 vector<int> tracklets;
						 tracklets.push_back(firstState);
						 tracklets.push_back(secondState);

						 error = CalcDeviationOfRiemannTrack(riemannTrack);

						 combi.riemannTrack = riemannTrack;
						 combi.tracklets = tracklets;
						 combi.errSum = error;
						 combi.numErrHits = numWrongHits;

						 fCombinedData.push_back(combi);
						 }*/

					}

				}

			}
		}
	}

	// combine the rightTrackCands
	for (int i = 0; i < rightTrackCands.size(); ++i) {
		firstState = rightTrackCands[i];

		// combine straight and "unfinished" tracklets only
		if (fStartTracklets[firstState].straight
				&& fHitNeighbors[fStartTracklets[firstState].maxID].size()
						> 1) {

			sector = fStrawMap.GetSector(fStartTracklets[firstState].maxID);
			sectorNeighbors.clear();
			sectorNeighbors.insert(sector);
			sectorNeighbors.insert(fStrawMap.GetLeftSector(sector));
			sectorNeighbors.insert(fStrawMap.GetRightSector(sector));

			for (int j = i + 1; j < rightTrackCands.size(); ++j) {
				secondState = rightTrackCands[j];

				// combine with tracklets with a higher state than maxID of first tracklet
				if (secondState > fStartTracklets[firstState].maxID) {

					// is second tracklet in nearby sector?
					if (sectorNeighbors.find(fStrawMap.GetSector(secondState))
							!= sectorNeighbors.end()) {

						tracklets.clear();
						tracklets.push_back(firstState);
						tracklets.push_back(secondState);

						TrackletInf_t trackletInf = GetTrackletInf(tracklets);

						/* it is not necessary to check riemannTrack because only tracklet with
						 * more than 2 hits of unskewed tubes were combined.*/
						if (trackletInf.numErrHits == 0) {
							Combination_t combi;
							combi.tracklets = tracklets;
							combi.trackletInf = trackletInf;

							fCombinedData.push_back(combi);
						}

					}
				}
			}
		}

	}

}

void PndSttCellTrackFinder::FindTrackletsWithoutCombi() {

	set<int> combinedTracklets;

	// fill set with states of combined tracklets
	for (int i = 0; i < fCombinedData.size(); ++i) {

		for (int j = 0; j < fCombinedData[i].tracklets.size(); ++j)
			combinedTracklets.insert(fCombinedData[i].tracklets[j]);
	}

	map<int, TrackletInf_t>::iterator it;
	for (it = fStartTracklets.begin(); it != fStartTracklets.end(); ++it) {

		if (combinedTracklets.find(it->first) == combinedTracklets.end()) {
			fTrackletsWithoutCombi.push_back(it->first);
		}
	}

}

void PndSttCellTrackFinder::CreateFurtherCombinations() {

	int curSize = fCombinedData.size();
	Combination_t curCombi, nextCombi;

	for (int i = 0; i < curSize; ++i) {

		curCombi = fCombinedData[i];

		//search for another combi that starts with the state of the second tracklet of the current combi
		for (int j = i + 1; j < curSize; ++j) {

			nextCombi = fCombinedData[j];

			// is there a chain?
			if (curCombi.tracklets[1] == nextCombi.tracklets[0]) {

				//combine 2 two-part-combinations to 1 three-part-combination
				Combination_t newCombi;
				newCombi.tracklets.push_back(curCombi.tracklets[0]);
				newCombi.tracklets.push_back(curCombi.tracklets[1]);
				newCombi.tracklets.push_back(nextCombi.tracklets[1]);
				newCombi.trackletInf = GetTrackletInf(newCombi.tracklets);

				if (newCombi.trackletInf.numErrHits == 0) {
					fCombinedData.push_back(newCombi);
					if(fVerbose>1){
						cout<< "Found Combination of 3 tracklets!"<<endl;
					}
				}
			}
		}
	}

}

void PndSttCellTrackFinder::AddMissingHits() {

	// add hits with ambiguities
	int curTubeID;
	vector<int> index;
	index.push_back(3);
	index.push_back(4);

	for (int i = 0; i < index.size(); ++i) {

		for (int k = 0; k < fSeparations[index[i]].size(); ++k) {
			curTubeID = fSeparations[index[i]][k];

			if (!fStrawMap.IsSkewedStraw(curTubeID)) {
				AddHitToBestCombi(fMapTubeIdToHit[curTubeID]);
			}
		}
	}

	// add tracklets with one or two hits that were not combined yet

	int state;
	PndSttHit* sttHit;
	for (int i = 0; i < fTrackletsWithoutCombi.size(); ++i) {
		state = fTrackletsWithoutCombi[i];
		if ((fStartTracklets[state].hitIDs.size()
				- fStartTracklets[state].numSkewed) < 3) {

			for (int j = 0; j < fStartTracklets[state].hitIDs.size(); ++j) {
				sttHit = (PndSttHit*) fHits[fStartTracklets[state].hitIDs[j]];
				if (!fStrawMap.IsSkewedStraw(sttHit->GetTubeID())) {
					AddHitToBestCombi(fStartTracklets[state].hitIDs[j]);

				}
			}

		}
	}
}

bool PndSttCellTrackFinder::AddHitToBestCombi(int hitID) {

	double minDistance = 100, tmpDistance;
	PndSttHit* sttHit = (PndSttHit*) fHits[hitID];
	int tubeID = sttHit->GetTubeID();
	int sector = fStrawMap.GetSector(tubeID);
	int sectorOfCombi, foundCombi;

	for (int i = 0; i < fCombinedData.size(); ++i) {

		sectorOfCombi = fStrawMap.GetSector(fCombinedData[i].tracklets[0]);

		if (sector / 3 == sectorOfCombi / 3) {
			// combi and hit are in the same half of the STT (integer division)
			tmpDistance=CalcDeviation(fCombinedData[i].trackletInf.riemannTrack,
							hitID);
			if(tmpDistance < minDistance){
				minDistance=tmpDistance;
				foundCombi=i;
			}
		}
	}

	// min distance smaller than radius of a tube?
	if (minDistance < 0.5005) {
		// add hit to combi
		fCombinedData[foundCombi].trackletInf.hitIDs.push_back(hitID);
		if(fVerbose>2){
			cout<<"AddHitToBestCombi(): add "<<tubeID<<" to Combi #"<<foundCombi<<", distance: "<<minDistance<<endl;
		}
		return true;
	} else {
		return false;
	}

}

// es wird davon ausgegangen, dass nur hintereinanderliegende tracklets kombiniert werden
TrackletInf_t PndSttCellTrackFinder::GetTrackletInf(vector<int> tracklets) {

	// sort by state of tracklets
	sort(tracklets.begin(), tracklets.end());

	TrackletInf_t inf;
	int numTracklets = tracklets.size();
	int state, maxID, endID;
	bool straight = false;

	if (numTracklets > 1) {
		// update trackletInf

		state = 5000;
		// state of combined tracklet equals min state of all
		for (int i = 0; i < numTracklets; ++i) {
			state = TMath::Min(state, tracklets[i]);
		}

		straight = true;
		// combined tracklet is straight if each particular tracklets are straight
		for (int i = 0; i < numTracklets; ++i) {
			if (fStartTracklets[tracklets[i]].straight) {
				straight = false;
				break;
			}
		}

		if (straight) {
			// maxID equals the data of the last tracklet
			maxID = fStartTracklets[tracklets[numTracklets - 1]].maxID;

		} else {
			// maxID is the maximum of all maxID
			maxID = 0;
			for (int i = 0; i < numTracklets; ++i) {
				maxID = TMath::Max(maxID, fStartTracklets[tracklets[i]].maxID);
			}
		}
		// endID equals the endID of last tracklet
		endID = fStartTracklets[tracklets[numTracklets - 1]].endID;

		// update hitIDs
		for (int i = 0; i < numTracklets; ++i) {
			inf.hitIDs.insert(inf.hitIDs.end(),
					fStartTracklets[tracklets[i]].hitIDs.begin(),
					fStartTracklets[tracklets[i]].hitIDs.end());
		}

	}

	inf.riemannTrack = CreateRiemannTrack(inf.hitIDs);

	if (inf.riemannTrack.getNumHits() > 2) {
		inf.riemannTrack.refit(false);
		inf.error = CalcDeviationOfRiemannTrack(inf.riemannTrack);
		inf.numErrHits = GetDeviationCount(inf.riemannTrack);
	}

	return inf;

}

bool PndSttCellTrackFinder::IsEndTubeOfTracklet(int tubeID) {

// tube is end of tracklet if there's only one hit-neighbor or if it has neighbors with ambiguity hits
	if (fHitNeighbors[tubeID].size() == 1
			|| fHitNeighbors[fHitNeighbors[tubeID][0]].size() > 2
			|| fHitNeighbors[fHitNeighbors[tubeID][1]].size() > 2) {
		return true;
	}

	return false;
}

PndRiemannTrack PndSttCellTrackFinder::CreateRiemannTrack(vector<int> hitIDs) {

	PndRiemannTrack riemannTrack;
	PndSttHit* sttHit;

	for (int i = 0; i < hitIDs.size(); ++i) {

		// check if hit was from skewed straw tube
		sttHit = (PndSttHit*) fHits[hitIDs[i]];
		if (!fStrawMap.IsSkewedStraw(sttHit->GetTubeID())) {

			// no skewed tube --> add hit to riemannTrack
			PndRiemannHit riemannHit(fHits[hitIDs[i]], hitIDs[i]);
			riemannTrack.addHit(riemannHit);

		}
	}

	return riemannTrack;
}

double PndSttCellTrackFinder::CalcDeviationOfRiemannTrack(
		PndRiemannTrack& track) {

	vector<PndRiemannHit> hits = track.getHits();
	TVector2 pos;
	TVectorD orig(2);
	orig = track.orig();
	TVector2 diff;
	double r = track.r();
	double sum = 0;

	for (int i = 0; i < track.getNumHits(); ++i) {

		// get x- and y-coordinate of the hit
		pos.Set(hits[i].x()[0], hits[i].x()[1]);
		// calc diff-vector to origin
		diff.Set(pos.X() - orig[0], pos.Y() - orig[1]);
		// calc distance to circle
		sum += (diff.Mod() - r) * (diff.Mod() - r);
	}

	return sum / track.getNumHits();

}

double PndSttCellTrackFinder::CalcDeviation(PndRiemannTrack& track, int hitID) {

	PndSttHit* sttHit = (PndSttHit*) fHits[hitID];
	TVector3 pos = fMapTubeIdToPos[sttHit->GetTubeID()];
	TVector2 diff;
	TVectorD orig(2);
	double r = track.r();
	orig = track.orig();

	diff.Set(pos.X() - orig[0], pos.Y() - orig[1]);

	return TMath::Abs(diff.Mod() - r);

}

int PndSttCellTrackFinder::GetDeviationCount(PndRiemannTrack& track) {
	int counter = 0;

	vector<PndRiemannHit> hits = track.getHits();
	TVector2 pos;
	TVectorD orig(2);
	orig = track.orig();
	TVector2 diff;
	double r = track.r();

	for (int i = 0; i < track.getNumHits(); ++i) {

// get x- and y-coordinate of the hit
		pos.Set(hits[i].x()[0], hits[i].x()[1]);
		diff.Set(pos.X() - orig[0], pos.Y() - orig[1]);

		if (TMath::Abs(diff.Mod() - r) > 0.5005) {
			++counter;
		}
	}

	return counter;
}

