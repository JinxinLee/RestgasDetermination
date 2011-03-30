
/** PndSdsHybridHitProducer.h
 **
 **/

#ifndef PNDSDSHYBRIDHITPRODUCER_H
#define PNDSDSHYBRIDHITPRODUCER_H

#include "PndSdsTask.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsMCPoint.h"
#include "PndSdsPixel.h"
#include "PndSdsDigiPixel.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "FairMCEventHeader.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndGeoHandling.h"
#include "PndSdsChargeConversion.h"
#include "PndSdsTotDigiPar.h"

#include <string>
#include <vector>

class TClonesArray;

class PndSdsHybridHitProducer : public PndSdsTask
{
 public:

  /** Default constructor **/
  PndSdsHybridHitProducer();
  
  /** Named constructor **/
  PndSdsHybridHitProducer(const char* name);

 PndSdsHybridHitProducer(Double_t lx, Double_t ly, Double_t threshold, Double_t noise);

  /** Destructor **/
  virtual ~PndSdsHybridHitProducer();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishEvent();
  
  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};
  
protected:
  
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  Bool_t fDigiPixelMCInfo; // switch to turn on/off storing additional MC Info of Digis
  

  /** Input array of PndSdsMCPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndSdsDigis **/
  TClonesArray* fPixelArray;

  /** Output array of PndSdsDigiPixelMCInfo **/
  TClonesArray* fPixelMCArray;


  PndSdsPixelDigiPar* fDigiPar;
  PndSdsTotDigiPar* fTotDigiPar;
  PndSdsChargeConversion* fChargeConverter;
  FairMCEventHeader* fMCEventHeader;
 // fDetectorType fMCPointType;

  void Register();
  void Reset();
  void ProduceHits();

  TGeoHMatrix GetTransformation (Int_t sensorID);
  void GetLocalHitPoints(PndSdsMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
//  PndSdsHit CalcGlobalPoint(std::vector<PndSdsPixel> pixels);
  TVector3 GetSensorDimensions(Int_t sensorID);

  void AddHit(PndSdsPixel& hit, int mcIndex);
  void AddHits(std::vector<PndSdsPixel>* hitList, int mcIndex);

  Double_t flx;	//pixel widh in x;
  Double_t fly;	//pixel width in y;
  Double_t fthreshold; //pixel threshold in electrons
  Double_t fnoise; //pixel noise in electrons
  Int_t    fcols; //pixel columns in one FE
  Int_t    frows; //pixel rows in one FE
  Int_t fPixelHits;
  PndGeoHandling* fGeoH;
  Int_t fEventNr;
  Bool_t fOverwriteParams;

  std::vector<PndSdsPixel> fPixelList;

  ClassDef(PndSdsHybridHitProducer,7);

};

#endif
