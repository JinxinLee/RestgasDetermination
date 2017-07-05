/*
 * PairFinderTask.h
 *
 *  Created on: Jul 22, 2014
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#ifndef LMDPAIRFINDERTASK_H_
#define LMDPAIRFINDERTASK_H_

#include "TClonesArray.h"

#include "PndSdsTask.h"
#include "PndLmdDim.h"
#include <PndLmdHitPair.h>

#include <string>
#include <vector>

class LmdPairFinderTask: public PndSdsTask {

private:

	//for statistics
	Int_t noOfGoodPairs, noOfEvents;
	Int_t plane0, plane1, plane2, plane3;
	Int_t eventMissedAllPlanes, noOverlap;
	Int_t distanceTooHigh;
	Int_t sumOfPixelHits;
	Int_t unsuitable;
	Int_t hitsClustered, hitsSinglePixel;
	Int_t noOfCombos;

	//deprecated
	//Bool_t _sortByModule;
	Bool_t _ignoreClusters;

	Double_t _maxDistance;

public:

	LmdPairFinderTask();
	LmdPairFinderTask(const char* name);
	virtual ~LmdPairFinderTask();

	/** Take default naming **/
	virtual void SetBranchNames();

	/** Virtual method Init **/
	virtual InitStatus Init();
	virtual InitStatus ReInit();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	virtual void FinishEvent();
	virtual void FinishTask();

	//function is deprecated!
	//void storeSorted(Bool_t value){
	//	_sortByModule = value;
	//}

	// apply distance cut (used squred internally)
	void setMaxDistance(Double_t value) {
		_maxDistance = value*value;
	}

	void ignoreClusters(Bool_t value){
		_ignoreClusters = value;
	}


protected:

	PndLmdDim* dimension;

	TClonesArray* mcPixels;
	TClonesArray* hitPairArray;
	std::map<int, TClonesArray*> hitPairMap;
	std::map<int, int> hitCountMap;

	void Register();
	void Reset();

	void transformToLMDlocal(PndLmdHitPair &pair);

	//function to return result of all checks, distance cut etc.
	bool isSuitable(PndLmdHitPair &candidate);

	//self explanatory
	bool candHitsOverlappingArea(PndLmdHitPair &candidate);
	bool candHitsOverlappingArea(Int_t firstSensorId, Int_t secondSensorId);

	//means two clusters can reasonably belong to a single track
	bool candDistanceIsGood(PndLmdHitPair &candidate);

	ClassDef(LmdPairFinderTask,7);
};

#endif /* PAIRFINDERTASK_H_ */
