// -------------------------------------------------------------------------
// -----                 CbmStsHitProducerIdel header file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** PndMvdNoiseProducer.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The Noise Producer adds fake hits to silicon sensor channels (strips and 
 ** Pixels)
 **/


#ifndef PNDMVDNOISEPRODUCER_H
#define PNDMVDNOISEPRODUCER_H 

#include "CbmTask.h"

#include <vector>

#include "TRandom.h"
#include "PndMvdGeoPar.h"
#include "PndMvdGeoHandling.h"
#include "PndMvdStripDigiPar.h"
#include "PndMvdPixelDigiPar.h"
 
class TClonesArray;

class PndMvdNoiseProducer : public CbmTask
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

  Int_t CalcChanWhite(Int_t chanleft, Double_t spread, Double_t level, Double_t threshold);

 private:
  
  TString fBranchName;
  /** In-Output array of PndMvdDigis **/
  TClonesArray* fDigiStripArray;  
  TClonesArray* fDigiPixelArray;  
  
  /** Parameter Containers **/
  PndMvdStripDigiPar* fDigiParRect;
  PndMvdStripDigiPar* fDigiParTrap;
  PndMvdPixelDigiPar* fDigiParPix;

  PndMvdGeoPar* fGeoPar;

  PndMvdGeoHandling* fGeoH;      //! Geometry name handling

  std::vector<Int_t> fPixelIds;
  std::vector<Int_t> fStripRectIds;
  std::vector<Int_t> fStripTrapIds;

  Int_t fNoiseLevel;
  Int_t fNoiseSpread;
  Int_t fThreshold;

  void Register();
  
  void Reset();
  
  void ProduceHits();

  ClassDef(PndMvdNoiseProducer,1);

};

#endif
