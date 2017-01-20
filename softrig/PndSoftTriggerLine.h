// ************************************************************************
//
// Online SoftTrigger TriggerLine class
//
// K.Goetzen 08/2014
//
// ************************************************************************

#ifndef PndSoftTriggerLine_H
#define PndSoftTriggerLine_H 1

#include "TString.h"
#include <vector>

class RhoMassParticleSelector;
class RhoTuple;
class TDatabasePDG;

class PndSoftTriggerLine
{
public:
	// *** Constructors   
	PndSoftTriggerLine();
	PndSoftTriggerLine(int mode, TString name, TString dec, TString pre, TString ntpname);
	
	// *** Destructor 
	~PndSoftTriggerLine();	
	
	// *** general methods
	void Init();  // has to be called to initialize the trigger line after everything has set properly; creates ntuple and parses Decay pattern	
	void Print();
	
	// *** Modifiers
	void SetModeCode(int mode) {fMode = mode;}
	void SetName(TString name) {fName = name;}
	void SetDecay(TString dec) {fDecay = dec; ParseDecayString();}
	void SetPrefix(TString pre) {fPrefix = pre;}
	void SetNtupleName(TString ntp) {fNTupleName = ntp;}

	void SetWriteQA(bool qa=true) {fWriteQA = qa;}
	void SetTagActive(bool ac=true) {fActive = ac;}
	
	void SetQAMassWindow(double min, double max) {fQAMassMin = min; fQAMassMax = max; }
	void SetMeanSigma(double mean, double sig) {fMean = mean; fSigma = sig; }
	void SetThreshold(double thr) {fThresh = thr;}
	void SetTagNSig(double nsig) {fTagNSig = nsig; }
	
	void SetNTagged(int n) {fNTagged = n; }
	
	// *** Accessors
	int GetModeCode() {return fMode;}
	TString GetName() {return fName;}
	TString GetDecay() {return fDecay;}
	TString GetPrefix() {return fPrefix;}
	TString GetNTupleName() {return fNTupleName;}
	
	bool GetWriteQA() {return fWriteQA;}
	bool GetTagActive() {return fActive;}
	bool GetCC() {return fCC;}
	bool GetAuxNeeded() {return fAux;}
	
	double GetQAMassMin() {return fQAMassMin;}
	double GetQAMassMax() {return fQAMassMax;}
	double GetMean() {return fMean;}
	double GetSigma() {return fSigma;}
	double GetThreshold() {return fThresh;}
	double GetTagNSig() {return fTagNSig;}
	
	int GetNTagged() {return fNTagged;}

	int GetMotherPdg() {if (fPdgList.size()>0) return fPdgList[0]; return -1;}
	int GetDaughterPdg(int idx) {if ((int)fPdgList.size()>idx+1) return fPdgList[idx+1]; return -1;}
	int GetNDaughters() {return fPdgList.size()-1;}
		
	RhoMassParticleSelector *GetQASelector() {return fQASelector;}
	RhoMassParticleSelector *GetSelector() {return fSelector;}
	RhoTuple* GetRhoTuple() {return fNTuple;}
	
	
private:
	bool ParseDecayString();	            // convert the decay pattern string to a list of pdg codes (fPdgList)
	void SetQASelector(double m, double w);	// set new parameters for the QA selector
	void SetSelector(double m, double w);   // set new parameters for the mass selector
	int  SplitString(TString s, TString delim, TString *toks, int maxtoks); // splits a string into tokens; toks is the return array and has to created by the calling entity
	
	int     fMode;			// the unique mode code 
	TString fName;			// name of decay mode (e.g. "D0ToKpi")
	TString fDecay;			// decay pattern (e.g. "D0 -> K- pi+")
	TString fPrefix;		// prefix used in output n-tuple (e.g. "d0")
	TString fNTupleName;	// name of the QA Ntuple
	bool	fWriteQA;		// flag for output of QA tuple
	bool    fActive;		// flag whether tagging for this mode is cuurently active
	bool    fCC;			// flag whether charged conjugation is implied
	bool    fAux;			// flag whether auxilliary resonance is present (e.g. D*0 -> D0 [K- pi+] pi0, D0 is aux res.)
	
	double  fQAMassMin;		// mass window minimum for preselection/QA tuple
	double  fQAMassMax;		// mass window maximum for preselection/QA tuple
	double  fMean;			// mean value for final mass cut (ideally extracted from distribution)
	double  fSigma;			// sigma value for final mass cut ('')
	double  fThresh;		// threshold sqrt(s) for triggering (e.g. for incl D0, it could be m_D0 + m_charmquark)
	double  fTagNSig;		// number of sigmas for final mass selection
	
	int     fNTagged;		// number of tagged candidates (used by the task)

	RhoMassParticleSelector		*fQASelector;	// the selector for QA preselection
	RhoMassParticleSelector		*fSelector;		// the final inv mass selector
	RhoTuple 					*fNTuple;		// the QA n-tuple
	std::vector<int>			fPdgList;		// container to hold the pdg codes of the decay; [0] holds the mothers code
	
	TDatabasePDG				*fPdg;			// PDG database for lookup
};

#endif
