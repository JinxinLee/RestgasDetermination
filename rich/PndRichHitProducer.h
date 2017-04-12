#ifndef PNDRICHHITPRODUCER_H
#define PNDRICHHITPRODUCER_H 1


#include "FairTask.h"
#include "PndRichDigi.h"
#include "PndRichHit.h"
#include "PndRichGeo.h"
#include "PndRichPDHit.h"
//#include "PndRichTSPDHit.h"
#include "PndRichResolution.h"
#include "TVector3.h"

class TClonesArray;
class PndRichHitWriteoutBuffer;

class PndRichHitProducer : public FairTask
{

 public:

  /** Default constructor **/  
  PndRichHitProducer();


  /** Destructor **/
  ~PndRichHitProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void AddXPDHit(Int_t detID, Int_t sensorId, TVector3& pos, TVector3& dpos, Int_t index, Double_t time );
  PndRichDigi* AddDigi(Int_t detID, Int_t sensorId, TVector3& pos, TVector3& dpos, Int_t index, Double_t time );
  PndRichPDHit* AddPDHit(Int_t detID, Int_t sensorId, TVector3& pos, TVector3& dpos, Int_t index, Double_t time );
  PndRichHit* AddHit(Int_t detID, Int_t sensorId, TVector3& pos, TVector3& dpos,
                     Double_t thetaC, Double_t errThetaC, Int_t index);

  void SetPositionSmearing(Float_t res) { fPosResolution = res; };

  void SetGeoVersion(UInt_t version) { fGeoVersion = version; };
  
  void SetPhDetNoiseON(UInt_t phDetNoise) { fPhDetNoise = phDetNoise; };

  void FinishEvent();
  void FinishTask();
  void SetPersistency(Bool_t v = kTRUE) { fPersistency = v; }

  void RunTimeBased(){fTimeOrderedDigi = kTRUE;}  

 private: 

  PndRichGeo* fGeo;
  UInt_t fGeoVersion;
  Bool_t fPhDetNoise;
  UInt_t fNumRand;
  Bool_t fPersistency;
  Bool_t fTimeOrderedDigi;
  Double_t fDeadTime;
  Double_t fEventTime;
  Double_t fPreviousEventTime;
  
  Float_t fPosResolution;                    // Position smearing [cm]
  PndRichResolution* fRichResolution;
  
  /** Input array of PndRichPoint **/
  TClonesArray* fPDPointArray;
  TClonesArray* fBarPointArray;

  /** Output array of PndRichHit **/
  TClonesArray* fPDHitArray;
  TClonesArray* fHitArray;

  std::vector<Double_t> PhDetNoise();

  PndRichHitWriteoutBuffer* fDataBuffer;
   
  ClassDef(PndRichHitProducer,1);

};

#endif
