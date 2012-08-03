#ifndef PndMyAnalysisTask_H
#define PndMyAnalysisTask_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class TPidMassSelector;
class PndAnalysis;
class TCandList;
class PndMcTruthMatch;
class TStopwatch;
class TTuple;

class PndMyAnalysisTask : public FairTask
{

 public:
		typedef std::map<Int_t, Float_t> mapper;
	
	/** Default constructor **/  
	PndMyAnalysisTask();
	
	
	/** Destructor **/
	~PndMyAnalysisTask();
	
	
	/** Virtual method Init **/
	virtual InitStatus Init();
	
	
	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();
	//void CreateStructure();

 protected:
	
	int evcount;
	int epmax, emmax, pipmax, pimmax, mcmax;
	
	
	// **** mass selectors for the resonances/composites
	//
	
 private: 
	
	void FillMassHisto(TH1F* h, TCandList &l);
	int  SelectPdgCode(TCandList &mct, TCandList &l);
	
	TPidMassSelector *jpsiMassSel;
	PndMcTruthMatch *mcm;
	
	/**book all the histograms**/
	TH1F *hjpsim_nopid;
	TH1F *hpsim_nopid;
	TH1F *hjpsim_lpid;
	TH1F *hpsim_lpid;
	TH1F *hjpsim_tpid;
	TH1F *hpsim_tpid;
	
	TH1F *hjpsim_ftm;
	TH1F *hpsim_ftm;
	TH1F *hjpsim_nm;
	TH1F *hpsim_nm;
	
	TH1F *hjpsim_diff;
	TH1F *hpsim_diff;
	
	TH1F *hjpsim_vf;
	TH1F *hjpsim_4cf;
	TH1F *hjpsim_mcf;
	
	TH1F *hjpsi_chi2_vf;
	TH1F *hpsi_chi2_4c;
	TH1F *hjpsi_chi2_mf;
	
	TH2F *hvpos;
	
	TTuple *ntp;
	TTuple *ntp2;
	
/*	TH1F *hjpsimass;
	TH1F *hpsimass;*/
	
	PndAnalysis *theAnalysis;
	TStopwatch *timer;

	/** Get parameter containers **/
	virtual void SetParContainers();
	
	
	ClassDef(PndMyAnalysisTask,1);
  
};

#endif
