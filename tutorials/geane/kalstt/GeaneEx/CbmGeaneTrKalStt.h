#ifndef CBMGEANETRKALSTT_H
#define CBMGEANETRKALSTT_H 1



#include "FairTask.h"
#include "TVector3.h"
#include "TFile.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"

class TGeant3;
class TClonesArray;
class TTree;
class CbmPlanePoint;
class PndSttTrack;
class PndSttHit;
class PndSttPoint;

class FairGeaneTrKalStt : public FairTask
{
 public:
  /** Default constructor **/  
  FairGeaneTrKalStt();

  /** Destructor **/
  ~FairGeaneTrKalStt();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  Bool_t CoordSDToMARS(TVector3 o, TVector3 y, TVector3 z, TMatrixT<double> coor, TVector3 &coordinate);

  Bool_t ProcessHit(PndSttTrack *pTrack, Int_t k, FairTrackParP *fRunningStart, FairTrackParP *fRunningRes, TString fb);
  Bool_t Propagation(PndSttHit *currenthit, FairTrackParP *fRunningStart, FairTrackParP *fRunningRes, TString fb);
  Bool_t Kalman(PndSttHit *currenthit, FairTrackParP *fRunningRes, FairTrackParP *fRunningStart);
  Bool_t RetrieveVertex(PndSttTrack *pTrack);
  Bool_t BackToVertex(FairTrackParP *fRunningRes, FairTrackParP *fRes);
 Bool_t BackToVertex2(FairTrackParP *fRunningRes, FairTrackParP *fRes);
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
  ClassDef(FairGeaneTrKalStt,1);
  Int_t fEvent;
  FairGeanePro *fPro;
  FairGeaneUtil *fUtil;

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
