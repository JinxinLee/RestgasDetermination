
/** PndSdsHybridHitProducer.h
 **
 **/

#ifndef PNDSDSHYBRIDHITPRODUCER_H
#define PNDSDSHYBRIDHITPRODUCER_H

#include "FairTask.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsMCPoint.h"
#include "PndSdsPixel.h"
#include "PndSdsDigiPixel.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"

#include "PndGeoHandling.h"

#include <string>
#include <vector>

class TClonesArray;

class PndSdsHybridHitProducer : public FairTask
{
 public:

  /** Default constructor **/
 PndSdsHybridHitProducer();

  PndSdsHybridHitProducer(Double_t lx, Double_t ly, Double_t threshold, Double_t noise);

  /** Destructor **/
  virtual ~PndSdsHybridHitProducer();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** pure virtual method SetBranchNames
   **
   ** called by Init()
   ** function to set individual branch names
   **/
  virtual void SetBranchNames()=0;

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 protected:

  TString fBranchName;
  TString fOutBranchName;
  TString fFolderName;
  /** Input array of PndSdsMCPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndSdsDigis **/
//  TClonesArray* fHitArray;
  TClonesArray* fPixelArray;
//   TClonesArray* fFePixelArray;

  PndSdsPixelDigiPar* fDigiPar;

  void Register();
  void Reset();
  void ProduceHits();

  TGeoHMatrix GetTransformation (std::string detName);
  void GetLocalHitPoints(PndSdsMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
//  PndSdsHit CalcGlobalPoint(std::vector<PndSdsPixel> pixels);
  TVector3 GetSensorDimensions(std::string detName);

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
