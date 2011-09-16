
/** PndSdsPixelDigiSorterTask.h
 **
 **/

#ifndef PndSdsPixelDigiSorterTask_H
#define PndSdsPixelDigiSorterTask_H

#include "PndSdsTask.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsDigiPixel.h"
#include "FairRingSorter.h"

#include "FairMCEventHeader.h"

class TClonesArray;

class PndSdsPixelDigiSorterTask : public PndSdsTask
{
 public:

  /** Default constructor **/
  PndSdsPixelDigiSorterTask();
  
  /** Named constructor **/
  PndSdsPixelDigiSorterTask(const char* name);

 PndSdsPixelDigiSorterTask(Int_t numberOfCells, Double_t widthOfCells);

  /** Destructor **/
  virtual ~PndSdsPixelDigiSorterTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  virtual void FinishEvent();
  virtual void FinishTask();
  
  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};
  
protected:
  
  FairRingSorter* fSorter;

  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  Bool_t fDigiPixelMCInfo; // switch to turn on/off storing additional MC Info of Digis
  
  Int_t fNumberOfCells;
  Double_t fWidthOfCells; // in ns


  /** Input array of PndSdsPixelDigis **/
  TClonesArray* fPixelArray;

  /** Output array of sorted PndSdsDigis **/
  TClonesArray* fSortedPixelArray;

  PndSdsPixelDigiPar* fDigiPar;
  Bool_t fOverwriteParams;

  Int_t fEntryNr;


  void Register();
  void Reset();
  void ProduceHits();


  ClassDef(PndSdsPixelDigiSorterTask,1);

};

#endif
