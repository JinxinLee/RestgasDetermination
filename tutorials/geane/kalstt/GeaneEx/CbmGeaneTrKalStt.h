#ifndef CBMGEANETRKALSTT_H
#define CBMGEANETRKALSTT_H 1



#include "CbmTask.h"
#include "TVector3.h"
#include "TFile.h"
#include "CbmGeanePro.h"
#include "CbmGeaneUtil.h"

class TGeant3;
class TClonesArray;
class TTree;
class CbmPlanePoint;
class PndSttTrack;
class PndSttHit;
class PndSttPoint;

class CbmGeaneTrKalStt : public CbmTask
{
 public:
  /** Default constructor **/  
  CbmGeaneTrKalStt();

  /** Destructor **/
  ~CbmGeaneTrKalStt();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  Bool_t CoordSDToMARS(TVector3 o, TVector3 y, TVector3 z, TMatrixT<double> coor, TVector3 &coordinate);

  Bool_t ProcessHit(PndSttTrack *pTrack, Int_t k, CbmTrackParP *fRunningStart, CbmTrackParP *fRunningRes, TString fb);
  Bool_t Propagation(PndSttHit *currenthit, CbmTrackParP *fRunningStart, CbmTrackParP *fRunningRes, TString fb);
  Bool_t Kalman(PndSttHit *currenthit, CbmTrackParP *fRunningRes, CbmTrackParP *fRunningStart);
  Bool_t RetrieveVertex(PndSttTrack *pTrack);
  Bool_t BackToVertex(CbmTrackParP *fRunningRes, CbmTrackParP *fRes);
 Bool_t BackToVertex2(CbmTrackParP *fRunningRes, CbmTrackParP *fRes);
 void FinishTask();

 private:
  /** Input array of Points **/
  TClonesArray* fHitArray;
  TClonesArray* fPointArray;
  TClonesArray* fTrackArray;

  TTree *t;
  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fTrackParFinal;  

  TGeant3 *gMC3;
  ClassDef(CbmGeaneTrKalStt,1);
  Int_t fEvent;
  CbmGeanePro *fPro;
  CbmGeaneUtil *fUtil;

  Int_t PDGCode; //!
  TVector3 StartPos;     //!
  TVector3 StartPosErr;  //!
  TVector3 StartMom;     //!
  TVector3 StartMomErr;  //!

  Int_t welldone; 
  Int_t notdone; 
  Int_t notconsidered; 
  Int_t notbackprocessedhit; 
  Int_t notprocessedhit; 
  Int_t total;
  Int_t tothits;

};

#endif
