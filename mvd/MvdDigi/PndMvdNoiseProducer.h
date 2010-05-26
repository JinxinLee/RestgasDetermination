// -------------------------------------------------------------------------
// -----                PndMvdNoiseProducer header file                -----
// -----                  Created 01.07.08  by R.Kliemt                -----
// -------------------------------------------------------------------------


/** PndMvdNoiseProducer.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The Noise Producer adds fake hits to silicon sensor channels (strips and
 ** Pixels)
 **/


#ifndef PNDMVDNOISEPRODUCER_H
#define PNDMVDNOISEPRODUCER_H

#include "FairTask.h"

#include <vector>

#include "TRandom.h"
//#include "PndMvdGeoPar.h"
#include "PndGeoHandling.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "PndSdsChargeConversion.h"
#include "TString.h"

class TClonesArray;

class PndMvdNoiseProducer : public FairTask
{
 public:

  /** Default constructor **/
  PndMvdNoiseProducer();


  /** Destructor **/
  virtual ~PndMvdNoiseProducer();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  Double_t CalcDistFraction(Double_t spread, Double_t threshold);
//   Int_t CalcChanWhite(Int_t chanleft, Double_t frac);
  Int_t CalcChargeAboveThreshold(Double_t spread, Double_t threshold);
  void AddDigiStrip(Int_t &iStrip, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge);
  void AddDigiPixel(Int_t &noisies, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge);

  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};

  void FillSensorLists();

  void DiveDownNode(TGeoNode *fN);
//   void Finish();
 private:
  Bool_t fPersistance;
  TString fBranchName;
  /** In-Output array of PndSdsDigis **/
  TClonesArray* fDigiStripArray;
  TClonesArray* fDigiPixelArray;

  /** Parameter Containers **/
  PndSdsStripDigiPar* fDigiParRect;
  PndSdsStripDigiPar* fDigiParTrap;
  PndSdsPixelDigiPar* fDigiParPix;

  PndSdsTotDigiPar* fTotDigiParRect;
  PndSdsTotDigiPar* fTotDigiParTrap;
  PndSdsTotDigiPar* fTotDigiParPix;

//  PndMvdGeoPar* fGeoPar;

  PndGeoHandling* fGeoH;      //! Geometry name handling

  std::vector<Int_t> fPixelIds4;
  std::vector<Int_t> fPixelIds6;
  std::vector<Int_t> fPixelIds8;
  std::vector<Int_t> fPixelIds12;
  std::vector<Int_t> fStripRectIds;
  std::vector<Int_t> fStripTrapIds;

  PndSdsChargeConversion* fStripRectChargeConv;
  PndSdsChargeConversion* fStripTrapChargeConv;
  PndSdsChargeConversion* fCurrentChargeConv;
  PndSdsChargeConversion* fPixChargeConv;

  Int_t fNoiseSpread;
  Int_t fThreshold;

//   void Register();

//   void Reset();

//   void ProduceHits();


  ClassDef(PndMvdNoiseProducer,1);

};

#endif
