#ifndef PNDMVDRIEMANNTRACKFINDERTASKCUTPAR_H_
#define PNDMVDRIEMANNTRACKFINDERTASKCUTPAR_H_

#include "FairTask.h"
#include "PndSdsHit.h"
#include "TH2F.h"
#include "TVector3.h"

#include "TString.h"
#include "PndRiemannHit.h"


class PndMvdRiemannTrackFinderTaskCutPar : public FairTask
{
public:
	PndMvdRiemannTrackFinderTaskCutPar();
	virtual ~PndMvdRiemannTrackFinderTaskCutPar();
  PndMvdRiemannTrackFinderTaskCutPar(const PndMvdRiemannTrackFinderTaskCutPar& o) :
	  fHitBranch(o.fHitBranch),
	  fHitBranch2(o.fHitBranch2),
	  fMCTrackBranch(o.fMCTrackBranch),
    fTrackBranch(o.fTrackBranch),
    fEventNr(o.fEventNr),
    fMaxSZChi2(o.fMaxSZChi2),
    fMaxSZDist(o.fMaxSZDist),
    fMinPointDist(o.fMinPointDist),
    fMaxDist(o.fMaxDist),
	  fHitArray(o.fHitArray),
	  fHitArray2(o.fHitArray2),
	  fTrackCandArray(o.fTrackCandArray),
	  fMCTrackArray(o.fMCTrackArray),
	  fRiemannTracks(o.fRiemannTracks),
    fNCut(o.fNCut),
    fNbin(o.fNbin),
    frangeDist(o.frangeDist),
    frangeChi2(o.frangeChi2),
    fPtS(o.fPtS),
    fPtF(o.fPtF),
    fThetaS(o.fThetaS),
    fThetaF(o.fThetaF),
    fhistsDist(),
    fhistsChi2(),
    fCutDistH(o.fCutDistH),
    fCutChi2H(o.fCutChi2H)
  {
    for(int i=0;i<fNPt;i++) for(int j=0;j<fNTh;j++) fhistsDist[i][j]=o.fhistsDist[i][j];
    for(int i=0;i<fNPt;i++) for(int j=0;j<fNTh;j++) fhistsChi2[i][j]=o.fhistsChi2[i][j];
  };
  PndMvdRiemannTrackFinderTaskCutPar& operator=(const PndMvdRiemannTrackFinderTaskCutPar& o)
  {
	  fHitBranch=o.fHitBranch;
	  fHitBranch2=o.fHitBranch2;
	  fMCTrackBranch=o.fMCTrackBranch;
    fTrackBranch=o.fTrackBranch;
    fEventNr=o.fEventNr;
    fMaxSZChi2=o.fMaxSZChi2;
    fMaxSZDist=o.fMaxSZDist;
    fMinPointDist=o.fMinPointDist;
    fMaxDist=o.fMaxDist;
	  fHitArray=o.fHitArray;
	  fHitArray2=o.fHitArray2;
	  fTrackCandArray=o.fTrackCandArray;
	  fMCTrackArray=o.fMCTrackArray;
	  fRiemannTracks=o.fRiemannTracks;
    fNCut=o.fNCut;
    fNbin=o.fNbin;
    frangeDist=o.frangeDist;
    frangeChi2=o.frangeChi2;
    fPtS=o.fPtS;
    fPtF=o.fPtF;
    fThetaS=o.fThetaS;
    fThetaF=o.fThetaF;
    for(int i=0;i<fNPt;i++) for(int j=0;j<fNTh;j++) fhistsDist[i][j]=o.fhistsDist[i][j];
    for(int i=0;i<fNPt;i++) for(int j=0;j<fNTh;j++) fhistsChi2[i][j]=o.fhistsChi2[i][j];
    fCutDistH=o.fCutDistH;
    fCutChi2H=o.fCutChi2H;
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

	void CalcCuts();

    //void PrintResult();  // not implemented
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};

private:
	TString fHitBranch;
	TString fHitBranch2;
	TString fMCTrackBranch;
    TString fTrackBranch;
    int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;

	TClonesArray* fHitArray;
	TClonesArray* fHitArray2;
	TClonesArray* fTrackCandArray;
	TClonesArray* fMCTrackArray;
	TClonesArray* fRiemannTracks;

  void Register();
  void Reset();
  void ProduceHits();

  bool CheckTooCloseHits(PndRiemannHit hit1,PndRiemannHit hit2);
  void CalcRiemannTracks();
  void CalcParHists();

  unsigned int fNCut; ///< cut max number of hits in IdealTrack

  int fNbin; ///< number of bins in the cut hists  during calculation of the cut parameters
  double frangeDist;   ///< max dist
  double frangeChi2;   ///< max cutChi2
  double fPtS;		   ///< min Pt
  double fPtF;		   ///< max Pt
  static const int fNPt=10; ///< number of Pt bins
  double fThetaS;  	   ///< min Theta
  double fThetaF;	   ///< max Theta
  static const int fNTh=10;///< number of Theta bins

  TH1F *fhistsDist[fNPt][fNTh];
  TH1F *fhistsChi2[fNPt][fNTh];
  TH2F *fCutDistH;    ///< output cutDist histogram
  TH2F *fCutChi2H;	  ///< output cutChi2 histogram

  ClassDef(PndMvdRiemannTrackFinderTaskCutPar,1);


};

#endif /*PndMvdRiemannTrackFinderTaskCutPar_H_*/
