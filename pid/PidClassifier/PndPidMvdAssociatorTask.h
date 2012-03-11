#ifndef PID_MVDASSOCIATOR_H
#define PID_MVDASSOCIATOR_H

#include "TClonesArray.h"
#include "FairTask.h"

class PndPidCandidate;
class PndPidProbability;
class PndPidMvdPar;
class PndPidMvdAssociatorTask : public FairTask {
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles


  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob); // cheating for each candidate

  Double_t GetPdf(Double_t dedx, Double_t Mpv, Double_t Sigma);

  
 public:
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
  
  void Register();
  void Reset();
  
  PndPidMvdAssociatorTask(const char *name, const char *title="Pnd Task");
  PndPidMvdAssociatorTask();
  virtual ~PndPidMvdAssociatorTask();
  
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();
  
  PndPidMvdPar *mvdPara;
  ClassDef(PndPidMvdAssociatorTask,1)   // PndPidMvdTask
  
    };

#endif
