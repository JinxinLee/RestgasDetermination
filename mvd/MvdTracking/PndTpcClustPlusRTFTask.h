#ifndef PNDTPCCLUSTPLUSRTFTASK_H_
#define PNDTPCCLUSTPLUSRTFTASK_H_

#include "FairTask.h"
#include "TH2F.h"
#include "TVector3.h"
#include "PndMvdGeoHandling.h"

#include "TString.h"


class PndTpcClustPlusRTFTask : public FairTask
{
public:
	PndTpcClustPlusRTFTask();
	virtual ~PndTpcClustPlusRTFTask();

	 /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    virtual void FinishEvent();
    //void PrintResult();  // not implemented
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};
    void SetMaxDist(Double_t mdist){ fMaxDist = mdist;};


	void SetCutRadiusH(TH2F* hist)    {fCutRadiusH=hist;}
	void SetCutDipH(TH2F* hist)    {fCutDipH=hist;}
	void SetCutOriginH(TH2F* hist)    {fCutOriginH=hist;}

	void SetCutDistH(TH2F* hist)    {fCutDistH=hist;}
	void SetCutChi2H(TH2F* hist)    {fCutChi2H=hist;}

private:
	TString fHitBranch;
	TString fHitBranch2;
	TString fHitBranchTPC;
    TString fTrackBranch;
    TString fMVDTrackBranch;
    double fMaxDist;

    int fEventNr;


	TClonesArray* fHitArray;
	TClonesArray* fHitArray2;
	TClonesArray* fHitArrayTPC;
	TClonesArray* fTPCTrackCandArray;
	TClonesArray* fMVDTrackCandArray;

	TClonesArray* fFinalTrackCand;


	TH2F *fCutRadiusH;
	TH2F *fCutDipH;
	TH2F *fCutOriginH;

	TH2F *fCutDistH;
	TH2F *fCutChi2H;

  void Register();
  void Reset();
  void ProduceHits();

  void AddHit(int index,TVector3 pos, std::vector< std::pair< std::vector< std::pair<int,double> > , std::pair<double,double>  > >& Blocks);
  double dist(TVector3 pos1, TVector3 pos2);
  std::pair<double,double> CutAngles(TVector2 begin, TVector2 end);
  bool CheckAngle(double testAngle,std::pair<double,double> cutAngles);
  void MergeMVDandTPC();
  void CreateTrackCand(int mvdSeed , std::vector<int> tpcAdd);

	double GetMaxRadius(double radius, double dip, bool sign);
	double GetMaxDip(double radius, double dip, bool sign);
	double GetMaxOrigin(double radius, double dip, bool sign);


	double GetMaxPlaneDist(double radius, double dip, bool sign);
	double GetMaxSZChi2(double radius, double dip, bool sign);

  ClassDef(PndTpcClustPlusRTFTask,1);


};

#endif /*PndTpcClustPlusRTFTask_H_*/
