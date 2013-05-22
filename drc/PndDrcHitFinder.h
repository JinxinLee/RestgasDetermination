
/** PndDrcHitFinder.h
 **
 **/

#ifndef PNDDRCHITFINDER_H
#define PNDDRCHITFINDER_H

#include "PndDrcDigi.h"

#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "FairMCEventHeader.h"
#include "TVector3.h"
//#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndGeoHandling.h"

#include "PndGeoDrc.h"
#include "PndDrcPDHit.h"

#include <string>
#include <vector>

class TClonesArray;

class PndDrcHitFinder : public FairTask
{
 public:

  /** Default constructor **/
  PndDrcHitFinder();
  
  /** Named constructor **/
  PndDrcHitFinder(const char* name);

  /** Destructor **/
  virtual ~PndDrcHitFinder();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishEvent();
  virtual void FinishTask();
  
protected:
    
  Bool_t fDigiPixelMCInfo; // switch to turn on/off storing additional MC Info of Digis
  
  /** Input array of PndDrcDigis **/
  TClonesArray* fDigiArray;

  /** Output array of PndDrcPDHits **/
  TClonesArray* fPdHitArray;
   
  FairMCEventHeader* fMCEventHeader;

  void Register();
  void Reset();
  void ProduceHits();

  PndDrcPDHit* AddDrcPDHit(Int_t iDetectorId, TVector3 posHit, TVector3 dPosHit, Double_t time, Double_t timeThreshold, Int_t index);
  Double_t fPixelSize;	//pixel size;
  Double_t fPixelGap;   // gap betwen neighboring pixels
  Double_t fPixelStep;
  Double_t fMcpActiveArea; // dimension of the active area of one MCP
  Int_t    fNpix; //pixel rows in one one MCP
  Int_t fPixelHits;
  PndGeoHandling* fGeoH; //!
  PndGeoDrc*  fGeo;
  Int_t fEventNr;

  std::vector<PndDrcPDHit> fPDHitList;

  ClassDef(PndDrcHitFinder,1);

};

#endif
