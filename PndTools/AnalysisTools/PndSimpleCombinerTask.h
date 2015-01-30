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
class RhoTuple;
class PndSimpleCombiner;


class PndSimpleCombinerTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndSimpleCombinerTask(TString anadecay, TString anaparms);
	
	// ** Destructor 
	~PndSimpleCombinerTask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();
	
	void SetPidAlgo(TString algo) { fPidAlgo = algo;}
	
 protected:
	
	
 private: 
	int  SplitString(TString s, TString delim, std::vector<TString> &toks);// routine to split a string in pieces
	void InitParms();
	
	// *** event counter
	int fEvtCount;	
	
	// *** the initial 4-vector
	TLorentzVector fIni;
	TString        fAnaDecay;
	TString        fAnaParms;
	int            fNntp;
	TString        fPidAlgo;
	bool           fQaMC;
	bool           fQaEventShape;
	bool           fFit4C;
	bool           fFitVtx;
	
	// *** the PndAnalysis object
	PndAnalysis       *fAnalysis;
	TDatabasePDG      *fPdg;
	PndSimpleCombiner *fSimpleCombiner;

	
	
	// *******
	// ******* DECLARE THE STUFF YOU NEED
	// *******
	
	std::vector<int> vmpdg;        // pdg code of the composites
	std::vector<RhoTuple*> vntp;   // ntuples for the composites
	RhoTuple *nmc;                 // MC ntuple
		
	TClonesArray *fOnlineFilterInfo;	
	
	ClassDef(PndSimpleCombinerTask,1);
  
};

#endif
