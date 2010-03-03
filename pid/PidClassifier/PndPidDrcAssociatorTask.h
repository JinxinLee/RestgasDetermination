#ifndef PID_DRCASSOCIATOR_H
#define PID_DRCASSOCIATOR_H

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

class PndPidDrcAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles


  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

  Float_t GetPdf(Float_t thetaC, Float_t mom, Float_t mass, Float_t sigma);

 public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  PndPidDrcAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidDrcAssociatorTask();
  virtual ~PndPidDrcAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

  ClassDef(PndPidDrcAssociatorTask,1)   // PndPidDrcTask
  
    };

#endif
