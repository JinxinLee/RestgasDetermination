#ifndef PndPidTestTask_H
#define PndPidTestTask_H 1


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


class PndPidTestTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndPidTestTask();
	
	// ** Destructor 
	~PndPidTestTask();
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();

	void SetClassifier(TString val){
	    fClassifier = val;
	}
	
	void SetSelector(TString val){
	    fSelector = val;
	}

	void PrintConfusionMatrix(bool relative = false);

 protected:
	
	
 private: 
	// *** event counter
	int fEvtCount;	
	
	std::vector<std::vector <int> > fConfusionMatrix;
	std::map<int, int> fPdgIndex;
	std::vector<std::string> fParticleNames;
	
	
	
	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	

	// *** Get parameter containers
	virtual void SetParContainers();
	
	TString fClassifier;
	TString fSelector;
    std::vector<RhoCandList*> fCandLists;

	
	ClassDef(PndPidTestTask,1);
  
};

#endif
