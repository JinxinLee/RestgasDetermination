#ifndef PNDMVDTpcRiemannCorrelatorTASK_H_
#define PNDMVDTpcRiemannCorrelatorTASK_H_

#include "FairTask.h"
#include "PndMvdHit.h"

#include "PndRiemannTrack.h"
#include "TrackCand.h"

#include "TString.h"
#include "TH1.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include <vector>

// This class takes the tracks from the MVD trackfinder and tries to find corresponding tracks in the TPC

class PndMvdTpcRiemannCorrelatorTask : public FairTask
{
public:
	PndMvdTpcRiemannCorrelatorTask();
	virtual ~PndMvdTpcRiemannCorrelatorTask();
	
	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    
    void PrintResult();
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};
    
    void DrawDistHistos(TCanvas* can, int event);
    void DrawSZHistos(TCanvas* can, int event);
    void DrawSZChi2Histos(TCanvas* can, int event);
    void DrawHistos(TCanvas* can, std::vector<TH1*> histos);
    void SetMaxDist(double val){fMaxDist = val;}
    void SetMaxSZ(double val){fMaxSZ = val;}
    void SetMaxSZChi2(double val){fMaxSZChi2 = val;}
    
private:
	TString fHitBranchMVDPixel;
	TString fHitBranchMVDStrip;
	TString fHitBranchTPC;
    TString fTrackBranch;
    
    std::vector< std::vector<TH1*> > fHRiemannDistVector;
    std::vector< std::vector<TH1*> > fHSZDistVector;
    std::vector< std::vector<TH1*> > fHSZChi2Vector;
    
    double fMaxDist;
    double fMaxSZ;
    double fMaxSZChi2;
    
    int fEventNr;

	TClonesArray* fHitArrayMVDPixel;
	TClonesArray* fHitArrayMVDStrip;
	TClonesArray* fHitArrayTPC;	
	TClonesArray* fTrackCandArray;
	TClonesArray* fCombinedArray;
//	TClonesArray* fTrackArray;

	PndRiemannTrack GetRiemannTrack(TrackCand* cand);  ///< takes the track candidates and fills a Riemann Track
	TrackCand AddTPCHits(TrackCand* myTrack); ///< runs through all TPC clusters and checks if they belong to the track. If yes, they are added and a new track is created.
	
  void Register();
  void Reset();  
  void ProduceHits();
     
  ClassDef(PndMvdTpcRiemannCorrelatorTask,1);
	

};

#endif /*PNDMVDTpcRiemannCorrelatorTASK_H_*/
