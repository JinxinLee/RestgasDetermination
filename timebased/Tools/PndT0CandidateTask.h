#ifndef PndT0CandidateTask_H_
#define PndT0CandidateTask_H_



// MasterClass for a fast t0 determination in the continous read out. Is also working for the event based readout if RunContinuous(kFALSE) is set.


#include "FairTask.h"


class PndT0CandidateTask : public FairTask
{
public:

  /** Default constructor **/  
	PndT0CandidateTask();
/** Destructor **/
	virtual ~PndT0CandidateTask();


	 /** Virtual method Init **/
    //virtual void SetParContainers();
    virtual InitStatus Init();
   // virtual InitStatus ReInit();

    /** Virtual method Exec **/
    //virtual void Exec(Option_t* opt);
//    virtual void FinishEvent();
    //virtual void FinishTask();
       
    void RunContinuous(Bool_t val = kTRUE);
    void SetPersistence(Bool_t val = kTRUE);

private:
   
    Bool_t fRunContinuous;
    
    Bool_t fPersistence;    
    

  
  ClassDef(PndT0CandidateTask,0);


};

#endif /*PndT0CandidateTask_H_*/
