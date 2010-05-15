#ifndef PNDMVDTPCRIEMANNTRACKFINDERTASKCUTPAR_H_
#define PNDMVDTPCRIEMANNTRACKFINDERTASKCUTPAR_H_

#include "FairTask.h"
#include "PndMvdHit.h"
#include "TH2F.h"
#include "TVector3.h"

#include "TString.h"
#include "PndRiemannHit.h"
#include "GFTrackCand.h"



class PndMvdTPCRiemannTrackFinderTaskCutPar : public FairTask
{
public:
	PndMvdTPCRiemannTrackFinderTaskCutPar();
	virtual ~PndMvdTPCRiemannTrackFinderTaskCutPar();

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

	void CalcCuts();

    //void PrintResult();  // not implemented
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};

private:
	TString fHitBranch;
	TString fHitBranch2;
	TString fHitBranchTPC;
	TString fMCTrackBranch;
    TString fTrackBranch;
    TString fIdealTPCTrackBranch;
    int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;

	TClonesArray* fHitArray;
	TClonesArray* fHitArray2;
	TClonesArray* fHitArrayTPC;

	TClonesArray* fTrackCandArray;
//	TClonesArray* fMVDTPCTrackCandArray;
//	TClonesArray* fRiemannTrackArray;
	TClonesArray* fMCTrackArray;
	TClonesArray* fIdealTPCTrackArray;
	TClonesArray* fRiemannTracks;

  void Register();
  void Reset();
  void ProduceHits();

  bool CheckTooCloseHits(PndRiemannHit hit1,PndRiemannHit hit2);
  void CalcPlanes();
  void CalcParHists();

  unsigned int fNCut;

  int fNbin;
  double frangeDist;
  double frangeChi2;
  double fPtS;
  double fPtF;
  static const int fNPt=10;
  double fThetaS;
  double fThetaF;
  static const int fNTh=10;

  TH1F *fhistsDist[fNPt][fNTh];
  TH1F *fhistsChi2[fNPt][fNTh];
  TH2F *fCutDistH;
  TH2F *fCutChi2H;

  TH1F *fTEMP;

  std::vector<TVector3> GetNearestTPCHits(GFTrackCand Cand);

  ClassDef(PndMvdTPCRiemannTrackFinderTaskCutPar,1);


};

#endif /*PndMvdTPCRiemannTrackFinderTaskCutPar_H_*/
