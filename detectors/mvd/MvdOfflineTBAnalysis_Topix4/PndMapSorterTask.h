/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/** PndMapSorterTask.h
 **
 **/

#ifndef PndMapSorterTask_H
#define PndMapSorterTask_H

#include "FairTask.h"                   // for FairTask, InitStatus

#include "PndMapSorter.h"

#include "Rtypes.h"                     // for Bool_t, Int_t, kTRUE, etc
#include "TString.h"                    // for TString

class FairTimeStamp;
class TClonesArray;

class PndMapSorterTask : public FairTask
{
  public:

    /** Default constructor **/
    PndMapSorterTask():
      FairTask("SorterTask"),
      fSorter(0),
      fPersistance(kTRUE),
      fDigiPixelMCInfo(kFALSE),
      fInputBranch(),
      fInputArray(0),
      fOutputBranch(),
      fFolder(),
      fOutputArray(0),
      fEntryNr(0),
	  fTimeOffset(1000000000)
	  {
      SetVerbose(0);
    }

    /** Named constructor **/
    PndMapSorterTask(const char* name):
      FairTask(name),
      fSorter(0),
      fPersistance(kTRUE),
      fDigiPixelMCInfo(kFALSE),
      fInputBranch(),
      fInputArray(0),
      fOutputBranch(),
      fFolder(),
      fOutputArray(0),
      fEntryNr(0),
	  fTimeOffset(1000000)
    {
      SetVerbose(0);
    }

    PndMapSorterTask(TString inputBranch, TString outputBranch, TString folderName):
      FairTask("Sorter"),
      fSorter(0),
      fPersistance(kTRUE),
      fDigiPixelMCInfo(kFALSE),
      fInputBranch(inputBranch),
      fInputArray(0),
      fOutputBranch(outputBranch),
      fFolder(folderName),
      fOutputArray(0),
      fEntryNr(0),
	  fTimeOffset(1000000)
    {
      SetVerbose(0);
    }

    /** Destructor **/
    virtual ~PndMapSorterTask() {
      if (fSorter!= 0) { delete fSorter; }
    }

    /** Virtual method Init **/
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void FinishEvent();
    virtual void FinishTask();

    virtual void SetParContainers() {};

    void SetPersistance(Bool_t p = kTRUE) {fPersistance=p;};
    Bool_t GetPersistance() {return fPersistance;};

    void SetTimeOffset(Double_t val) {fTimeOffset = val;}

    virtual void AddNewDataToTClonesArray(FairTimeStamp* data);
//    virtual FairRingSorter* InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  protected:

    PndMapSorter* fSorter;
    /** switch to turn on/off storing the arrays to a file*/
    Bool_t fPersistance;
    /** switch to turn on/off storing additional MC Info of Digis*/
    Bool_t fDigiPixelMCInfo;
    /** Input array of PndSdsPixelDigis **/
    TString fInputBranch;
    TClonesArray* fInputArray;
    /** Output array of sorted PndSdsDigis **/
    TString fOutputBranch;
    TString fFolder;
    TClonesArray* fOutputArray;
    Int_t fEntryNr;
    Double_t fTimeOffset;
    PndMapSorterTask(const PndMapSorterTask&);
    PndMapSorterTask& operator=(const PndMapSorterTask&);

    ClassDef(PndMapSorterTask,2);

};

#endif
