#ifndef PndParticleQATask_H
#define PndParticleQATask_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "TLorentzVector.h"

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
	PndParticleQATask();
	
	// ** Destructor 
	~PndParticleQATask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	
	virtual void Finish();

 protected:
	
	
 private: 
	// *** event counter
	int fEvtCount;	
		
	// *** a method 
	int  SelectTruePid(PndAnalysis *ana, RhoCandList &l);
	
	void qaP4(TString pre, TLorentzVector c, RhoTuple *n, bool skip=false);
	
	void qaEmc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaMvd(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaStt(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaDrc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaDsc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaTof(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaMuo(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaTrk(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaPid(TString pre, RhoCandidate *c, RhoTuple *n);
	
	// *** declare NTuple
	RhoTuple *ntp;				// tuple to store vars from charged
	RhoTuple *ntpn;				// tuple to store vars from neutrals

	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndParticleQATask,1);
  
};

#endif
