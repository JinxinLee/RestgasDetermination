#ifndef PndT0CandidateDetermination_H_
#define PndT0CandidateDetermination_H_




// Class to process sorted and time of flight corrected time stamps (of fast counters) to determine TO in the continuous read out.
// potential event times (T0) are stored in a onlineT0 Branche in the reco output 
//For tests it can also run in the event based mode  (RunContinuous(kFALSE)) and the blocked time for triggering new potential T0s can be changed by using SetBlockedTime(double val). (standard is 4 ns)


#include "TClonesArray.h"
#include "TString.h"

#include "FairTask.h"

class PndT0CandidateDetermination : public FairTask
{
public:

  /** Default constructor **/  
	PndT0CandidateDetermination(TString inputBranch);
/** Destructor **/
	virtual ~PndT0CandidateDetermination();


	 /** Virtual method Init **/
    //virtual void SetParContainers();
    virtual InitStatus Init();
   // virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
//    virtual void FinishEvent();
    virtual void FinishTask();
    

    void SetOutBranchName(TString name) {fOutBranchName = name;};
    //void AddInputBranch(TString branchName){ fInBranches.push_back(branchName);};
    
    void RunContinuous(Bool_t val = kTRUE){fRunContinuous = val;};
    void SetPersistence(Bool_t val= kTRUE){ fPersistence = val;};
    
    void SetBlockedTime(double val){fBlockedTime = val;};


private:

    TString fOutBranchName;
    TString fInBranch;
    
    TClonesArray* fInArray;
    TClonesArray* fOutArray;
    
    double fBlockedTime;
    double fStartTime;
  
    Bool_t fRunContinuous;    
    Bool_t fPersistence;  
  

  
  ClassDef(PndT0CandidateDetermination,0);


};

#endif /*PndT0CandidateDetermination_H_*/
