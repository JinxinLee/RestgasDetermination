#ifndef PNDMVDRIEMANNTRACKFINDERTASK_H_
#define PNDMVDRIEMANNTRACKFINDERTASK_H_

#include "FairTask.h"
//#include "PndSdsHit.h"
#include "TH2F.h"
#include "PndGeoHandling.h"

class PndMvdRiemannTrackFinderTask : public FairTask
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
    virtual void FinishEvent();

	void SetMaxSZChi2(double val)	{fMaxSZChi2 = val;}
	void SetMaxSZDist(double val)	{fMaxSZDist = val;}
	void SetMinPointDist(double val){fMinPointDist = val;}
	void SetMaxDist(double val)		{fMaxDist = val;}

	void SetCutDistH(TH2F* hist)    {fCutDistH=hist;}
	void SetCutChi2H(TH2F* hist)    {fCutChi2H=hist;}

    //void PrintResult();  // not implemented
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};
  //  void SetGeoH(PndGeoHandling geoH){ fGeoH=geoH;};

private:
	TString fHitBranch;
	TString fHitBranch2;
    TString fTrackBranch;

    int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;



	TClonesArray* fHitArray;
	TClonesArray* fHitArray2;
	TClonesArray* fTrackCandArray;
//	TClonesArray* fRiemannTrackArray;
//	TClonesArray* fTrackArray;

	TH2F *fCutDistH;
	TH2F *fCutChi2H;

	//PndGeoHandling* fGeoH;


  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndMvdRiemannTrackFinderTask,1);


};

#endif /*PNDMVDRIEMANNTRACKFINDERTASK_H_*/
