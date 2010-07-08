// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PNDMCMATCHSELECTORTASK_H
#define PNDMCMATCHSELECTORTASK_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndDetectorList.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCMatchSelectorTask : public FairTask
{
 public:

  /** Default constructor **/
	PndMCMatchSelectorTask();

	PndMCMatchSelectorTask(TString start, TString stop);

	PndMCMatchSelectorTask(Int_t start, Int_t stop);

  /** Destructor **/
  virtual ~PndMCMatchSelectorTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();

  virtual void SetStart(Int_t type){fStart = type;}
  virtual void SetStop(Int_t type){fStop = type;}

  virtual void SetAllWeights(Float_t weight) {fCommonWeight = weight;}
  virtual void SetWeightStage(Int_t type, Float_t weight){
	  fStageWeights.push_back(std::pair<Int_t, Float_t>(static_cast<Int_t>(type), weight));
  }

  virtual void SetWeights();



 private:
  PndMCMatch* fMCMatch;
  Int_t fStart;
  Int_t fStop;

  TString fStartString;
  TString fStopString;

  std::vector<std::pair<Int_t, Float_t> > fStageWeights;
  Float_t fCommonWeight;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCMatchSelectorTask,1);

};

#endif
