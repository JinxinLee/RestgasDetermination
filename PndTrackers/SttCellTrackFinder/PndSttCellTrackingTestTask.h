/*
 * PndSttCellTrackingTestTask.h
 *
 *  Created on: Nov 18, 2014
 *      Author: micpa904
 */

#ifndef PNDSTTCELLTRACKINGTESTTASK_H_
#define PNDSTTCELLTRACKINGTESTTASK_H_

#include <FairTask.h>

#include "TH2D.h"

class TClonesArray;
class TCanvas;
class PndGeoSttPar;
class PndSttHit;

using namespace std;

class PndSttCellTrackingTestTask: public FairTask {
public:
  ClassDef(PndSttCellTrackingTestTask,1);
  PndSttCellTrackingTestTask();
  virtual ~PndSttCellTrackingTestTask();

  virtual InitStatus Init();
  virtual void Exec();
  virtual void FinishEvent();

private:

  Int_t fVerbose;
  bool fDraw;
  TClonesArray *fEventHeader;
  TCanvas *SttXYproj;
  vector<PndSttHit*> fSkewedHits;
  TClonesArray *fSTTHits;
  PndGeoSttPar *fSttParameters;     // for filling fTubeArray
  TClonesArray *fTubeArray;
  /** Input array of MVDHitsPixel **/
  TClonesArray* fMvdPixelHitArray;
  /** Input array of MVDHitsStrip **/
  TClonesArray* fMvdStripHitArray;
  map<int, vector<int> > fMapTubeIDToHits;
  map<int, int> fMapHitIndexToTubeID;

  TClonesArray *fFirstTrackCand;
  TClonesArray *fFirstRiemannTrack;
  TClonesArray *fCombiTrackCand;
  TClonesArray *fCombiTrack;
  TClonesArray *fCombiRiemannTrack;

  vector<PndTrackCand> fVectorPndTrackCand;
  vector<PndTrack> fVectorPndTrack;

  TClonesArray* fFinalTrackCand;
  TClonesArray* fFinalTrack;

  TCanvas *fCanvas;
  Double_t fSteps;

  void ExtractSkewedHits(Int_t eventNumber);
  void DrawSkewTubeLines(PndSttHit* hit);
  void DrawStt(bool isskew);
  void DrawHitIsochrone(PndSttHit* hit, Color_t color);
  void DrawHitSkewedIsochrone(PndSttHit* hit, Color_t color, Double_t beta, Double_t a, Double_t b);
  void DrawTrackParams(PndTrack* track);
  void DrawCombiLines(vector<vector< TVector2> > ZPhiPairVector);
  void DrawCombiLinesResult(vector< TVector2> ZPhiPairVector);

  vector<vector<PndSttHit> > MoveSkewedHitsToCircle(TVector2 circle, Double_t circlerad, vector<PndSttHit*> skewhits);
  vector<vector<PndSttHit> > MoveSkewedHitsToCircle2(TVector2 circle, Double_t circlerad, vector<PndSttHit*> skewhits);

  vector<TVector2> HoughTrueIsoFinder(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams);
  vector<TVector2> HoughTrueIsoFinder2(vector<vector< TVector2> > ZPhiPairVector, TH2D *HoughSpace, TVector2 &lineparams);

  vector<TVector2> LineCombiIsoFinder(vector<vector< TVector2> > ZPhiPairVector);
  vector<TVector2> LineCombiIsoFinder2(vector<vector< TVector2> > ZPhiPairVector);

  void NestedFor(vector<int> &times, vector<int> &current, pair<vector<int>,double> &best, double sum, vector<vector<pair<vector<int>,double> > > matrix, int depth);

  TVector2 PzLineFitExtract(vector< TVector2> ZPhi);
  void CreatePndTrack(Double_t tanalpha);

  Int_t ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2, double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2);
  TVector2 ComputeTangentInPoint(double xc, double yc, TVector2 point);

};

#endif /* PNDSTTCELLTRACKINGTESTTASK_H_ */
