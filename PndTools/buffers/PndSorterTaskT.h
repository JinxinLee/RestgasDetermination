
/** PndSorterTaskT.h
 **
 **/

#ifndef PndSorterTaskT_H
#define PndSorterTaskT_H

#include "FairTask.h"
#include "PndRingSorterT.h"
#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"

#include "FairMCEventHeader.h"

class TClonesArray;

template <class T> class PndSorterTaskT : public FairTask
{
 public:

  /** Default constructor **/
  PndSorterTaskT():FairTask("SorterTask"), fNumberOfCells(1000), fWidthOfCells(10),fEntryNr(0)
  {
	  SetVerbose(0);
	  SetPersistance();
  }
  
  /** Named constructor **/
  PndSorterTaskT(const char* name):
	  FairTask(name), fNumberOfCells(1000), fWidthOfCells(10), fEntryNr(0)
  {
	  SetVerbose(0);
	  SetPersistance();
  };

 PndSorterTaskT(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName):
	 FairTask("Sorter"), fNumberOfCells(numberOfCells), fWidthOfCells(widthOfCells), fInputBranch(inputBranch), fOutputBranch(outputBranch), fFolder(folderName)
  {
	 SetVerbose(0);
	  SetPersistance();
  }

  /** Destructor **/
  virtual ~PndSorterTaskT(){
		if (fSorter!= 0) delete fSorter;
	}


  /** Virtual method Init **/
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  virtual void FinishEvent();
  virtual void FinishTask();
  
  virtual void SetParContainers(){};

  void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
  Bool_t GetPersistance() {return fPersistance;};
  
protected:
  
  PndRingSorterT<T>* fSorter;

  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  Bool_t fDigiPixelMCInfo; // switch to turn on/off storing additional MC Info of Digis
  
  Int_t fNumberOfCells;
  Double_t fWidthOfCells; // in ns


  /** Input array of PndSdsPixelDigis **/
  TString fInputBranch;
  TClonesArray* fInputArray;

  /** Output array of sorted PndSdsDigis **/
  TString fOutputBranch;
  TString fFolder;
  TClonesArray* fOutputArray;

  Int_t fEntryNr;


  ClassDef(PndSorterTaskT,2);

};

#endif
