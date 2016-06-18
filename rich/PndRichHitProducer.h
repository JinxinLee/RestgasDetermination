#ifndef PNDRICHHITPRODUCER_H
#define PNDRICHHITPRODUCER_H 1


#include "FairTask.h"
#include "PndRichHit.h"
#include "PndRichGeo.h"
#include "PndRichPDHit.h"
#include "TVector3.h"

class TClonesArray;

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

  PndRichPDHit* AddPDHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index, Double_t time );
  PndRichHit* AddHit(Int_t detID, Int_t sensorId, TVector3& pos, TVector3& dpos,
                     Double_t thetaC, Double_t errThetaC, Int_t index);

  void SetPositionSmearing(Float_t res) { fPosResolution = res; };

  void SetGeoVersion(UInt_t version) { fGeoVersion = version; };
  
  void SetPhDetNoiseON(UInt_t phDetNoise) { fPhDetNoise = phDetNoise; };

  void FinishEvent();
  void FinishTask();
  void SetPersistency(Bool_t v = kTRUE) { fPersistency = v; }

 private: 

  PndRichGeo* fGeo;
  UInt_t fGeoVersion;
  Bool_t fPhDetNoise;
  UInt_t fNumRand;
  Bool_t fPersistency;
  
  Float_t fPosResolution;                    // Position smearing [cm]
  
  /** Input array of PndRichPoint **/
  TClonesArray* fPDPointArray;
  TClonesArray* fBarPointArray;

  /** Output array of PndRichHit **/
  TClonesArray* fPDHitArray;
  TClonesArray* fHitArray;

    std::vector<Double_t> fWlPhoton;
    std::vector<Double_t> fPDE;

    std::vector<Double_t> PhDetNoise();
   
  ClassDef(PndRichHitProducer,1);

};

#endif
