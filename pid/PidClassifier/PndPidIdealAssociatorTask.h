#ifndef PID_IDEALASSOCIATOR_H
#define PID_IDEALASSOCIATOR_H

// **************************************************************************
//  Author: Ralf Kliemt e-mail: r.kliemt@physik.tu-dresden.de
//   
//  pid simple classifier
//  Warning - will use Monte-Carlo information
//
// Created: 01-09-09
// Modified:
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidIdealAssociatorTask : public FairTask {
  
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
  
  PndPidIdealAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidIdealAssociatorTask();
  virtual ~PndPidIdealAssociatorTask();
  
  void SetToOnlyOne();
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();
  
  ClassDef(PndPidIdealAssociatorTask,1)   // PndPidIdealAssociatorTask
  
};

#endif
