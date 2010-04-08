
/** PndMvdHybridHitProducer.h
 **
 **/

#ifndef PNDMVDHYBRIDHITPRODUCER_H
#define PNDMVDHYBRIDHITPRODUCER_H

#include "FairTask.h"
#include "PndMvdPixelDigiPar.h"
#include "PndMvdMCPoint.h"
#include "PndMvdPixel.h"
#include "PndMvdDigiPixel.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"

#include "PndGeoHandling.h"
#include "PndMvdCalcTot.h"

#include <string>
#include <vector>

class TClonesArray;

class PndMvdHybridHitProducer : public FairTask
{
 public:

  /** Default constructor **/
 PndMvdHybridHitProducer();

  PndMvdHybridHitProducer(Double_t lx, Double_t ly, Double_t threshold, Double_t noise, Double_t raisingtime = 100., Double_t fallingratio = 60., Double_t clockfrequency = 50.);

  /** Destructor **/
  virtual ~PndMvdHybridHitProducer();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};


 private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file

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
  void GetLocalHitPoints(PndMvdMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
//  PndMvdHit CalcGlobalPoint(std::vector<PndMvdPixel> pixels);
  TVector3 GetSensorDimensions(std::string detName);

  void AddHit(PndMvdPixel& hit, int mcIndex);
  void AddHits(std::vector<PndMvdPixel>* hitList, int mcIndex);

  Double_t flx;				//pixel widh in x;
  Double_t fly;				//pixel width in y;
  Double_t fthreshold; 		//pixel threshold in electrons
  Double_t fnoise; 			//pixel noise in electrons
  Int_t    fcols; 			//pixel columns in one FE
  Int_t    frows; 			//pixel rows in one FE
  Int_t fPixelHits;
  Double_t fRaisingTime; 	//time constant until the capacitor is loaded in ns
  Double_t fFallingRatio; 	//ratio the capacitor is unloaded in e/ns
  Double_t fClockFrequency; //clock frequency in MHz
  PndGeoHandling* fGeoH;
  Int_t fEventNr;
  Bool_t fOverwriteParams;

  std::vector<PndMvdPixel> fPixelList;
  PndMvdCalcTot* TotCalc;
  ClassDef(PndMvdHybridHitProducer,7);

};

#endif
