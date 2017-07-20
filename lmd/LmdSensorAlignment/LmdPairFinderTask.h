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

struct dynamicCutHandler{
	int _overlapID;
	bool _ready;

	std::vector<double> samples;

	double _minDist, _maxDist, _hardMax, _curveWidth;

	dynamicCutHandler(){
		_overlapID = 0;
		_minDist = _maxDist = 0.0;
		_hardMax = 32*80e-4;	//sensors should not be farther than 32 pixels, 2,5mm!
		_curveWidth = 100e-4;	//empirical value, see report march 26th 2017
		_ready = false;
	}

	void addToSamples(PndLmdHitPair pair){

		//set first sample data
		if(samples.size() == 0){
			_overlapID = pair.getOverlapId();
			_minDist = pair.getDistance();
		}
		else{
			if(_overlapID != pair.getOverlapId()){
				cout << "something is wrong! stored OverlapID does not match added ID!\n";
				return;
			}
		}

		//100 should suffice, but more is always better
		if(samples.size() > 150){
			_ready = true;
		}

		double thisDistance=pair.getDistance();

		_minDist = std::min(_minDist, thisDistance);
		_maxDist = std::max(_maxDist, std::min(thisDistance, _hardMax) ); //never choose maximum higher than hardMax

		// store distance only if in valid range, some distances are too large
		if(thisDistance >= _minDist && thisDistance <= _maxDist){
			samples.push_back(pair.getDistance());
		}

		return;
	}

	void calcMinAndMax(){
		int noOfBuckets = 256;
		std::vector<int> buckets(noOfBuckets);

		for(int iSample=0; iSample<samples.size(); iSample++){

			int bucket=-1;

			//what bucket is this distance in?
			bucket = (int) floor(( samples[iSample]/_maxDist)*(noOfBuckets-1) );

			if(bucket >= 0 && bucket < noOfBuckets){
				//increment that bucket
				buckets[bucket]++;
			}
			else{
				cout << "I screwed up, this should not happen.\n";
			}
		}

		//find max bucket:
		int bucket=0;
		int maxentries = buckets[0];


		for(int iBucket=1; iBucket<buckets.size(); iBucket++){
			if(buckets[iBucket] >= maxentries ){
				maxentries = buckets[iBucket];
				bucket = iBucket;
			}
		}

		//calc min and max distance
		_minDist = std::max( ((bucket * _maxDist) / noOfBuckets ) - _curveWidth, 0.0 );			//in case min is negative
		_maxDist = std::min( ((bucket * _maxDist) / noOfBuckets ) + _curveWidth , _hardMax);	//in case max overflows

		return;
	}

	bool ready(){
		return _ready;
	}

	double getMinDist(){
		return _minDist;
	}
	double getMaxDist(){
		return _maxDist;
	}

};

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
	std::map<int, dynamicCutHandler> cutHandlers;

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

	//find the minDistance and maxDistance for dynamic cut
	void findDynamicCutParameters(Bool_t value){
		_findDynamicCutParameters = value;
	}

	//apply a dynamic cut. uses the first N pairs to decide what min and max distance should be.
	void useDynamicCut(Bool_t value){
		_useDynamicCut = value;
	}

	// apply distance cut, will be ignored when using dynamic cut
	void setMaxDistance(Double_t value) {
		_maxDistance = value;
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
	void getStatistics(PndLmdHitPair &candidate);

	bool applyDynamicDistanceCut(PndLmdHitPair &candidate);
	bool applyStaticDistanceCut(PndLmdHitPair &candidate);

	//self explanatory
	bool candHitsOverlappingArea(PndLmdHitPair &candidate);

	//means two clusters can reasonably belong to a single track
	bool candDistanceIsGood(PndLmdHitPair &candidate);

	ClassDef(LmdPairFinderTask,8);
};

#endif /* PAIRFINDERTASK_H_ */
