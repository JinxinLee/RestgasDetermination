#ifndef PndTripleAnaTask_H
#define PndTripleAnaTask_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "TLorentzVector.h"

class TClonesArray;
class TH1F;
class TH2F;
class TDatabasePDG;

class RhoMassParticleSelector;
class RhoEnergyParticleSelector;
class PndAnalysis;
class RhoCandList;
class RhoCandidate;
class RhoTuple;
class PndRhoTupleQA;
class PndEventShape;


class PndTripleAnaTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndTripleAnaTask(double pbarmom, TString outname, int mode, TString pidalg);
	
	// ** Destructor 
	~PndTripleAnaTask();	
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();

 protected:
	
	
 private: 
	void SetVerbose(int verb=1) {fVerbose=verb;}
	void JpsiAnalysis();
	void DsDs2317Analysis();
	void ThreePiAnalysis();
	
	// *** event counter
	int fEvtCount;	
	int fMode;  // 0 = J/psi pi+ pi- ; 1 = Ds Ds2317 ; 2 = pi0 pi0 pi0
	int fVerbose;
	
	// *** the initial 4-vector
	TLorentzVector fIni;
	
	// *** the PndAnalysis object
	PndAnalysis   *fAnalysis;
	TDatabasePDG  *fPdg;
	TFile 		  *fFile;
	TString		  fOutName;
	TString       fPidAlg;
	
	// *******
	// ******* DECLARE THE STUFF YOU NEED
	// *******
	
	// *** Pointer to event shape object
	PndEventShape *fEventShape;
	// *** RhoTuple QA helper class
	PndRhoTupleQA *fQA;
	
	RhoTuple *ntp1;
	RhoTuple *ntp2;
	RhoTuple *ntp3;
	RhoTuple *ntp4;
	RhoTuple *nmc;
	
	RhoMassParticleSelector *mSel1;
	RhoMassParticleSelector *mSel2;
	RhoMassParticleSelector *mSel3;
	RhoMassParticleSelector *mSel4;
	RhoEnergyParticleSelector *gamSel;
	
	TClonesArray *fOnlineFilterInfo;
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
	
	ClassDef(PndTripleAnaTask,1);
  
};

#endif
