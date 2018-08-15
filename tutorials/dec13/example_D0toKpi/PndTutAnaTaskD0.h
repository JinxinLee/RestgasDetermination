#ifndef PndTutAnaTaskD0_H
#define PndTutAnaTaskD0_H 1


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
class RhoVtxPoca;


class PndTutAnaTaskD0 : public FairTask
{

 public:
	
	// ** Default constructor   
	PndTutAnaTaskD0(double pbarmom);
	
	// ** Destructor 
	~PndTutAnaTaskD0();	
	
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
	RhoTuple *nd0;
	RhoMassParticleSelector *fD0Sel;
	RhoVtxPoca *fVtxPoca;
	
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndTutAnaTaskD0,1);
  
};

#endif
