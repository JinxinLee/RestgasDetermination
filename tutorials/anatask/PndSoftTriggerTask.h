#ifndef PndSoftTriggerTask_H
#define PndSoftTriggerTask_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class RhoMassParticleSelector;
class PndAnalysis;
class RhoCandList;
class TStopwatch;
class RhoTuple;


class PndSoftTriggerTask : public FairTask
{

 public:
		typedef std::map<Int_t, Float_t> mapper;
	
	/** Default constructor **/  
	PndSoftTriggerTask();
	
	
	/** Destructor **/
	~PndSoftTriggerTask();
	
	
	/** Virtual method Init **/
	virtual InitStatus Init();
	
	
	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();
	//void CreateStructure();

 protected:
	
	int evcount;
	int chmax, neutmax, mcmax, allmax;
	
	// counter for accepted events
	int cntsel;
	
	int cntjpsi;
	int cntd0;
	int cntdpm;
	int cntds;
	int cntphi;
	int cntlamc;
	
	
	// **** mass selectors for the resonances/composites
	//
	
 private: 
	
	void FillMassHisto(TH1F* h, RhoCandList &l);
	int  SelectPdgCode(RhoCandList &mct, RhoCandList &l);
	void PrintList(RhoCandList &l, int max=10);
	int  RemoveDoubles(RhoCandList &l, double limit=0.0001);
	void SelectPid(int type, int pdg, int chrg, RhoCandList &l, RhoCandList &lpid, double cut=0.2);
	void ConfigureHistos(TH1F *hall, TH1F *htrue, TH1F *hsel); 
	
	/**book all the histograms**/
	TH1F *h_mom;
	TH1F *h_momc;
	TH1F *h_momn;
	TH1F *h_mult;
	TH1F *h_multc;
	TH1F *h_multn;
	TH1F *h_multrem;
	
	// *** J/psi histos
	TH1F *h_jpsi;			// all J/psi combis
	TH1F *h_jpsit;			// J/psi combis w/ truth match
	TH1F *h_jpsisel;		// selected J/psi combis
	
	// *** D0 histos
	TH1F *h_d0;
	TH1F *h_d0t;
	TH1F *h_d0sel;
	
	TH1F *h_d0_pocx;
	TH1F *h_d0_pocy;
	TH1F *h_d0_pocz;
	
	TH1F *h_d0_pocxm;
	TH1F *h_d0_pocym;
	TH1F *h_d0_poczm;
	
	TH1F *h_d0_l;
	TH1F *h_d0_lm;
	
	// *** D+ histos
	TH1F *h_dpm;
	TH1F *h_dpmt;
	TH1F *h_dpmsel;
	
	// *** Ds histos
	TH1F *h_ds;
	TH1F *h_dst;
	TH1F *h_dssel;
	
	// *** Phi histos
	TH1F *h_phi;
	TH1F *h_phit;
	TH1F *h_phisel;
	
	// *** Lambda_c histos
	TH1F *h_lamc;
	TH1F *h_lamct;
	TH1F *h_lamcsel;
	
	
	RhoTuple *ntp;
	RhoTuple *ntp2;
	
/*	TH1F *hjpsimass;
	TH1F *hpsimass;*/
	
	PndAnalysis *theAnalysis;
	TStopwatch *timer;

	/** Get parameter containers **/
	virtual void SetParContainers();
	
	
	ClassDef(PndSoftTriggerTask,1);
  
};

#endif
