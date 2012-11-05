// Fast Digitization

#ifndef PNDFTSHITPRODUCERREALFAST_H
#define PNDFTSHITPRODUCERREALFAST_H 1


#include "FairTask.h"
#include "PndGeoFtsPar.h"
#include "TVector3.h"

class PndFtsHit;
class PndFtsHitInfo;
class TClonesArray;
class TObjectArray;

class PndFtsHitProducerRealFast : public FairTask
{

 public:

  /** Default constructor **/  
  PndFtsHitProducerRealFast();


  /** Destructor **/
  ~PndFtsHitProducerRealFast();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndFtsHit* AddHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t iPoint, TVector3& pos, TVector3& dpos, Double_t p, Double_t rsim, Double_t closestDistanceError, Double_t depcharge);

  PndFtsHitInfo* AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake);

  void FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos);
  
  Double_t GetError(Double_t);

 
  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  private: 

  void SetParContainers();


  /** Input array of PndFtsPoints **/
  TClonesArray* fPointArray; 

  /** Output array of PndFtsHits **/
  TClonesArray* fHitArray;   

  TObjArray *fVolumeArray;   

  /** Output array of PndFtsHitInfo **/
  TClonesArray* fHitInfoArray;
  Int_t fevtn;

  PndGeoFtsPar*  fFtsParameters;
  
  /** object persistence **/
  Bool_t  fPersistence; //!


  TClonesArray* fTubeArray;


  PndFtsHitProducerRealFast(const  PndFtsHitProducerRealFast& L);
  PndFtsHitProducerRealFast& operator= (const  PndFtsHitProducerRealFast&) {return *this;}
  
  ClassDef(PndFtsHitProducerRealFast,1);

};

#endif
