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
#include "PndMvdGeoHandling.h"
#include "PndMvdStripDigiPar.h"
#include "PndMvdPixelDigiPar.h"
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
  void AddDigiStrip(Int_t &iStrip, Int_t iPoint, TString detname, Int_t fe, Int_t chan, Double_t charge);
  void AddDigiPixel(Int_t &noisies, Int_t iPoint, TString detname, Int_t fe, Int_t col, Int_t row, Double_t charge);

  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};

  void DiveDownNode(TGeoNode *fN);
//   void Finish();
 private:
  Bool_t fPersistance;
  TString fBranchName;
  /** In-Output array of PndMvdDigis **/
  TClonesArray* fDigiStripArray;
  TClonesArray* fDigiPixelArray;

  /** Parameter Containers **/
  PndMvdStripDigiPar* fDigiParRect;
  PndMvdStripDigiPar* fDigiParTrap;
  PndMvdPixelDigiPar* fDigiParPix;

//  PndMvdGeoPar* fGeoPar;

  PndMvdGeoHandling* fGeoH;      //! Geometry name handling

  std::vector<TString> fPixelIds4;
  std::vector<TString> fPixelIds6;
  std::vector<TString> fPixelIds8;
  std::vector<TString> fPixelIds12;
  std::vector<TString> fStripRectIds;
  std::vector<TString> fStripTrapIds;


  Int_t fNoiseSpread;
  Int_t fThreshold;

//   void Register();

//   void Reset();

//   void ProduceHits();


  ClassDef(PndMvdNoiseProducer,1);

};

#endif
