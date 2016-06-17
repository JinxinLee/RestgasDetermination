#ifndef PndParticleQATask_H
#define PndParticleQATask_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "TLorentzVector.h"
#include "TString.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class RhoMassParticleSelector;
class PndAnalysis;
class RhoCandList;
class RhoTuple;
class RhoCandidate;

typedef std::vector<TString> StringList;

class PndParticleQATask : public FairTask
{

 public:
	
	// ** Default constructor   
  PndParticleQATask(bool fastsim = false, bool dumpchrg=true, bool dumpneut=true, bool dumpmc = true);
	
	// ** Destructor 
	~PndParticleQATask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	
	virtual void Finish();
	
	void SetFastSim(bool fsim=true) {fFastSim = fsim;}
	void SetPidArrayNames(TString names) {fPidArrayNames = names;}
	
 protected:
	
	
 private: 
	// *** event counter
	int fEvtCount;
	TString fPidArrayNames;
		
	// *** a method 
	int  SelectTruePid(PndAnalysis *ana, RhoCandList &l);
	int  SplitString(TString s, TString delim, StringList &toks);// routine to split a string in pieces
		
	// *** declare NTuple
	RhoTuple *nmc;				// mc truth
	RhoTuple *ntp;				// tuple to store vars from charged
	RhoTuple *ntpn;				// tuple to store vars from neutrals

	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	bool fFastSim;
	bool fDumpChrg;
	bool fDumpNeut;
	bool fDumpMc;
	
	StringList fPidList;
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndParticleQATask,1);
  
};

#endif
