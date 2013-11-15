#ifndef PndTutAnaTask_H
#define PndTutAnaTask_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "TLorentzVector.h"

class TClonesArray;
class TH1F;
class TH2F;

class RhoMassParticleSelector;
class PndAnalysis;
class RhoCandList;
class RhoCandidate;
class RhoTuple;


class PndTutAnaTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndTutAnaTask(double pbarmom);
	
	// ** Destructor 
	~PndTutAnaTask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();

 protected:
	
	
 private: 
	// *** event counter
	int fEvtCount;	
	
	// *** the initial 4-vector
	TLorentzVector fIni;
	
	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	
	// *******
	// ******* DECLARE THE STUFF YOU NEED
	// *******
	
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndTutAnaTask,1);
  
};

#endif
