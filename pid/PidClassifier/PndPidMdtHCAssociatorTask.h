#ifndef PID_MDTHCASSOCIATOR_H
#define PID_MDTHCASSOCIATOR_H

// **************************************************************************
//  Author: Yutie LiangStefano Spataro e-mail: spataro@to.infn.it
//   
//  Probability density functions from MDT Iron - Hard Cuts
//
// Created: 26-05-10
// Modified:
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidMdtHCAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles

  TString fTrackBranchNamePidHypo;

  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

  
 public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  PndPidMdtHCAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidMdtHCAssociatorTask();
  virtual ~PndPidMdtHCAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

  void SetOutputBranch(TString branch)    { fTrackBranchNamePidHypo = branch; };

  ClassDef(PndPidMdtHCAssociatorTask,1)   // PndPidMdtHCAssociatorTask
  
    };

#endif
