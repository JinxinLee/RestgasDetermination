#ifndef PID_RICHASSOCIATOR_H
#define PID_RICHASSOCIATOR_H

// **************************************************************************
//  Author: Stefano Spataro e-mail: spataro@to.infn.it
//   
//  Probability density functions from DIRC Cherenkov angle
//
// Created: 03-03-10
// Modified:
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidRichAssociatorTask : public FairTask {
  
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
  
  PndPidRichAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidRichAssociatorTask();
  virtual ~PndPidRichAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

  ClassDef(PndPidRichAssociatorTask,1)   // PndPidRichTask
  
    };

#endif
