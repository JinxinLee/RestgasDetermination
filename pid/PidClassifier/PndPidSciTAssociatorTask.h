#ifndef PID_SCITASSOCIATOR_H
#define PID_SCITASSOCIATOR_H

// **************************************************************************
//  Author: Dominik Steinschaden e-mail: 
//   
//  Probability density functions from Barrel Time-of-Flight counter
//
// Created: 18-05-16
// Modified:
//
// **************************************************************************

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;

class PndPidSciTAssociatorTask : public FairTask {

 public:
  
  PndPidSciTAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidSciTAssociatorTask();
  virtual ~PndPidSciTAssociatorTask();
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

  protected:
  
  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

  Double_t GetPdf(Double_t mom, Double_t length, Double_t tof, Double_t mass, Double_t sigma);
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles
  
  ClassDef(PndPidSciTAssociatorTask,1)   // PndPidSciTTask
  
 };

#endif
