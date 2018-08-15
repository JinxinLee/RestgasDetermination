
/** PndMvdEventMergerTask.h
 **
 **/

#ifndef PndMvdEventMergerTask_H
#define PndMvdEventMergerTask_H 

#include "FairTask.h"
#include "PndMvdEventMerger.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndMvdEventMergerTask : public FairTask
{
 public:

  /** Default constructor **/  
 PndMvdEventMergerTask();
 
 PndMvdEventMergerTask(TString signalBranch, TString bgFile, TString bgBranch,Int_t events, Int_t mergedEvents, Bool_t signalIsBg);
  
  /** Destructor **/
  virtual ~PndMvdEventMergerTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:
  
  TString fSignalBranch;
  TString fBgBranch;
  TString fBgFile;
  Int_t fNEvents;
  Int_t  fNMergedEvents;
  Int_t fEventNr;
  Bool_t fSignalIsBg;
  /** Input array of Hits to merge **/
  TClonesArray* fSignalArray;

  /** Output array of merged hits**/
  TClonesArray* fMergedArray;
  
  PndMvdEventMerger* fMerger;
  
  void Register();
  void Reset();  
  void ProduceHits();
  
 
  ClassDef(PndMvdEventMergerTask,7);

};

#endif
