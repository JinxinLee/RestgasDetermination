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
class PndSttHitWriteoutBuffer;

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

  PndSttHit* AddHit(Int_t detID, Int_t tubeID, Int_t iPoint, TVector3& pos, TVector3& dpos, Double_t p, Double_t rsim, Double_t closestDistanceError, Double_t depcharge, Double_t timeOfFlight);

  PndSttHitInfo* AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  void SetParContainers();

  void RunTimeBased(){fTimeOrderedDigi = kTRUE;}

 private: 

  /** Input array of PndSttPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndSttHits **/
  TClonesArray* fHitArray;
  PndSttHitWriteoutBuffer* fDataBuffer;

  /** Output array of PndSttHitInfo **/
  TClonesArray* fHitInfoArray;

  /** from parameters array of PndSttTube **/  //  CHECK added
  TClonesArray* fTubeArray;

  /** object persistence **/
  Bool_t  fPersistence; //!
  Bool_t fTimeOrderedDigi;

  PndGeoSttPar *fSttParameters;  //  CHECK added

  ClassDef(PndSttHitProducerRealFull,1);

};

#endif
