// ************************************************************************
//
//  Analysis Task using PndSimpleCombiner 
// 
// ************************************************************************
//
// Parameters: 
// - anadecay     : Decay specification, e.g. "phi -> K+ K-; D_s+ -> phi pi+" (automatic charged conjugation; particle used have to be defined beforehand)
//                  Keyword 'nocc' at end of decay definition suppresses automatic charged conjugation
//                  This string is handed over to PndSimpleCombiner
//
// - params       : configuration parameters, e.g. "fit4c:qamc". The string contains also parameters handled by PndSimpleCombiner; those handled by this task are:
//   - fit4c      : perform 4C fit on last resonance
//   - fitvtx     : perform vertex fit on all resonances when possible (at least two daughters)
//   - qamc       : stored MC information
//   - qaevtshape : store event shape information
//
// K.Goetzen 1/2015
//
// ************************************************************************

#ifndef PndSimpleCombinerTask_H
#define PndSimpleCombinerTask_H 1


#include "FairTask.h"
#include <vector>
#include "TLorentzVector.h"
#include "TString.h"

class TClonesArray;
class TH1F;
class TH2F;
class TDatabasePDG;

class PndAnalysis;
class RhoCandList;
class RhoCandidate;
class RhoTuple;
class PndSimpleCombiner;


class PndSimpleCombinerTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndSimpleCombinerTask(TString anadecay, TString anaparms, double p=0, int run=0, int mode=0);
	
	// ** Destructor 
	~PndSimpleCombinerTask();	
	
	void SetMultFactor(int fac) {fRunMult=fac;} // set run multiplicator
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();
	
	void SetPidAlgo(TString algo) { fPidAlgo = algo;}
	void SetVerbose(int v=1) {fVerbose = v;}   // verbosity level, also passed to PndSimpleCombiner
	
 protected:
	
	
 private: 
	int  SplitString(TString s, TString delim, std::vector<TString> &toks);// routine to split a string in pieces
	int  CountChargedDaughters(RhoCandidate *c);
	void InitParms();
	
	int fVerbose;                     // verbosity level
	int fEvtCount;                    // event counter
	int fRun;                         // run number to be stored in ntuple (for unique event ID)
	int fMode;                        // optional arbitrary mode number
	int fRunMult;                     // factor for fRun to create unique event ID; default = 10000 (has be larger the num. of events per job!)
	
	TLorentzVector fIni;              // initial 4-vector; either computed from constructor parameter Mom, or taken from MC list
	TString        fAnaDecay;         // decay string
	TString        fAnaParms;         // parameter string; has to contain also parameters for PndSimpleCombiner
	int            fNntp;             // number of ntuples to be created
	TString        fPidAlgo;          // PID algo name 
	bool           fQaMC;             // flag to store MC list
	bool           fQaEventShape;     // flag to store event shape variables
	bool           fFit4C;            // flag to perform 4C fit
	bool           fFitVtx;           // flag to perform vtx fit
	double         fFit4CChiCut;      // cut on chi2 from 4C fitter
	double         fFitVtxChiCut;     // cut on chi2 from vtx fitter
	
	// *** object instances needed
	PndAnalysis       *fAnalysis;
	PndSimpleCombiner *fSimpleCombiner;
		
	std::vector<int> vmpdg;        // pdg code of the composites
	std::vector<RhoTuple*> vntp;   // ntuples for the composites
	unsigned int fNodump;          // bit marker for ntuples not to be dumped (to save disc space); e.g. 101 -> don't dump ntp0 & ntp2
	RhoTuple *nmc;                 // MC ntuple
		
	TClonesArray *fOnlineFilterInfo;	
	
	ClassDef(PndSimpleCombinerTask,1);
  
};

#endif
