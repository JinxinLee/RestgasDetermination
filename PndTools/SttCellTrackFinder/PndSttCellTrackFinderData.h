/*
 * PndSttCellTrackFinderData.h
 *
 *  Created on: May 8, 2014
 *      Author: schumann
 */

#ifndef PNDSTTCELLTRACKFINDERDATA_H_
#define PNDSTTCELLTRACKFINDERDATA_H_

#include <vector>
#include <map>

#include "TVector3.h"
#include "PndSttGeometryMap.h"
#include "PndSttStrawMap.h"
#include  "FairHit.h"

class TClonesArray;
class PndSttSkewedHit;

class PndSttCellTrackFinderData {

public:

	PndSttCellTrackFinderData(TClonesArray* fTubeArray);
	virtual ~PndSttCellTrackFinderData() {
		delete fStrawMap;
		delete fGeometryMap;
		for (int i = 0; i < fHits.size(); ++i)
			delete fHits.at(i);

	}

	void GenerateNeighborhoodData(std::vector<FairHit*> hits,
			std::multimap<int, PndSttSkewedHit*> combinedSkewedHits, bool allowDoubleHits=true);

	void clear() {
		fHits.clear();
		fMapTubeIdToHit.clear();
		fHitNeighbors.clear();
		fSeparations.clear();
		fHitNeighborsWithoutEdges.clear();
		fSeparationsWithoutEdges.clear();
		fHitNeighborsWithoutSkewed.clear();
		fSeparationsWithoutSkewed.clear();
	}

	void PrintInfo();

	std::vector<FairHit*> GetHits() const {
		return fHits;
	}

	std::multimap<int, PndSttSkewedHit*> GetCombinedSkewedHits() const {
		return fCombinedSkewedHits;
	}

	const PndSttStrawMap* GetStrawMap() const {
		return fStrawMap;
	}

	const PndSttGeometryMap* GetGeometryMap() const {
		return fGeometryMap;
	}

	std::map<int, int> GetMapTubeIdToHit() const {
		return fMapTubeIdToHit;
	}

	std::map<int, TVector3> GetMapTubeIdToPos() const {
		return fMapTubeIdToPos;
	}

	std::map<int, std::vector<int> > GetHitNeighbors() const {
		return fHitNeighbors;
	}

	std::map<int, std::vector<int> > GetSeparations() const {
		return fSeparations;
	}

	std::map<int, std::vector<int> > GetHitNeighborsWithoutEdges() const {
		return fHitNeighborsWithoutEdges;
	}

	std::map<int, std::vector<int> > GetSeparationsWithoutEdges() const {
		return fSeparationsWithoutEdges;
	}

	std::map<int, std::vector<int> > GetHitNeighborsWithoutSkewed() const {
		return fHitNeighborsWithoutSkewed;
	}

	std::map<int, std::vector<int> > GetSeparationsWithoutSkewed() const {
		return fSeparationsWithoutSkewed;
	}

	int GetNumHits(){
		return fNumHits;
	}

	int GetNumHitsWithoutDouble(){
		return fNumHitsWithoutDouble;
	}

private:

	std::vector<FairHit*> fHits;		//vector with all hits of the an event
	std::multimap<int, PndSttSkewedHit*> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

	bool fAllowDoubleHits;
	int fNumHits;
	int fNumHitsWithoutDouble;

	PndSttStrawMap* fStrawMap;	// for getting more information about the tubes
	PndSttGeometryMap* fGeometryMap;// for initializing the neighbors of each tube
	std::map<int, TVector3> fMapTubeIdToPos;// map<straw id, position of the center of the tube>
	std::map<int, int> fMapTubeIdToHit; // map< id of straw tube, index of hit in fHit>

	std::map<int, std::vector<int> > fHitNeighbors; // map<straw id, vector<ids of hit-neighbors>>
	std::map<int, std::vector<int> > fSeparations; // map<#active neighbors, vector<straw ids>>

	std::map<int, std::vector<int> > fHitNeighborsWithoutEdges; // map<straw id, vector<ids of hit-neighbors>>
	std::map<int, std::vector<int> > fSeparationsWithoutEdges; // map<#active neighbors, vector<straw ids>>

	std::map<int, std::vector<int> > fHitNeighborsWithoutSkewed; // map<straw id, vector<ids of hit-neighbors>>
	std::map<int, std::vector<int> > fSeparationsWithoutSkewed; // map<#active neighbors, vector<straw ids>>

	/* Method grades the active cells according to the number of hit-neighbors.*/
	void SeparateNeighbors();

	/* Method searches for hit-neighbors of each cell.*/
	void FindHitNeighbors();

ClassDef(PndSttCellTrackFinderData,1)
	;
};

#endif /* PNDSTTCELLTRACKFINDERDATA_H_ */
