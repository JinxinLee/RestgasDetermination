#ifndef PNDMVDRIEMANNTRACKFINDERASKEFF_H_
#define PNDMVDRIEMANNTRACKFINDERASKEFF_H_

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndTrackCand.h"

#include "TString.h"
#include <vector>
#include <TH2F.h>

class PndMvdRiemannTrackFinderTaskEff : public FairTask
{
public:
	PndMvdRiemannTrackFinderTaskEff();
	virtual ~PndMvdRiemannTrackFinderTaskEff();
  PndMvdRiemannTrackFinderTaskEff(const PndMvdRiemannTrackFinderTaskEff& o) :
    eff0H(o.eff0H),
    effH(o.effH),
    GhH(o.GhH),
	  fHitBranch(o.fHitBranch),
	  fHitBranch2(o.fHitBranch2),
    fMCTrackBranch(o.fMCTrackBranch),
    fIdealTrackBranch(o.fIdealTrackBranch),
    fFTrackBranch(o.fFTrackBranch),
    fEventNr(o.fEventNr),
    fMaxSZChi2(o.fMaxSZChi2),
    fMaxSZDist(o.fMaxSZDist),
    fMinPointDist(o.fMinPointDist),
    fMaxDist(o.fMaxDist),
	  fHitArray(o.fHitArray),
	  fHitArray2(o.fHitArray2),
	  fTrackCandArray(o.fTrackCandArray),
	  fIdealTrackCandArray(o.fIdealTrackCandArray),
	  fMCTracksArray(o.fMCTracksArray)
  {};
  PndMvdRiemannTrackFinderTaskEff& operator=(const PndMvdRiemannTrackFinderTaskEff& o)
  {
    eff0H=o.eff0H;
    effH=o.effH;
    GhH=o.GhH;
	  fHitBranch=o.fHitBranch;
	  fHitBranch2=o.fHitBranch2;
    fMCTrackBranch=o.fMCTrackBranch;
    fIdealTrackBranch=o.fIdealTrackBranch;
    fFTrackBranch=o.fFTrackBranch;
    fEventNr=o.fEventNr;
    fMaxSZChi2=o.fMaxSZChi2;
    fMaxSZDist=o.fMaxSZDist;
    fMinPointDist=o.fMinPointDist;
    fMaxDist=o.fMaxDist;
	  fHitArray=o.fHitArray;
	  fHitArray2=o.fHitArray2;
	  fTrackCandArray=o.fTrackCandArray;
	  fIdealTrackCandArray=o.fIdealTrackCandArray;
	  fMCTracksArray=o.fMCTracksArray;
    return *this;
  };

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

private:
	TString fHitBranch;
	TString fHitBranch2;
    TString fMCTrackBranch;

    TString fIdealTrackBranch;
    TString fFTrackBranch;

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

  void Register();
  void Reset();
  void ProduceHits();

  void ComparingFandR(std::vector<PndTrackCand*>  RecoT);
  bool CheckRecoTrack(PndTrackCand *cand,PndMCTrack* myTrack);

  void AddGhostTrack(int trackF);

  ClassDef(PndMvdRiemannTrackFinderTaskEff,1)
};

#endif /*PNDMVDRIEMANNTRACKFINDERASKEFF_H_*/
