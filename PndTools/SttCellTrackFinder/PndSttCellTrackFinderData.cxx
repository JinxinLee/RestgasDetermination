/*
 * PndSttCellTrackFinderData.cpp
 *
 *  Created on: May 8, 2014
 *      Author: schumann
 */

#include "PndSttCellTrackFinderData.h"
#include "PndSttStrawMap.h"
#include "PndSttGeometryMap.h"
#include "PndSttTube.h"
#include "PndSttHit.h"
#include <stdio.h>

//macro for printing tubes + neighbors to a file called tubeNeighborings.txt
//#define PRINT_STT_NEIGHBORS

using namespace std;

ClassImp(PndSttCellTrackFinderData);

PndSttCellTrackFinderData::PndSttCellTrackFinderData(
		TClonesArray* sttTubeArray) {

	// Generate information of Straw- and GeometryMap.
	// It is always the same data for all events.

	fStrawMap = new PndSttStrawMap(sttTubeArray);
	fGeometryMap = new PndSttGeometryMap(sttTubeArray, 1);

	PndSttTube* tube;
	TVector3 pos;

#ifdef PRINT_STT_NEIGHBORS
	TArrayI neighbors;
	FILE* fp = fopen("tubeNeighborings.txt", "w");
	int minNumNeigh=100, maxNumNeigh=0, numSkewed=0, numSkewedLessNeig=0;
	bool skewed;
#endif

	for (int i = 1; i < sttTubeArray->GetEntriesFast(); ++i) {
		tube = (PndSttTube*) sttTubeArray->At(i);
		pos = tube->GetPosition();
		fMapTubeIdToPos[i] = pos;

#ifdef PRINT_STT_NEIGHBORS
		neighbors=fGeometryMap->GetNeighboringsByMap(tube->GetTubeID());
		skewed=fGeometryMap->IsSkewedStraw(tube->GetTubeID());
		if(skewed) ++numSkewed;
		if(skewed && neighbors.GetSize()<7) ++numSkewedLessNeig;
		fprintf(fp, "%d %4i:",skewed, tube->GetTubeID());
		for(int j=0; j<neighbors.GetSize(); ++j) {
			fprintf(fp, " %4i", neighbors[j]);
		}
		fprintf(fp, "\n");

		if(minNumNeigh>neighbors.GetSize()) minNumNeigh=neighbors.GetSize();
		if(maxNumNeigh<neighbors.GetSize()) maxNumNeigh=neighbors.GetSize();
#endif
	}

#ifdef PRINT_STT_NEIGHBORS
	fprintf(fp, "max/min number of neighbors: %3i / %3i\n", maxNumNeigh, minNumNeigh);
	fprintf(fp, "number of skewed straws: %3i \n", numSkewed);
	fprintf(fp, "number of skewed straws with less than 7 neighbors: %3i \n", numSkewedLessNeig);
	fclose(fp);
#endif

}

void PndSttCellTrackFinderData::GenerateNeighborhoodData(vector<FairHit*> hits,
		multimap<int, PndSttSkewedHit*> combinedSkewedHits,
		bool allowDoubleHits) {

	//fill set with tubeIDs to remove double hits
	int tubeId;
	set<int> sttHits;
	vector<FairHit*> hitsWithoutDouble;
	for(int i=0; i<hits.size(); ++i){
		tubeId=((PndSttHit*) hits[i])->GetTubeID();
		if(sttHits.find(tubeId)==sttHits.end()){
			sttHits.insert(tubeId);
			hitsWithoutDouble.push_back(hits[i]);
		}
	}

	fNumHits=hits.size();
	fNumHitsWithoutDouble=sttHits.size();

	if (!allowDoubleHits) {
		fHits = hitsWithoutDouble;

	} else {
		fHits = hits;
	}

	fCombinedSkewedHits = combinedSkewedHits;

	PndSttHit* sttHit;
	for (int i = 0; i < fHits.size(); ++i) {
		sttHit = (PndSttHit*) fHits[i];
		fMapTubeIdToHit[sttHit->GetTubeID()] = i;
	}

	FindHitNeighbors();
	SeparateNeighbors();
}

