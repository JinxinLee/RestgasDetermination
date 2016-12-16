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
#include "FairMCEventHeader.h"

#include <vector>

#include "TRandom.h"
//#include "PndMvdGeoPar.h"
#include "PndGeoHandling.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "PndSdsChargeConversion.h"
#include "PndSdsDigiPixelWriteoutBuffer.h"
#include "PndSdsDigiStripWriteoutBuffer.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsFE.h"
#include "TString.h"

class TClonesArray;

class PndMvdNoiseProducer : public FairTask
{
 public:

  /** Default constructor **/
  PndMvdNoiseProducer();

  /** Destructor **/
  virtual ~PndMvdNoiseProducer();

  PndMvdNoiseProducer(const PndMvdNoiseProducer& o) :
    fPersistance(o.fPersistance),
    fTimeOrderedDigi(o.fTimeOrderedDigi),
    fBranchName(o.fBranchName),
    fDigiStripArray(o.fDigiStripArray),
    fDigiPixelArray(o.fDigiPixelArray),
    fDigiPixelBuffer(o.fDigiPixelBuffer),
    fDigiStripBuffer(o.fDigiStripBuffer),
    fDigiParRect(o.fDigiParRect),
    fDigiParTrap(o.fDigiParTrap),
    fDigiParPix(o.fDigiParPix),
    fTotDigiParRect(o.fTotDigiParRect),
    fTotDigiParTrap(o.fTotDigiParTrap),
    fTotDigiParPix(o.fTotDigiParPix),
    fGeoH(o.fGeoH),
    fMCEventheader(o.fMCEventheader),
    fPixelIds2(o.fPixelIds2),
    fPixelIds4(o.fPixelIds4),
    fPixelIds5(o.fPixelIds5),
    fPixelIds6(o.fPixelIds6),
    fStripRectLIds(o.fStripRectLIds),
    fStripRectSIds(o.fStripRectSIds),
    fStripTrapIds(o.fStripTrapIds),
    fStripRectChargeConv(o.fStripRectChargeConv),
    fStripTrapChargeConv(o.fStripTrapChargeConv),
    fCurrentChargeConv(o.fCurrentChargeConv),
    fPixChargeConv(o.fPixChargeConv),
    fNoiseSpread(o.fNoiseSpread),
    fThreshold(o.fThreshold),
    fPreviousTime(o.fPreviousTime)
  {};
  PndMvdNoiseProducer& operator=(const PndMvdNoiseProducer& o)
  {
    fPersistance=o.fPersistance;
    fTimeOrderedDigi=o.fTimeOrderedDigi;
    fBranchName=o.fBranchName;
    fDigiStripArray=o.fDigiStripArray;
    fDigiPixelArray=o.fDigiPixelArray;
    fDigiPixelBuffer=o.fDigiPixelBuffer;
    fDigiStripBuffer=o.fDigiStripBuffer;
    fDigiParRect=o.fDigiParRect;
    fDigiParTrap=o.fDigiParTrap;
    fDigiParPix=o.fDigiParPix;
    fTotDigiParRect=o.fTotDigiParRect;
    fTotDigiParTrap=o.fTotDigiParTrap;
    fTotDigiParPix=o.fTotDigiParPix;
    fGeoH=o.fGeoH;
    fMCEventheader=o.fMCEventheader;
    fPixelIds2=o.fPixelIds2;
    fPixelIds4=o.fPixelIds4;
    fPixelIds5=o.fPixelIds5;
    fPixelIds6=o.fPixelIds6;
    fStripRectLIds=o.fStripRectLIds;
    fStripRectSIds=o.fStripRectSIds;
    fStripTrapIds=o.fStripTrapIds;
    fStripRectChargeConv=o.fStripRectChargeConv;
    fStripTrapChargeConv=o.fStripTrapChargeConv;
    fCurrentChargeConv=o.fCurrentChargeConv;
    fPixChargeConv=o.fPixChargeConv;
    fNoiseSpread=o.fNoiseSpread;
    fThreshold=o.fThreshold;
    fPreviousTime=o.fPreviousTime;
    return *this;
  };


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  virtual void FinishEvent();

  Double_t CalcDistFraction(Double_t spread, Double_t threshold);
//   Int_t CalcChanWhite(Int_t chanleft, Double_t frac);
  Int_t CalcChargeAboveThreshold(Double_t spread, Double_t threshold);
  Double_t CalcReadoutCycles(Double_t clock);
  void AddDigiStrip(Int_t &iStrip, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge);
  void AddDigiPixel(Int_t &noisies, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge);

  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};

  void FillSensorLists();

  void RunTimeBased(){fTimeOrderedDigi = kTRUE;}

 protected:
  Bool_t fPersistance;

  Bool_t fTimeOrderedDigi; ///< parameter to switch to time ordered simulation

  TString fBranchName;
  /** In-Output array of PndSdsDigis **/
  TClonesArray* fDigiStripArray;
  TClonesArray* fDigiPixelArray;

  PndSdsDigiPixelWriteoutBuffer* fDigiPixelBuffer;
  PndSdsDigiStripWriteoutBuffer* fDigiStripBuffer;

  /** Parameter Containers **/
  PndSdsStripDigiPar* fDigiParRect;
  PndSdsStripDigiPar* fDigiParTrap;
  PndSdsPixelDigiPar* fDigiParPix;

  PndSdsTotDigiPar* fTotDigiParRect;
  PndSdsTotDigiPar* fTotDigiParTrap;
  PndSdsTotDigiPar* fTotDigiParPix;

  PndGeoHandling* fGeoH;      //! Geometry name handling
  FairMCEventHeader* fMCEventheader;

  std::vector<Int_t> fPixelIds2;
  std::vector<Int_t> fPixelIds4;
  std::vector<Int_t> fPixelIds5;
  std::vector<Int_t> fPixelIds6;
  std::vector<Int_t> fStripRectLIds;
  std::vector<Int_t> fStripRectSIds;
  std::vector<Int_t> fStripTrapIds;

  PndSdsChargeConversion* fStripRectChargeConv;
  PndSdsChargeConversion* fStripTrapChargeConv;
  PndSdsChargeConversion* fCurrentChargeConv;
  PndSdsChargeConversion* fPixChargeConv;

  Int_t fNoiseSpread;
  Int_t fThreshold;
  Double_t fPreviousTime;

//   void Register();

//   void Reset();

//   void ProduceHits();


  ClassDef(PndMvdNoiseProducer,1);

};

#endif
