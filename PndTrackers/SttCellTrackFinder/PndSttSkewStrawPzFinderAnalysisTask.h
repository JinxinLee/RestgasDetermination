/*
 * PndSttSkewStrawPzFinderAnalysisTask.h
 *
 *  Created on: Apr 6, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDERANALYSISTASK_H_
#define PNDSTTSKEWSTRAWPZFINDERANALYSISTASK_H_

#include <FairTask.h>
#include <PndSttSkewStrawPzFinder.h>

class TClonesArray;
class TCanvas;
class PndGeoSttPar;
class PndSttHit;
class PndTrack;
class PndTrackCand;

class PndMCMatch;
class PndMCResult;

using namespace std;

class PndSttSkewStrawPzFinderAnalysisTask: public FairTask {
public:
	ClassDef(PndSttSkewStrawPzFinderAnalysisTask,1);

	PndSttSkewStrawPzFinderAnalysisTask();

	virtual ~PndSttSkewStrawPzFinderAnalysisTask();

	virtual InitStatus Init();

	virtual void Exec(Option_t* opt);

	virtual void Finish();

private:

	Int_t fVerbose;

	TClonesArray *fEventHeader;
	TCanvas *SttXYproj;
	vector<PndSttHit*> fSkewedHits;
	TClonesArray *fSTTHits;
	PndGeoSttPar *fSttParameters;     // for filling fTubeArray
	TClonesArray *fTubeArray;

	TClonesArray *fMCTrack;
	//PndMCMatch* fMCMatch;

	TClonesArray *fFirstTrack;
	TClonesArray *fFirstTrackCand;
	TClonesArray *fFirstRiemannTrack;

	TClonesArray *fCombiTrackCand;
	TClonesArray *fCombiTrack;
	TClonesArray *fCombiRiemannTrack;

	TClonesArray* fFinalTrackCand;
	TClonesArray* fFinalTrack;
	TClonesArray *fFinalRiemannTrack;

	TClonesArray *fPzData;

	//TrackingQA output branches
	TClonesArray *fMCTrackInfo;
	TClonesArray *fRecoTrackInfo;

	void DrawSkewTubeLines(PndSttHit* hit);

	void DrawStt(bool isskew);

	void DrawHitIsochrone(PndSttHit* hit, Color_t color);

	void DrawHitSkewedIsochrone(PndSttHit* hit, Color_t color, Double_t beta, Double_t a, Double_t b);

	void DrawTrackParams(PndTrack* track);

	void DrawCombiLines(vector<vector< TVector2> > ZPhiPairVector);

	void DrawCombiLinesResult(vector< TVector2> TrueZPhi);

	void DrawZPhiBase(vector<vector< TVector2> > ZPhiPairVector, vector< TVector2> TrueZPhiVector,
			Double_t slope, Double_t intercept,
			Double_t MCslope, Double_t MCintercept);
};

#endif /* PNDSTTSKEWSTRAWPZFINDERANALYSISTASK_H_ */
