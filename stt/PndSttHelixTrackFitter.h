/** PndSttTrackFitter
 *@author R.Castelijns <r.castelijns@fz-juelich.de>
 **
 ** Abstract base class for concrete STT track fitting algorithm.
 ** Each derived class must implement the method DoFit. This has
 ** to operate on an object of type PndSttTrack and fill the
 ** parameters fPidHypo, fParamFirst, fParamLast, fFlag and fChi2.
 **/

#ifndef PNDSTTHELIXTRACKFITTER
#define PNDSTTHELIXTRACKFITTER 1

#include "PndSttTrack.h"
//#include "PndSttHit.h"
//#include "PndSttPoint.h"
//#include "FairTrackParam.h"
#include "PndSttTrackFitter.h"

//#include "TH2F.h"
//#include "TCanvas.h"
#include "TList.h"

#include <map>

class FairTrackParam;
class PndSttHit;
//class PndSttTrackFitter;
class TCanvas;
class TH2F;

void fcnHelix(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
void fcnHelix2(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

class PndSttHelixTrackFitter : public PndSttTrackFitter
{
 private:
    Int_t fEventCounter;

  PndSttTrack* fTrack;
  PndSttTrack currentTrack;

  TClonesArray* fHitArray;
  TClonesArray* fPointArray;
  TClonesArray *fHotArray;
  TObjArray *ZPointsArray;
  TCanvas *eventCanvas;
  TCanvas *eventCanvas2;
  TH2F *h1;
  TH2F *h2;
  Bool_t rootoutput;
 Int_t fVerbose;

 /** Private method AddHOT
  **
  ** Adds a PndSttHOT the HOTCollection
  **/
/*  PndSttHOT* AddHOT(Double_t x, Double_t y, Double_t x, Int_t hitindex, Int_t pointindex, Int_t trackindex); */
 
 public:
 PndSttHelixTrackFitter();
  PndSttHelixTrackFitter(Int_t verbose);
  ~PndSttHelixTrackFitter();
  void Init();

  // xy plane ----------------------
  // intersection finder 
  Bool_t IntersectionFinder(PndSttTrack *pTrack, FairTrackParam *par);  
  Bool_t IntersectionFinder4b(PndSttTrack *pTrack, FairTrackParam *par); 
  // fit
  Int_t Fit4(PndSttTrack* pTrack, Int_t pidHypo); 
  Int_t Fit4b(PndSttTrack* pTrack, Int_t pidHypo);
  Int_t MinuitFit(PndSttTrack* pTrack, Int_t pidHypo);

  // z track length plane ----------
  // zfinder
  Bool_t ZFinder(PndSttTrack* pTrack, Int_t pidHypo);
  Bool_t ZFinder2(PndSttTrack* pTrack, Int_t pidHypo);
  Bool_t ZFinderbb3(PndSttTrack* pTrack, Int_t pidHypo); 
   Bool_t ZFinder6b(PndSttTrack* pTrack, Int_t pidHypo); 
  // hough
  void Hough(TVector3* choice, Double_t Phi0, Double_t x0, Double_t y0, Double_t R);
  TVector3 GetHoughResponse();
  // zfit
  Int_t Zfit(PndSttTrack* pTrack, Int_t pidHypo);
  Int_t Zfit2(PndSttTrack* pTrack, Int_t pidHypo);
  Int_t Zfitbb2(PndSttTrack* pTrack, Int_t pidHypo);
  
  Int_t DoFit(PndSttTrack* pTrack, Int_t pidHypo = 211);

  /*   Int_t AddHitOnTrack(PndSttTrack *pTrack); */

  // track length calculation
  Double_t CalculateScosl(Double_t h, Double_t d0,  Double_t phi0, Double_t R, Double_t x, Double_t y);
  // find the tri-momentum in the PCA to a point
  // TVector3* MomentumAtPoint(PndSttTrack *pTrack, TVector2 *point);
  TVector3* MomentumAtPoint(PndSttTrack *pTrack, TVector3 *point);
  // find the PCA to a point
  //  TVector2* PCAToPoint(PndSttTrack *pTrack, TVector2 *point);
  TVector3* PCAToPoint(PndSttTrack *pTrack, TVector3 *point);

  // charge reconstruction from xy fit
  Int_t GetCharge(Double_t dCenter, Double_t phiCenter, Double_t radius);
  void OrderHitsByR(std::map <Double_t, Int_t> &hitMap);
  Double_t GetHitAngle(Int_t hitNo, Double_t dCenter, Double_t phiCenter, Double_t radius);

  PndSttHit* GetHitFromCollections(Int_t hitCounter) const;
  TList fHitCollectionList; 
  virtual void AddHitCollection(TClonesArray* mHitArray) {fHitCollectionList.Add(mHitArray); }


  virtual void Extrapolate( PndSttTrack* track, Double_t r, 
			    FairTrackParam *param );

  // marray reset
  void ResetMArray();
 
  PndSttTrack* GetTrack() const { return fTrack; };
  TClonesArray* GetHitArray() const { return fHitArray; };
  /*   TClonesArray* GetHOTCollection() const { return fHotArray; }; */
  Double_t refAngle;
  ClassDef(PndSttHelixTrackFitter,1);
};

#endif
