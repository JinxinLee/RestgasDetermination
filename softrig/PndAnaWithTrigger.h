#ifndef PndAnaWithTrigger_H
#define PndAnaWithTrigger_H 1


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



class PndAnaWithTrigger : public FairTask
{

 public:
	
	// ** Default constructor   
	PndAnaWithTrigger(double pbarmom, TString outname);
	
	// ** Destructor 
	~PndAnaWithTrigger();	
	
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
	TFile 		  *fFile;
	TString		  fOutName;
	
	
	// *******
	// ******* DECLARE THE STUFF YOU NEED
	// *******
	
	RhoTuple *ntp1;
	RhoTuple *ntp2;
	RhoTuple *nmc;
	
	RhoMassParticleSelector *jpsiMassSel;
	RhoMassParticleSelector *jpsiPreMassSel;
	
	// TCA to access software trigger info
	TClonesArray *fOnlineFilterInfo;	
	
	ClassDef(PndAnaWithTrigger,1);
  
};

#endif
