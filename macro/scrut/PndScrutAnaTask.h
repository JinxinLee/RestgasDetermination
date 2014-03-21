#ifndef PndScrutAnaTask_H
#define PndScrutAnaTask_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "TLorentzVector.h"

class TClonesArray;
class TH1F;
class TH2F;
class TDatabasePDG;

class RhoMassParticleSelector;
class PndAnalysis;
class RhoCandList;
class RhoCandidate;
class RhoTuple;
class PndRhoTupleQA;



class PndScrutAnaTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndScrutAnaTask(double pbarmom);
	
	// ** Destructor 
	~PndScrutAnaTask();	
	
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
	PndAnalysis   *fAnalysis;
	TDatabasePDG  *fPdg;
	
	
	// *******
	// ******* DECLARE THE STUFF YOU NEED
	// *******
	
	RhoTuple *ntp1;
	RhoTuple *ntp2;
	RhoTuple *nmc;
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndScrutAnaTask,1);
  
};

#endif
