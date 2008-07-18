#ifndef LHE_TRACK_CUTS_H
#define LHE_TRACK_CUTS_H

// *************************************************************************
//  Author: Oleg Rogachevsky             e-mail: rogach@sunhe.jinr.ru
//   
//  track cuts and simple fitting
//
// Created: 1-07-07
// Modified:
//
// *************************************************************************

#include "TArrayD.h"

#include "PndTpcLheCMTrack.h"
#include "PndTpcLheCMPoint.h"
#include "lhe.h"

class PndTpcLheTrackCuts {

protected:

  // Cuts

  Int_t fTrackMinPoints;       // minimum number of points on one track

  struct Cuts{
    Double_t fDelX;           //  delX from predictor
    Double_t fDelY;           //  delY from predictor

    Double_t fBendChi2;       // upper cut fo parabolic fitting
    Double_t fDeepChi2;       // cut for deep angle scattering
    Double_t fMaxHitDist;     // cut for max dist between 1st and 2nd hits
    Double_t fBetaMin;        // min dev of bending angle from predictor
  };

  Cuts fTrackCuts[2];


  static PndTpcLheTrackCuts*   fInstance;  // Cuts instance
 
public:

  PndTpcLheTrackCuts();               // default constructor
  virtual ~PndTpcLheTrackCuts();      // destructor
  static PndTpcLheTrackCuts* Instance();
  
  // Setters
  void Reset();

  void SetHighMomTrackCuts();
  void SetLowMomTrackCuts();

  // Getters

  Double_t GetChi2Bend(Int_t pl);
  Double_t GetChi2Deep(Int_t pl);
  Double_t GetDelX(Int_t pl);
  Double_t GetDelY(Int_t pl);

  // ======================

  Bool_t IsGoodGeantTrack(PndTpcLheTrack *track);
  Bool_t IsGoodFoundTrack(PndTpcLheCMTrack* track);

  void CMLineFit(PndTpcLheCMTrack *track, Double_t *a);
  void DeepAngleFit( const PndTpcLheCMTrack *track, Double_t *a);

  void LineFit(TArrayD *x, TArrayD *delx,
	       TArrayD *y, TArrayD *dely,
	       Double_t *a, Int_t np);
  Double_t const DistFromLine(Double_t Xh, Double_t Yh, Double_t *coeff);

  Double_t GetThetPrediction(PndTpcLheCMTrack *track, Double_t z, Bool_t b);
  Double_t GetPhiPrediction(PndTpcLheCMTrack *track);

  Bool_t VerifyTrack(PndTpcLheCMTrack *track, PndTpcLheCMPoint *hit, Bool_t back);

  //  void PredictAngles(PndTpcLheCMTrack *tr, Bool_t bw);
  Double_t TrackHitAlpha(PndTpcLheCMTrack *t1, PndTpcLheCMPoint *hit, Bool_t bw);
  Double_t TrackHitCircleDist(PndTpcLheCMTrack *t, PndTpcLheCMPoint *h, Bool_t b);


  void Circle3pnts(Double_t x[],Double_t y[], Double_t r[]);
  int Circle_Circle_Intersection(double x0, double y0, double r0,
				 double x1, double y1, double r1,
				 double *xi, double *yi,
				 double *xi_prime, double *yi_prime);
  Double_t GetMaxDist(Int_t pl);

  ClassDef(PndTpcLheTrackCuts, 1)  //
    
  };

//________________________________________________________________
inline Double_t PndTpcLheTrackCuts::GetMaxDist(Int_t pl) {
  //---
  return fTrackCuts[pl].fMaxHitDist; 
}

#endif
