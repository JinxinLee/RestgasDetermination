#ifndef PID_DISCASSOCIATOR_H
#define PID_DISCASSOCIATOR_H

// **************************************************************************
//  Author: Yutie Liang e-mail: yutie.liang@physik.uni-giessen.de
//   
//  Probability density functions from Disc Cherenkov angle
//
// Created: 09-05-10
// Modified:
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidDiscAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles


  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

  Double_t GetPdf(Double_t thetaC, Double_t mom, Double_t mass, Double_t sigma);

 public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  PndPidDiscAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidDiscAssociatorTask();
  virtual ~PndPidDiscAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

  ClassDef(PndPidDiscAssociatorTask,1)   // PndPidDiscTask
  
    };

#endif
