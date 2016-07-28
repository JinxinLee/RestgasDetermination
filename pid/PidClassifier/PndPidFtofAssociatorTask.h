#ifndef PID_FTOFASSOCIATOR_H
#define PID_FTOFASSOCIATOR_H

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;
class PndPidCorrPar;

class PndPidFtofAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles


  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);

  Double_t GetPdf(Double_t mass2, Double_t mom, Double_t mass, Double_t sigma);

 public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  PndPidFtofAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidFtofAssociatorTask();
  virtual ~PndPidFtofAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

 private:

  PndPidCorrPar* fCorrPar;          //! Correlation parameters

  
  ClassDef(PndPidFtofAssociatorTask,1)   // PndPidftofTask
  
    };

#endif
