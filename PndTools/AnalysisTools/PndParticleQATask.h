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


class PndParticleQATask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndParticleQATask(bool fastsim = false);
	
	// ** Destructor 
	~PndParticleQATask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	
	virtual void Finish();
	
	void SetFastSim(bool fsim=true) {fFastSim = fsim;}

 protected:
	
	
 private: 
	// *** event counter
	int fEvtCount;	
		
	// *** a method 
	int  SelectTruePid(PndAnalysis *ana, RhoCandList &l);
		
	// *** declare NTuple
	RhoTuple *nmc;				// mc truth
	RhoTuple *ntp;				// tuple to store vars from charged
	RhoTuple *ntpn;				// tuple to store vars from neutrals

	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	bool fFastSim;
	
	TString fPid[10];
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndParticleQATask,1);
  
};

#endif
