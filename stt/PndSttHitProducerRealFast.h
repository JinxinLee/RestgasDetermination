// Fast Digitization

#ifndef PNDSTTHITPRODUCERREALFAST_H
#define PNDSTTHITPRODUCERREALFAST_H 1


#include "CbmTask.h"

//#include "PndSttHit.h"
//#include "PndSttHitInfo.h"

#include "TVector3.h"

class PndSttHit;
class PndSttHitInfo;
class TClonesArray;
class TObjectArray;

class PndSttHitProducerRealFast : public CbmTask
{

 public:

  /** Default constructor **/  
  PndSttHitProducerRealFast();


  /** Destructor **/
  ~PndSttHitProducerRealFast();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndSttHit* AddHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t iPoint, Int_t trackID, Double_t p, Double_t rsim, Double_t rtrue, Double_t closestDistanceError, TVector3 wireDirection, Double_t halflength, Double_t depcharge, Double_t dedx);

  PndSttHitInfo* AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);
  
  Double_t GetError(Double_t);
  //  void WriteHistograms();
  Double_t TruncatedMean(Double_t vec[], Double_t perc, Int_t totalnum);

  private: 

  /** Input array of PndSttPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndSttHits **/
  TClonesArray* fHitArray;  

  TObjArray *fVolumeArray;

   /** Output array of PndSttHitInfo **/
  TClonesArray* fHitInfoArray;
  Int_t fevtn;

  ClassDef(PndSttHitProducerRealFast,1);

};

#endif
