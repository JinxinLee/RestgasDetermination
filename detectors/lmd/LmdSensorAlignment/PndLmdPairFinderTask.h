/*
 * PairFinderTask.h
 *
 *  Created on: Jul 22, 2014
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#ifndef PNDLMDPAIRFINDERTASK_H_
#define PNDLMDPAIRFINDERTASK_H_

#include "PndLmdAlignStructs.h"
#include <PndLmdHitPair.h>
#include <PndSdsHit.h>
#include "PndSdsTask.h"

#include "TClonesArray.h"

#include <iostream>
#include <string>
#include <vector>

class PndLmdGeometryHelper;

class PndLmdPairFinderTask: public PndSdsTask {

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

	Double_t _maxDistance;

	Bool_t _ignoreClusters;

	pixelHit getPixelHitFromSdsHit(PndSdsHit *sdsHit);

public:

	void SetParContainers();

	PndLmdPairFinderTask();
	PndLmdPairFinderTask(const char* name);
	virtual ~PndLmdPairFinderTask();

	/** Take default naming **/
	virtual void SetBranchNames();

	/** Virtual method Init **/
	virtual InitStatus Init();
	virtual InitStatus ReInit();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	virtual void FinishEvent();
	virtual void FinishTask();

	// apply distance cut, will be ignored when using dynamic cut
	void setMaxDistance(Double_t value) {
		_maxDistance = value;
	}

	void ignoreClusters(Bool_t value) {
		_ignoreClusters = value;
	}

protected:

	//PndLmdDim* dimension;

	PndLmdGeometryHelper *helper;

	TClonesArray* digiArray;
	TClonesArray* recoArray;
	TClonesArray* hitPairArray;
	TClonesArray* clusterCandidateArray;

	TString fInRecoBranchName;
	TString fInClusterCandidates;

	std::map<int, TClonesArray*> hitPairMap;

	void Register();
	void Reset();

	//function to return result of all checks, distance cut etc.
	void getStatistics(PndLmdHitPair &candidate);

	bool pairDistanceValid(PndLmdHitPair &candidate);

	//self explanatory
	bool candHitsOverlappingArea(const PndLmdHitPair &candidate);

ClassDef(PndLmdPairFinderTask,20)
	;
};

#endif /* PAIRFINDERTASK_H_ */
