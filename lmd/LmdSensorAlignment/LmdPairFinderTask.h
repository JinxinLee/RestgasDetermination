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
//#include "PndLmdDim.h"
#include "PndLmdAlignManager.h"
#include "PndLmdAlignStructs.h"
#include <PndLmdHitPair.h>
#include <PndSdsHit.h>

//apparently, CINT has a problem with some boost classes
//#ifndef __CINT__
#include <boost/property_tree/ptree.hpp>
//#endif

#include <string>
#include <vector>

class PndLmdGeometryHelper;

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

	Double_t _maxDistance;

	Bool_t _ignoreClusters;

	//data for dynamic cut
	Bool_t _findDynamicCutParameters;
	Bool_t _useDynamicCut;
	std::string _cutParameterFile;
	std::map<int, dynamicCutHandler> cutHandlers;

	pixelHit getPixelHitFromSdsHit(PndSdsHit *sdsHit);

	//#ifndef __CINT__
	boost::property_tree::ptree config;
	//#endif

public:

	void SetParContainers();

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

	//find the minDistance and maxDistance for dynamic cut
	void findDynamicCutParameters(Bool_t value, std::string parameterFile){
		_findDynamicCutParameters = value;
		_cutParameterFile = parameterFile;
	}

	//apply a dynamic cut. uses the first N pairs to decide what min and max distance should be.
	void useDynamicCut(Bool_t value, std::string parameterFile){
		_useDynamicCut = value;
		_cutParameterFile = parameterFile;
	}

	// apply distance cut, will be ignored when using dynamic cut
	void setMaxDistance(Double_t value) {
		_maxDistance = value;
	}

	void ignoreClusters(Bool_t value){
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
	std::map<int, int> hitCountMap;

	void Register();
	void Reset();

	//void transformToLMDlocal(PndLmdHitPair &pair);

	//function to return result of all checks, distance cut etc.
	void getStatistics(PndLmdHitPair &candidate);

	bool applyDynamicDistanceCut(PndLmdHitPair &candidate);
	bool applyStaticDistanceCut(PndLmdHitPair &candidate);

	//self explanatory
	bool candHitsOverlappingArea(PndLmdHitPair &candidate);

	//means two clusters can reasonably belong to a single track
	bool candDistanceIsGood(PndLmdHitPair &candidate);

	ClassDef(LmdPairFinderTask,11);
};

#endif /* PAIRFINDERTASK_H_ */