void PndSttCellTrackFinderData::FindHitNeighbors() {

	/* Approach: At first create a set of the tubeIDs of all hits.
	 * Then get the neighbors of each hit/tube and store only those
	 * that are included in the set.*/

	PndSttHit* sttHit;
	int tubeId;
	set<int> hitIds;

	//initialize set with straw-ids of hits
	for (int i = 0; i < fHits.size(); ++i) {
		sttHit = (PndSttHit*) fHits[i];
		hitIds.insert(sttHit->GetTubeID());
	}

	//fill fHitNeighbors
	for (int i = 0; i < fHits.size(); ++i) {
		sttHit = (PndSttHit*) fHits[i];
		tubeId = sttHit->GetTubeID();
		//get neighbors
		TArrayI neighbors = fGeometryMap->GetNeighboringsByMap(tubeId);

		for (int j = 0; j < neighbors.GetSize(); ++j) {

			//set contains neighbor?
			if (hitIds.find(neighbors[j]) != hitIds.end()) {

				//add to map with all neighbors
				fHitNeighbors[tubeId].push_back(neighbors[j]);

				//actual tube and neighbor are not both on the edge of the stt?
				if (!(fStrawMap->IsEdgeStraw(tubeId)
						&& fStrawMap->IsEdgeStraw(neighbors[j]))) {

					//add to the other map
					fHitNeighborsWithoutEdges[tubeId].push_back(neighbors[j]);
				}
			}
		}

		if (fHitNeighbors.find(tubeId) == fHitNeighbors.end()) {
			//no hitNeighbor was found
			vector<int> tmp;
			fHitNeighbors[tubeId] = tmp;
		}

		if (fHitNeighborsWithoutEdges.find(tubeId)
				== fHitNeighborsWithoutEdges.end()) {
			//no hitNeighbor was found
			vector<int> tmp;
			fHitNeighbors[tubeId] = tmp;
		}

		if (!fStrawMap->IsSkewedStraw(tubeId)) {
			for (int j = 0; j < neighbors.GetSize(); ++j) {
				if (hitIds.find(neighbors[j]) != hitIds.end()
						&& !fStrawMap->IsSkewedStraw(neighbors[j])) {
					fHitNeighborsWithoutSkewed[tubeId].push_back(neighbors[j]);
				}
			}
		}
	}
}

void PndSttCellTrackFinderData::SeparateNeighbors() {

	/* Separate the hits/active cells concerning the number of neighbors that made a signal, too.*/

	map<int, vector<int> >::iterator it;

	for (int i = 0; i < 8; ++i) {
		fSeparations[i].clear();
		fSeparationsWithoutEdges[i].clear();
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

	for (it = fHitNeighborsWithoutEdges.begin();
			it != fHitNeighborsWithoutEdges.end(); ++it) {

		if (it->second.size() > 6) {
			// store cells with more than 6 neighbors in the same entry
			fSeparationsWithoutEdges[7].push_back(it->first);
		} else {
			// separation concerning 0-6 hit-neighbors
			fSeparationsWithoutEdges[it->second.size()].push_back(it->first);
		}
	}

	for (it = fHitNeighborsWithoutSkewed.begin();
			it != fHitNeighborsWithoutSkewed.end(); ++it) {

		if (it->second.size() > 6) {
			// store cells with more than 6 neighbors in the same entry
			fSeparationsWithoutSkewed[7].push_back(it->first);
		} else {
			// separation concerning 0-6 hit-neighbors
			fSeparationsWithoutSkewed[it->second.size()].push_back(it->first);
		}
	}
}

void PndSttCellTrackFinderData::PrintInfo() {
	cout << "PndSttCellTrackFinderData::PrintInfo()" << endl;
	cout << "#hits: " << fHits.size() << ", #unambiguous: "
			<< fSeparations[0].size() + fSeparations[1].size()
					+ fSeparations[2].size() << ", #ambiguous: "
			<< fSeparations[3].size() + fSeparations[4].size()
					+ fSeparations[5].size() + fSeparations[6].size()
					+ fSeparations[7].size() << endl;

	cout << "fHits (*-skewed): ";
	int tubeID;
	for (int i = 0; i < fHits.size(); ++i) {
		tubeID = ((PndSttHit*) fHits.at(i))->GetTubeID();
		cout << tubeID;
		if (fStrawMap->IsSkewedStraw(tubeID))
			cout << "*";
		cout << ", ";
	}
	cout << endl;

	cout << "fSeparations: " << endl;
	for (int i = 0; i < 8; ++i) {
		cout << "#" << i << ": ";
		for (int j = 0; j < fSeparations[i].size(); ++j) {
			cout << fSeparations[i].at(j);
			if (fStrawMap->IsSkewedStraw(tubeID))
				cout << "*";
			cout << ", ";
		}
		cout << endl;
	}

	cout << "fHitNeighbors: " << endl;
	for (map<int, vector<int>>::iterator it = fHitNeighbors.begin();
			it != fHitNeighbors.end(); ++it) {
		cout << it->first << ": ";
		for (int i = 0; i < it->second.size(); ++i) {
			cout << it->second.at(i) << " ";
		}
		cout << endl;
	}
}
