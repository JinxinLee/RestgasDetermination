#ifndef PndTutThaiTask_H
#define PndTutThaiTask_H 1


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


class PndTutThaiTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndTutThaiTask();
	
	// ** Destructor 
	~PndTutThaiTask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	
	virtual void Finish();

 protected:
	
	
 private: 
	// *** event counter
	int fEvtCount;	
	
	// *** mass selector for the J/psi
	RhoMassParticleSelector *fJpsiMassSel;
	
	// *** a method 
	int  SelectTruePid(PndAnalysis *ana, RhoCandList &l);
	
	// #### EXERCISE: declare some histograms
	TH1F *hjpsim_all;
	TH1F *hpsim_all;
	// ...
	
	// *** the initial 4-vector
	TLorentzVector fIni;
	
	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndTutThaiTask,1);
  
};

#endif
