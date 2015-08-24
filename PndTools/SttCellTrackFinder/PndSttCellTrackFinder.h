/*
* PndSttCellTrackFinder.h
*
*  Created on: May 20, 2014
*      Author: schumann
*/

#ifndef PndSttCellTrackFinder_H_
#define PndSttCellTrackFinder_H_

#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndTrack.h"
#include "FairLink.h"
#include "PndSttCellTrackFinderData.h"
#include "PndSttCellTrackletGenerator.h"
#include "PndSttHitCorrector.h"
#include "PndSttSkewedHit.h"

class TClonesArray;

class PndSttCellTrackFinder {
public:
	PndSttCellTrackFinder() :
		fVerbose(0), fCalcFirstTrackletInf(kFALSE),fTrackletGenerator(0),fHitCorrector(0) {
	}
	;
	virtual ~PndSttCellTrackFinder() {
		delete fTrackFinderData;
		delete fHitCorrector;
		//delete fTrackletGenerator;
		for (int i = 0; i < fHits.size(); ++i) {
			delete fHits.at(i);
		}
		for (std::multimap<int, PndSttSkewedHit*>::iterator it =
				fCombinedSkewedHits.begin(); it != fCombinedSkewedHits.end();
				++it) {
			delete (*it).second;
		}

	}

	void FindTracks();

	void AddHits(TClonesArray* hits, Int_t branchId);

	void SetSttTubeArray(TClonesArray* sttTubeArray);

	std::map<Int_t, FairHit*> GetCorrectedIsochrones() {
		return fHitCorrector->GetCorrectedHits();
	}

	// Get TrackCands of start-tracklets before combination
	PndTrackCand GetFirstTrackCand(int i) {
		return fFirstTrackCand[i];
	}
	;

	// Get RiemannTracks before combination of tracklets
	PndRiemannTrack GetFirstRiemannTrack(int i) {
		return fFirstRiemannTrack[i];
	}
	;

	// Get TrackCands of combinated tracklets
	PndTrackCand GetCombiTrackCand(int i) {
		return fCombiTrackCand[i];
	}
	;

	// Get PndTrack of combinated tracklets
	PndTrack GetCombiTrack(int i) {
		return fCombiTrack[i];
	}
	;

	PndRiemannTrack GetCombiRiemannTrack(int i) {
		return fCombiRiemannTrack[i];
	}
	;

	PndTrackCand GetCorrectedCombiTrackCand(int i) {
		return fCorrectedCombiTrackCand[i];
	}
	;

	PndTrack GetCorrectedCombiTrack(int i) {
		return fCorrectedCombiTrack[i];
	}
	;

	PndRiemannTrack GetCorrectedCombiRiemannTrack(int i) {
		return fCorrectedCombiRiemannTrack[i];
	}
	;

	std::vector<std::vector<Double_t> > GetTimeStampsTrackletGen() {
		return fTimeStampsTrackletGen;
	};

	std::vector<std::vector<Double_t> > GetTimeStampsGenerateNeighborhoodData() {
			return fTimeStampsGenerateNeighborhoodData;
		};

	int NumFirstTrackCands() {
		return fFirstTrackCand.size();
	}
	;

	int NumFirstRiemannTracks() {
		return fFirstRiemannTrack.size();
	}
	;

	int NumCombinedTracks() {
		return fCombiTrackCand.size();
	}
	;

	int NumCombinedRiemannTracks() {
		return fCombiRiemannTrack.size();
	}
	;

	int NumHits(){
			return fTrackFinderData->GetNumHits();
	}

	int NumHitsWithoutDouble(){

		return fTrackFinderData->GetNumHitsWithoutDouble();
	}

	int NumUnambiguousNeighbors(){
		return fTrackFinderData->GetSeparations()[1].size()+fTrackFinderData->GetSeparations()[2].size();
	}

	void SetCalcFirstTrackletInf(Bool_t val) {
		fCalcFirstTrackletInf = val;
	}
	;

	void SetVerbose(Int_t val) {
		fVerbose = val;
	}
	;

	void StoreTrackData(){
		fFirstTrackCand=fTrackletGenerator->GetFirstTrackCands();
		if(fCalcFirstTrackletInf)
			fFirstRiemannTrack=fTrackletGenerator->GetFirstRiemannTracks();

		if(!fTrackletGenerator->CalcWithCorrectedHits()){

			fCombiTrackCand=fTrackletGenerator->GetCombiTrackCands();
			fCombiRiemannTrack=fTrackletGenerator->GetCombiRiemannTracks();
			fCombiTrack=fTrackletGenerator->GetCombiTracks();
		} else {
			fCorrectedCombiTrackCand=fTrackletGenerator->GetCombiTrackCands();
			fCorrectedCombiRiemannTrack=fTrackletGenerator->GetCombiRiemannTracks();
			fCorrectedCombiTrack=fTrackletGenerator->GetCombiTracks();

		}


	};

	void Reset() {
		fHits.clear();
		fTrackFinderData->clear();

		fFirstTrackCand.clear();
		fFirstRiemannTrack.clear();

		fCombiTrackCand.clear();
		fCombiTrack.clear();
		fCombiRiemannTrack.clear();

		fCorrectedCombiTrackCand.clear();
		fCorrectedCombiTrack.clear();
		fCorrectedCombiRiemannTrack.clear();

		if(fHitCorrector!=0)	delete fHitCorrector;
		if(fTrackletGenerator!=0) delete fTrackletGenerator;

	}




private:

	std::vector<std::vector<Double_t> > fTimeStampsTrackletGen;
	std::vector<std::vector<Double_t> > fTimeStampsGenerateNeighborhoodData;

	Int_t fVerbose;
	Bool_t fCalcFirstTrackletInf; // if true, calculate riemannTracks for start-tracklets
	std::vector<FairHit*> fHits;	// vector with all hits of the current event
	std::multimap<int, PndSttSkewedHit*> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

	std::map<int, FairLink> fMapHitToFairLink; // map< index of hit in fHit, FairLink of SttHit>

	PndSttCellTrackFinderData* fTrackFinderData;
	PndSttCellTrackletGenerator* fTrackletGenerator;
	PndSttHitCorrector* fHitCorrector;

	// for first step of trackfinding
	std::vector<PndTrackCand> fFirstTrackCand; // for saving trackCands after the use of cellular automaton
	std::vector<PndRiemannTrack> fFirstRiemannTrack; //	for saving + plotting the riemann-tracks after the first step

	std::vector<PndTrackCand> fCombiTrackCand; // resulting tracks of combined tracklets
	std::vector<PndTrack> fCombiTrack; // resulting PndTrack
	std::vector<PndRiemannTrack> fCombiRiemannTrack;

	std::vector<PndTrackCand> fCorrectedCombiTrackCand;
	std::vector<PndTrack> fCorrectedCombiTrack;
	std::vector<PndRiemannTrack> fCorrectedCombiRiemannTrack;

	ClassDef(PndSttCellTrackFinder,1)
	;

};

#endif /* PndSttCellTrackFinder_H_ */
