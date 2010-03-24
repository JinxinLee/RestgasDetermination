// Complete Digitization

#ifndef PNDSTTHITPRODUCERREALFULL_H
#define PNDSTTHITPRODUCERREALFULL_H 1


#include "FairTask.h"
#include "PndGeoSttPar.h"

//#include "PndSttHit.h"
//#include "PndSttHitInfo.h"

#include "TVector3.h"

class PndSttHit;
class PndSttHitInfo;
class TClonesArray;
class TObjectArray;

class PndSttHitProducerRealFull : public FairTask
{

 public:

  /** Default constructor **/  
  PndSttHitProducerRealFull();


  /** Destructor **/
  ~PndSttHitProducerRealFull();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndSttHit* AddHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t iPoint, Int_t trackID, Double_t p, Double_t rsim, Double_t rtrue, Double_t closestDistanceError, TVector3 wireDirection, Double_t halflength, Double_t depcharge, Double_t dedx, Int_t tubeID);

  PndSttHitInfo* AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  void SetParContainers();

 private: 

  /** Input array of PndSttPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndSttHits **/
  TClonesArray* fHitArray;  

  /** Output array of PndSttHitInfo **/
  TClonesArray* fHitInfoArray;

  /** from parameters array of PndSttTube **/  //  CHECK added
  TClonesArray* fTubeArray;

  /** object persistence **/
  Bool_t  fPersistence; //!

  PndGeoSttPar *fSttParameters;  //  CHECK added

  ClassDef(PndSttHitProducerRealFull,1);

};

#endif
