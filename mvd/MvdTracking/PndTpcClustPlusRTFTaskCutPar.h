#ifndef PNDMVDTPCCLUSTPLUSRTFTASKCUTPAR_H_
#define PNDMVDTPCCLUSTPLUSRTFTASKCUTPAR_H_

#include "FairTask.h"
#include "PndMvdHit.h"
#include "TH2F.h"
#include "TVector3.h"

#include "TString.h"
#include "PndRiemannHit.h"



class PndTpcClustPlusRTFTaskCutPar : public FairTask
{
public:
	PndTpcClustPlusRTFTaskCutPar ();
	virtual ~PndTpcClustPlusRTFTaskCutPar ();

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
	TClonesArray* fMVDRiemannTracks;
	TClonesArray* fTPCRiemannTracks;
  void Register();
  void Reset();
  void ProduceHits();

  bool CheckTooCloseHits(PndRiemannHit hit1,PndRiemannHit hit2);
  void CalcPlanes();
  void CalcParHists();

  unsigned int fNCut;

  int fNbin;
  double frangeR;
  double frangeD;
  double frangeO;
  double frangeDist;
  double frangeChi2;
  double fPtS;
  double fPtF;
  static const int fNPt=6;
  double fThetaS;
  double fThetaF;
  static const int fNTh=6;

  TH1F *fhistsR[fNPt][fNTh]; /// radius
  TH1F *fhistsD[fNPt][fNTh]; /// Dip
  TH1F *fhistsO[fNPt][fNTh]; /// Origin
  TH1F *fhistsDist[fNPt][fNTh]; /// dist to plane
  TH1F *fhistsChi2[fNPt][fNTh]; /// sz-Chi2
  TH2F *fCutRadiusH;
  TH2F *fCutDipH;
  TH2F *fCutOriginH;
  TH2F *fCutDistH;
  TH2F *fCutChi2H;

  ClassDef(PndTpcClustPlusRTFTaskCutPar ,1);


};

#endif /*PNDMVDTPCCLUSTPLUSRTFTASKCUTPAR_H_*/
