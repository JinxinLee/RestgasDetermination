#ifndef PNDMVDRIEMANNTRACKFINDERTASK_H_
#define PNDMVDRIEMANNTRACKFINDERTASK_H_

#include "CbmTask.h"
#include "PndMvdHit.h"

#include "TString.h"



class PndMvdRiemannTrackFinderTask : public CbmTask
{
public:
	PndMvdRiemannTrackFinderTask();
	virtual ~PndMvdRiemannTrackFinderTask();
	
	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    
    void PrintResult();
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};
    
private:
	TString fHitBranch;
	TString fHitBranch2;
    TString fTrackBranch;
    
    int fEventNr;

	TClonesArray* fHitArray;
	TClonesArray* fHitArray2;	
	TClonesArray* fTrackCandArray;
//	TClonesArray* fTrackArray;

	
  void Register();
  void Reset();  
  void ProduceHits();
     
  ClassDef(PndMvdRiemannTrackFinderTask,1);
	

};

#endif /*PNDMVDRIEMANNTRACKFINDERTASK_H_*/
