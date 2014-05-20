#ifndef PNDMVDTRACKFINDERANATASK_H_
#define PNDMVDTRACKFINDERANATASK_H_

#include "FairTask.h"
#include "PndSdsHit.h"

#include "TString.h"

#include "TH1.h"
#include "TCanvas.h"

#include <map>
#include <vector>

class PndMvdTrackFinderAnaTask : public FairTask
{
public:
	PndMvdTrackFinderAnaTask();
	virtual ~PndMvdTrackFinderAnaTask();
	
	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    
    // void PrintResult();  // not implemented
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};
    //void FinishTask(){std::cout << "Draw Canvas and Histo!" << std::endl; TCanvas c1; histo->DrawClone();};
      
    
private:
	TH1I* histo;
	TH1I* hitsPerTrack;
    TString fIdealTrackCandBranch;
    TString fRiemannTrackCandBranch;
    int fEventNr;

	TClonesArray* fIdealTrackCandArray;
	TClonesArray* fRiemannTrackCandArray;
	std::vector<std::map<int, std::vector<int> > > fHitMatrix;
	  
//	TClonesArray* fTrackArray;
	int NHitsInTrack(std::map<int, std::vector<int> > cand);
	int TrackIncluded(std::map<int, std::vector<int> > cand);
	int CheckAgreement(std::map<int, std::vector<int> > val1, std::map<int, std::vector<int> > val2);

	
  void Register();
  void Reset();  
  void ProduceHits();
     
  ClassDef(PndMvdTrackFinderAnaTask,1);
	

};

#endif /*PNDMVDTRACKANATASK_H_*/
