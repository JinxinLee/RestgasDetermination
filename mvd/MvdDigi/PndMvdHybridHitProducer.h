
/** PndMvdHybridHitProducer.h
 **
 **/

#ifndef PNDMVDHYBRIDHITPRODUCER_H
#define PNDMVDHYBRIDHITPRODUCER_H 

#include "CbmTask.h"
#include "PndMvdPixelDigiPar.h"
#include "PndMvdMCPoint.h"
#include "PndMvdPixel.h"
#include "PndMvdDigiPixel.h"
#include "CbmGeoVector.h"
#include "CbmGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"

#include "PndMvdGeoHandling.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndMvdHybridHitProducer : public CbmTask
{
 public:

  /** Default constructor **/  
 PndMvdHybridHitProducer();
  
  PndMvdHybridHitProducer(Double_t lx, Double_t ly, Double_t threshold, Double_t noise);

  /** Destructor **/
  virtual ~PndMvdHybridHitProducer();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:
  
  TString fBranchName;
  /** Input array of PndMvdMCPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndMvdDigis **/
//  TClonesArray* fHitArray;  
  TClonesArray* fPixelArray;
//   TClonesArray* fFePixelArray;
  
  PndMvdPixelDigiPar* fDigiPar;
  
  void Register();
  void Reset();  
  void ProduceHits();
  
  TGeoHMatrix GetTransformation (std::string detName);
  void GetLocalHitPoints(PndMvdMCPoint* myPoint, CbmGeoVector& myHitIn, CbmGeoVector& myHitOut);
//  PndMvdHit CalcGlobalPoint(std::vector<PndMvdPixel> pixels);
  TVector3 GetSensorDimensions(std::string detName);
  
  void AddHit(PndMvdPixel& hit, int mcIndex);
  void AddHits(std::vector<PndMvdPixel>* hitList, int mcIndex);
  
  Double_t flx;	//pixel widh in x;
  Double_t fly;	//pixel width in y;
  Double_t fthreshold; //pixel threshold in electrons
  Double_t fnoise; //pixel noise in electrons
  Int_t    fcols; //pixel columns in one FE
  Int_t    frows; //pixel rows in one FE
  Int_t pixelHits;
  PndMvdGeoHandling* fGeoH;
  Int_t event_Nr;
  Bool_t fOverwriteParams;
  
  std::vector<PndMvdPixel> fPixelList;
  ClassDef(PndMvdHybridHitProducer,7);

};

#endif
