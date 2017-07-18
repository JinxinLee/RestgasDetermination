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
	int _sampleSize;
	bool _ready;

	std::vector<double> samples;

	double _minDist, _maxDist;

	dynamicCutHandler(){
		_overlapID = 0;
		_minDist = _maxDist = 0.0;
		_sampleSize = 10;
		_ready = false;
	}

	void addToSamples(PndLmdHitPair pair){
		samples.push_back(pair.getDistance());

		_maxDist = std::max(_maxDist, pair.getDistance());

		if(samples.size() >= _sampleSize){
			calcMinAndMax();
			_ready = true;
		}
	}

	void calcMinAndMax(){
		int noOfBuckets = 10;
		std::vector<int> buckets(noOfBuckets);

		for(int iSample=0; iSample<samples.size(); iSample++){

			int bucket=-1;

			//what bucket is this distance in?
			bucket = (int)samples[iSample]*noOfBuckets / _maxDist;

			if(bucket >= 0){
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
			if(buckets[iBucket] > maxentries ){
				bucket = iBucket;
			}
		}

		int bucketSpread=2;
		//calc min and max distance
		_minDist = (bucket-bucketSpread) / 10.0 * _maxDist;
		_maxDist = (bucket+bucketSpread) / 10.0 * _maxDist;

		cout << "overlap:" << _overlapID << ", found min: " << _minDist << " and max: " << _maxDist << "\n";

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

	Bool_t _ignoreClusters;

	Int_t _dynamicSampleSize;
	std::vector<Double_t> _dynamicSamples;
	Bool_t _useDynamicCut;
	Double_t _minDistance;
	Double_t _maxDistance;
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

	/*
	 * apply a dynamic cut. uses the first N pairs to decide what min and max distance should be.
	 */
	void useDynamicCut(Bool_t value, Int_t sampleSize = 1000){
		_useDynamicCut = value;
		_dynamicSampleSize = sampleSize;
	}

	// apply distance cut (used squred internally), will be ignored when using dynamic cut
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
