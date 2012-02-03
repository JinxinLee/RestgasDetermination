#ifndef PID_IDEALFWDONLYASSOCIATOR_H
#define PID_IDEALFWDONLYASSOCIATOR_H

// **************************************************************************
//  Author: Ralf Kliemt (Uni Bonn)
//   
//  pid simple classifier
//  Warning - will use Monte-Carlo information
//
// Created: 03 Feb. 2012
// Modified:
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidIdealFwdOnlyAssociatorTask : public FairTask {
  
protected:
  TString fNeutralBranchName;
  TString fChargedBranchName;
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidNeutralCand;    //! PndPidCandidate TCA for neutral particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles
  TClonesArray* fPidNeutralProb;    //! PndPidProbability TCA for neutral particles
  TClonesArray* fMCTrack;           //! Monte-Carlo Truth track TCA

  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob); // cheating for each candidate
  
public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  PndPidIdealFwdOnlyAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidIdealFwdOnlyAssociatorTask();
  virtual ~PndPidIdealFwdOnlyAssociatorTask();
  
  void SetToOnlyOne();
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();
  
  Bool_t IsForward(PndPidCandidate* cand);
  
  ClassDef(PndPidIdealFwdOnlyAssociatorTask,1)   // PndPidIdealFwdOnlyAssociatorTask
  
};

#endif
