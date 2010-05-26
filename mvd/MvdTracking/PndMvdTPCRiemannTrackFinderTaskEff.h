#ifndef PNDMVDTPCRIEMANNTRACKFINDERASKEFF_H_
#define PNDMVDTPCRIEMANNTRACKFINDERASKEFF_H_

#include "FairTask.h"
#include "PndSdsHit.h"
#include "PndMCTrack.h"
#include "PndTrackCand.h"
#include "TString.h"
#include <vector>
#include <TH2F.h>

class PndMvdTPCRiemannTrackFinderTaskEff : public FairTask
{
public:
	PndMvdTPCRiemannTrackFinderTaskEff();
	virtual ~PndMvdTPCRiemannTrackFinderTaskEff();

	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void FinishEvent();

	void SetMaxSZChi2(double val)	{fMaxSZChi2 = val;}
	void SetMaxSZDist(double val)	{fMaxSZDist = val;}
	void SetMinPointDist(double val){fMinPointDist = val;}
	void SetMaxDist(double val)		{fMaxDist = val;}

    //void PrintResult();  // not implemented
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};


    TH2F* eff0H;
    TH2F* effH;

    TH2F* GhH;

    TH2F* TPCinclTrue;
    TH2F* TPCinclFalse;
    TH2F* TPCtotal;

private:
	TString fHitBranch;
	TString fHitBranch2;
    TString fMCTrackBranch;

    TString fIdealTrackBranch;
    TString fFTrackBranch;
    TString fIdealTPCTrackBranch;
    TString fHitBranchTPC;

    int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;



	TClonesArray* fHitArray;
	TClonesArray* fHitArray2;
	TClonesArray* fTrackCandArray;
	TClonesArray* fIdealTrackCandArray;
	TClonesArray* fMCTracksArray;
	TClonesArray* fIdealTPCTrackArray;
	TClonesArray* fHitArrayTPC;

  void Register();
  void Reset();
  void ProduceHits();

  void ComparingFandR(std::vector<PndTrackCand*>  RecoT);
  bool CheckRecoTrack(PndTrackCand *cand,PndMCTrack* myTrack);
  int NumOfTpcHits(int NumOfTrack);
  void FillTPCHits(int NF, int NR,std::vector<PndTrackCand*>  RecoT);
  void AddGhostTrack(int trackF);
  ClassDef(PndMvdTPCRiemannTrackFinderTaskEff,1)


};

#endif /*PNDMVDTPCRIEMANNTRACKFINDERASKEFF_H_*/
