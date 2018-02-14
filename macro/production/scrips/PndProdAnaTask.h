#ifndef PndProdAnaTask_H
#define PndProdAnaTask_H 1


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


class PndProdAnaTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndProdAnaTask(int mode=0, TString pidAlgo="");
	
	// ** Destructor 
	~PndProdAnaTask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	
	virtual void Finish();

 protected:
	
	
 private: 
	// *** event counter
	int fEvtCount;
	
	// *** mode number to be stored
	int fMode;	
	
	// *** PID algo
	TString fPidAlgo;
	
	// *** ntuple output
	RhoTuple *ntp;
	
	// *** a method 
	int  SelectTruePid(PndAnalysis *ana, RhoCandList &l);
		
	// *** the initial 4-vector
	TLorentzVector fIni;
	
	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	ClassDef(PndProdAnaTask,1);
  
};

#endif
