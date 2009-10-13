#ifndef PNDMVDTPCRIEMANNTRACKFINDERTASK_H_
#define PNDMVDTPCRIEMANNTRACKFINDERTASK_H_

#include "FairTask.h"
//#include "PndMvdHit.h"
#include "TH2F.h"
#include "TVector3.h"
#include "PndMvdGeoHandling.h"

#include "TString.h"


class PndMvdTPCRiemannTrackFinderTask : public FairTask
{
public:
	PndMvdTPCRiemannTrackFinderTask();
	virtual ~PndMvdTPCRiemannTrackFinderTask();

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

	void SetCutDistH(TH2F* hist)    {fCutDistH=hist;}
	void SetCutChi2H(TH2F* hist)    {fCutChi2H=hist;}

	void SetTPCCutDistH(TH2F* hist)    {fTPCCutDistH=hist;}
	void SetTPCCutChi2H(TH2F* hist)    {fTPCCutChi2H=hist;}

    void PrintResult();
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};
    void SetGeoH(PndMvdGeoHandling geoH){ fGeoH=geoH;};

private:
	TString fHitBranch;
	TString fHitBranch2;
	TString fHitBranchTPC;
    TString fTrackBranch;

    int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;

	PndMvdGeoHandling fGeoH;

	TClonesArray* fHitArray;
	TClonesArray* fHitArray2;
	TClonesArray* fHitArrayTPC;


	TClonesArray* fTrackCandArray;
	TClonesArray* fMVDTPCTrackCandArray;
//	TClonesArray* fRiemannTrackArray;
//	TClonesArray* fTrackArray;
	TClonesArray* fRiemannTracks;

	TH2F *fCutDistH;
	TH2F *fCutChi2H;

	TH2F *fTPCCutDistH;
	TH2F *fTPCCutChi2H;



  void Register();
  void Reset();
  void ProduceHits();

  void CheckTPCHitsForMvdSeeds();

	double GetMaxPlaneDist(double radius, double dip, bool sign);
	double GetMaxSZChi2(double radius, double dip, bool sign);

  ClassDef(PndMvdTPCRiemannTrackFinderTask,1);


};

#endif /*PndMvdTPCRiemannTrackFinderTask_H_*/
