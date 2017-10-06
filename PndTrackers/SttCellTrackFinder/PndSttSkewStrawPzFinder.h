/*
 * PndSttSkewStrawPzFinder.h
 *
 *  Created on: Mar 17, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDER_H_
#define PNDSTTSKEWSTRAWPZFINDER_H_

#include <iostream>

//Root includes
#include <TH2D.h>

//Pandaroot includes
#include <PndSttSkewStrawPzFinderData.h>
#include "PndSttStrawMap.h"
#include <PndRiemannTrack.h>

class PndSttHit;
class PndTrack;
class PndTrackCand;

class TH2D;

using namespace std;

class PndSttSkewStrawPzFinder {
public:
	PndSttSkewStrawPzFinder(TClonesArray* sttTubeArray, TClonesArray* sttHitArray);

	virtual ~PndSttSkewStrawPzFinder() {

	}

	void AddPndRiemannTracks(vector<PndRiemannTrack> AllRiemannTracks);

	void AddPndTracks(vector<PndTrack> AllTracks);

	void AddPndTrackCands(vector<PndTrackCand> AllTrackCands);

	void AddPndSttHits(vector<vector<PndSttHit> > AllSkewedHits);

	void ExtractPz();

	PndRiemannTrack GetRiemannTrack(int i) {
		return fResultPndRiemannTrack[i];
	}
	;

	PndTrack GetTrack(int i) {
		return fResultPndTrack[i];
	}
	;

	PndTrackCand GetTrackCand(int i) {
		return fResultPndTrackCand[i];
	}
	;

	PndSttSkewStrawPzFinderData GetPzData(int i) {
		return fResultPzData[i];
	}
	;

	vector<PndSttHit> GetSttHits(int i) {
		return fResultSkewedSttHits[i];
	}
	;

	int NumPndRiemannTracks() {
		return fResultPndRiemannTrack.size();
	}
	;

	int NumPndTracks() {
		return fResultPndTrack.size();
	}
	;

	int NumPndTrackCands() {
		return fResultPndTrackCand.size();
	}
	;

	int NumPndPzData() {
		return fResultPzData.size();
	}
	;

	int NumPndSttHits() {
		return fResultSkewedSttHits.size();
	}
	;

	TH2D* getHoughHisto() {
		return fHoughHisto;
	}

	void Reset(){
		fHoughHisto->Reset();

		fVectorPndRiemannTrack.clear();
		fVectorPndTrack.clear();
		fVectorPndTrackCand.clear();
		fVectorSkewedSttHits.clear();

		fResultPndRiemannTrack.clear();
		fResultPndTrack.clear();
		fResultPndTrackCand.clear();
		fResultPzData.clear();
		fResultSkewedSttHits.clear();

		//ZPhiPairVector.clear();
		//TrueZPhi.clear();

		if (fVerbose > 0) {
			cout << "PndSttSkewStrawPzFinder::Reset()"
					<< endl;
		}
	}

private:
	int fVerbose;
	Double_t fSteps;
	Double_t LineCombiAngleThreshold;
	TClonesArray *fTubeArray;
	TClonesArray *fSTTHits;
	PndSttStrawMap* fStrawMap;	// for getting more information about the tubes

	TH2D* fHoughHisto;	//Line Hough histogram

	//Vectors of initial tracks and hits
	vector<PndRiemannTrack> fVectorPndRiemannTrack;
	vector<PndTrack> fVectorPndTrack;
	vector<PndTrackCand> fVectorPndTrackCand;

	vector<vector<PndSttHit> > fVectorSkewedSttHits;

	//Vectors of final tracks and hits that extracted Pz
	vector<PndRiemannTrack> fResultPndRiemannTrack;
	vector<PndTrack> fResultPndTrack;
	vector<PndTrackCand> fResultPndTrackCand;

	vector<vector<PndSttHit> > fResultSkewedSttHits;

	//Additional data produced by PzFinder
	vector<PndSttSkewStrawPzFinderData> fResultPzData;

	//Vectors used internally in the PzFinder
	//vector< vector<TVector2> > ZPhiPairVector;
	//vector<TVector2> TrueZPhi;

	void InitSkewed();

	bool Clockwise(vector<vector<TVector2> > ZPhiPairVector, TVector2 center);//TODO: implement check clockwise!

	Int_t ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2, double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2);

	TVector2 ComputeTangentInPoint(double xc, double yc, TVector2 point);

	//TODO: Generalize output data structure to vector<vector<TVector2>>
	vector<TVector2> LineCombiIsoFinder(vector<vector< TVector2> > ZPhiPairVector);

	//TODO: refine Hough transform part
	//TODO: Generalize input data structure to vector<vector<TVector2>>
	vector<TVector2> HoughTrueIsoFinder(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams);

	vector<TVector2> HoughTrueIsoFinder2(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams);

	vector<vector< TVector2> > TranslateZPhi(vector<vector< TVector2> > ZPhiPairVector);

	TVector2 TheilSen(vector<vector< TVector2> > ZPhiPairVector);

	vector<vector<PndSttHit> > MoveSkewedHitsToCircle(TVector2 circle, Double_t circlerad, vector<PndSttHit> skewhits);

	void NestedFor(vector<int> &times, vector<int> &current, pair<vector<int>,double> &best, double sum, vector<vector<pair<vector<int>,double> > > matrix, int depth);

	TVector2 PzLineFitExtract(vector< TVector2> ZPhi);

	TVector2 PzLineFitExtract2(vector<vector< TVector2> > ZPhiPairVector);

	FairTrackParP GetTrackParam(FairTrackParP oldParam, PndRiemannTrack circle, TVector2 lineparams, Double_t pz);

	Double_t CalculateResidual(Double_t lineSlope, Double_t lineIntercept, TVector2 ZPhiPoint);





	ClassDef(PndSttSkewStrawPzFinder,1);
};

#endif /* PNDSTTSKEWSTRAWPZFINDER_H_ */
