
/** PndMvdDigiTask.h
 **
 **/


#ifndef PNDMVDDIGITASK_H
#define PNDMVDDIGITASK_H 

#include "CbmTask.h"
#include "PndMvdGeoPar.h"
#include "PndMvdMCPoint.h"
#include "PndMvdPixel.h"
#include "PndMvdDigiPixel.h"
#include "PndMvdDigiStrip.h"
#include "CbmGeoVector.h"
#include "CbmGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndMvdDigiTask : public CbmTask
{
 public:

    /** Default constructor **/  
    PndMvdDigiTask();
  

    PndMvdDigiTask(Double_t pixelX, Double_t pixelY,
                Double_t topPitch, Double_t botPitch,
                Double_t ori, Double_t skew,
                TVector2 topAnchor, TVector2 botAnchor,
                Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                Double_t pixelfThreshold, Double_t pixelfNoise,
                Double_t stripfThreshold, Double_t stripfNoise);
                      
    /** Destructor **/
    virtual ~PndMvdDigiTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);


 private:
  
    PndMvdHybridHitProducer* fPixelProducer;
    PndMvdStripHitProducer* fStripProducer;
    
    
    TString fBranchName;

  PndMvdGeoPar* fGeoPar;
  
  void Register();
  void Reset();  
  void ProduceHits();
  
//   TGeoHMatrix GetTransformation (std::string detName);
//   void GetLocalHitPoints(PndMvdMCPoint* myPoint, CbmGeoVector& myHitIn, CbmGeoVector& myHitOut);
//   PndMvdHit CalcGlobalPoint(std::vector<PndMvdPixel> pixels);
//   TVector3 GetSensorDimensions(std::string detName);  
  Double_t flx;	//pixel widh in x;
  Double_t fly;	//pixel width in y;
  Double_t fthreshold; //pixel threshold in electrons
  Double_t fnoise; //pixel noise in electrons
  Double_t fSlx;	//strip widh in x;
  Double_t fSly;	//strip width in y;
  Double_t fSthreshold; //strip threshold in electrons
  Double_t fSnoise; //strip noise in electrons
  Int_t stripHits;
  Int_t pixelHits;
  ClassDef(PndMvdDigiTask,5);

};

#endif /* MVDDIGITASK_H */
