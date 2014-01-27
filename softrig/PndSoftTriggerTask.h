#ifndef PndSoftTriggerTask_H
#define PndSoftTriggerTask_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "TLorentzVector.h"
#include "RhoCandList.h"
#include "TDatabasePDG.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class RhoMassParticleSelector;
class RhoEnergyParticleSelector;
class RhoMomentumParticleSelector;
class PndAnalysis;
class RhoCandList;
class RhoTuple;
class RhoCandidate;
class PndVtxPoca;
class PndEventShape;
class TDatabasePDG;
class PndRhoTupleQA;

class PndSoftTriggerTask : public FairTask
{

 public:
	
	// *** Default constructor   
	PndSoftTriggerTask(double pmom, int mode=0);
	
	// *** Destructor 
	~PndSoftTriggerTask();	
	
	// *** Virtual method Init 
	virtual InitStatus Init();
	
	// *** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	virtual void Finish();
	
	// *** set max number of sigmas deviation for tag
	void SetNsigTag(double nsig) {fNsigTag = nsig;}
	void SetNsigAux(double nsig) {fNsigAux = nsig;}
	
	// *** set selection parameters for QA tuple
	void SetPi0QASelection(double min, double max) {fPi0QaMin=min; fPi0QaMax=max;}
	void SetKs0QASelection(double min, double max) {fKs0QaMin=min; fKs0QaMax=max;}
	void SetEtaQASelection(double min, double max) {fEtaQaMin=min; fEtaQaMax=max;}

	void SetPhiQASelection(double min, double max) {fPhiQaMin=min; fPhiQaMax=max;}
	
	void SetD0QASelection(double min, double max) {fD0QaMin=min; fD0QaMax=max;}
	void SetDpmQASelection(double min, double max) {fDpmQaMin=min; fDpmQaMax=max;}
	void SetDsQASelection(double min, double max) {fDsQaMin=min; fDsQaMax=max;}
	
	void SetLambdaQASelection(double min, double max) {fLamQaMin=min; fLamQaMax=max;}
	void SetLambdacQASelection(double min, double max) {fLamcQaMin=min; fLamcQaMax=max;}

	void SetJpsiQASelection(double min, double max) {fJpsiQaMin=min; fJpsiQaMax=max;}
	void SetEtacQASelection(double min, double max) {fEtacQaMin=min; fEtacQaMax=max;}
	void SetChic0QASelection(double min, double max) {fChic0QaMin=min; fChic0QaMax=max;}
	
	void Set2eQASelection(double min, double max) {f2eQaMin=min; f2eQaMax=max;}
	void Set2muQASelection(double min, double max) {f2muQaMin=min; f2muQaMax=max;}
	void Set2gamQASelection(double min, double max) {f2gamQaMin=min; f2gamQaMax=max;}
	
	void SetQASelectionDefaults();
	
	// *** set mean and sigma for tagging algos
	void SetPi0SignalParams(double mean, double sigma) {fPi0Mean=mean; fPi0Sigma=sigma;}
	void SetKs0SignalParams(double mean, double sigma) {fKs0Mean=mean; fKs0Sigma=sigma;}
	void SetEtaSignalParams(double mean, double sigma) {fKs0Mean=mean; fKs0Sigma=sigma;}

	void SetPhiSignalParams(double mean, double sigma) {fPhiMean=mean; fPhiSigma=sigma;}
	
	void SetD01SignalParams(double mean, double sigma) {fD01Mean=mean; fD01Sigma=sigma;}
	void SetD02SignalParams(double mean, double sigma) {fD02Mean=mean; fD02Sigma=sigma;}
	void SetD03SignalParams(double mean, double sigma) {fD03Mean=mean; fD03Sigma=sigma;}
	
	void SetDpm1SignalParams(double mean, double sigma) {fDpm1Mean=mean; fDpm1Sigma=sigma;}
	void SetDpm2SignalParams(double mean, double sigma) {fDpm2Mean=mean; fDpm2Sigma=sigma;}
	void SetDpm3SignalParams(double mean, double sigma) {fDpm3Mean=mean; fDpm3Sigma=sigma;}
	void SetDpm4SignalParams(double mean, double sigma) {fDpm4Mean=mean; fDpm4Sigma=sigma;}
	
	void SetDs1SignalParams(double mean, double sigma) {fDs1Mean=mean; fDs1Sigma=sigma;}
	void SetDs2SignalParams(double mean, double sigma) {fDs2Mean=mean; fDs2Sigma=sigma;}
	
	void SetLambdaSignalParams(double mean, double sigma) {fLamMean=mean; fLamSigma=sigma;}
	
	void SetLambdacSignalParams(double mean, double sigma) {fLamcMean=mean; fLamcSigma=sigma;}

	void SetJpsi1SignalParams(double mean, double sigma) {fJpsi1Mean=mean; fJpsi1Sigma=sigma;}
	void SetJpsi2SignalParams(double mean, double sigma) {fJpsi2Mean=mean; fJpsi2Sigma=sigma;}

	void SetEtac1SignalParams(double mean, double sigma) {fEtac1Mean=mean; fEtac1Sigma=sigma;}
	void SetEtac2SignalParams(double mean, double sigma) {fEtac2Mean=mean; fEtac2Sigma=sigma;}
	void SetEtac3SignalParams(double mean, double sigma) {fEtac3Mean=mean; fEtac3Sigma=sigma;}
	void SetEtac4SignalParams(double mean, double sigma) {fEtac4Mean=mean; fEtac4Sigma=sigma;}

	void SetChic01SignalParams(double mean, double sigma) {fChic01Mean=mean; fChic01Sigma=sigma;}
	void SetChic02SignalParams(double mean, double sigma) {fChic02Mean=mean; fChic02Sigma=sigma;}
	void SetChic03SignalParams(double mean, double sigma) {fChic03Mean=mean; fChic03Sigma=sigma;}
	
	void Set2eSignalParams(double mean, double sigma) {f2eMean=mean; f2eSigma=sigma;}
	
	void Set2muSignalParams(double mean, double sigma) {f2muMean=mean; f2muSigma=sigma;}
	
	void Set2gamSignalParams(double mean, double sigma) {f2gamMean=mean; f2gamSigma=sigma;}
	
	void SetSignalParamsDefaults();
	
	// *** pre selection of neutral and charged + PID cut
	void SetGammaMinE(double min) {fGammaMinE=min;}
	void SetTrackMinP(double min) {fTrackMinP=min;}
	void SetInitialPidCut(double cut) {fIniPidCut=cut;}
	
	// *** modifiers for PID selection string
	void SetAlgoElectron(TString algo) {fAlgoElectron=algo;}
	void SetAlgoMuon(TString algo) {fAlgoMuon=algo;}
	void SetAlgoPion(TString algo) {fAlgoPion=algo;}
	void SetAlgoKaon(TString algo) {fAlgoKaon=algo;}
	void SetAlgoProton(TString algo) {fAlgoProton=algo;}
	
	// *** Switch Tagging methods
	void SetTag_Phi_KK(bool tag=true) { fTagPhiKK = tag;}
	
	void SetTag_D0_Kpi(bool tag=true) { fTagD0Kpi = tag;}
	void SetTag_D0_Kpipi0(bool tag=true) { fTagD0Kpipi0 = tag;}
	void SetTag_D0_K3pi(bool tag=true) { fTagD0K3pi = tag;}
	
	void SetTag_Dpm_Kpipi(bool tag=true) { fTagDpmKpipi = tag;}
	void SetTag_Dpm_K2pipi0(bool tag=true) { fTagDpmK2pipi0 = tag;}
	void SetTag_Dpm_Kspipi0(bool tag=true) { fTagDpmKspipi0 = tag;}
	void SetTag_Dpm_Ks3pi(bool tag=true) { fTagDpmKs3pi = tag;}
	
	void SetTag_Ds_KKpi(bool tag=true) { fTagDsKKpi = tag;}
	void SetTag_Ds_KKpipi0(bool tag=true) { fTagDsKKpip0 = tag;}
	
	void SetTag_Lambda_ppi(bool tag=true) { fTagLamppi = tag;}

	void SetTag_Lambdac_pKpi(bool tag=true) { fTagLamcpKpi = tag;}

	void SetTag_Jpsi_2e(bool tag=true) { fTagJpsi2e = tag;}
	void SetTag_Jpsi_2mu(bool tag=true) { fTagJpsi2mu = tag;}
	
	void SetTag_Etac_KKpi0(bool tag=true) { fTagEtacKKpi0 = tag;}
	void SetTag_Etac_KKspi(bool tag=true) { fTagEtacKKspi = tag;}
	void SetTag_Etac_etapipi(bool tag=true) { fTagEtacetapipi = tag;}
	void SetTag_Etac_gg(bool tag=true) { fTagEtacgg = tag;}
	
	void SetTag_Chic0_2pi2pi0(bool tag=true) { fTagChic02pi2pi0 = tag;}
	void SetTag_Chic0_4pi(bool tag=true) { fTagChic04pi = tag;}
	void SetTag_Chic0_2pi2K(bool tag=true) { fTagChic02pi2K = tag;}
	
	void SetTag_2e(bool tag=true) { fTag2e = tag;}
	void SetTag_2mu(bool tag=true) { fTag2mu = tag;}
	void SetTag_2gam(bool tag=true) { fTag2gam = tag;}
	
	void SetTag_All(bool qa=true);
	
	// *** Enable/Disable QA output
	void SetQA_Phi_KK(bool qa=true) { fQAPhiKK = qa;}
	
	void SetQA_D0_Kpi(bool qa=true) { fQAD0Kpi = qa;}
	void SetQA_D0_Kpipi0(bool qa=true) { fQAD0Kpipi0 = qa;}
	void SetQA_D0_K3pi(bool qa=true) { fQAD0K3pi = qa;}
	
	void SetQA_Dpm_Kpipi(bool qa=true) { fQADpmKpipi = qa;}
	void SetQA_Dpm_K2pipi0(bool qa=true) { fQADpmK2pipi0 = qa;}
	void SetQA_Dpm_Kspipi0(bool qa=true) { fQADpmKspipi0 = qa;}
	void SetQA_Dpm_Ks3pi(bool qa=true) { fQADpmKs3pi = qa;}
	
	void SetQA_Ds_KKpi(bool qa=true) { fQADsKKpi = qa;}
	void SetQA_Ds_KKpipi0(bool qa=true) { fQADsKKpip0 = qa;}
	
	void SetQA_Lambdac_pKpi(bool qa=true) { fQALamcpKpi = qa;}

	void SetQA_Lambda_ppi(bool qa=true) { fQALamppi = qa;}
	
	void SetQA_Jpsi_2e(bool qa=true) { fQAJpsi2e = qa;}
	void SetQA_Jpsi_2mu(bool qa=true) { fQAJpsi2mu = qa;}
	
	void SetQA_Etac_KKpi0(bool qa=true) { fQAEtacKKpi0 = qa;}
	void SetQA_Etac_KKspi(bool qa=true) { fQAEtacKKspi = qa;}
	void SetQA_Etac_etapipi(bool qa=true) { fQAEtacetapipi = qa;}
	void SetQA_Etac_gg(bool qa=true) { fQAEtacgg = qa;}
	
	void SetQA_Chic0_2pi2pi0(bool qa=true) { fQAChic02pi2pi0 = qa;}
	void SetQA_Chic0_4pi(bool qa=true) { fQAChic04pi = qa;}
	void SetQA_Chic0_2pi2K(bool qa=true) { fQAChic02pi2K = qa;}
	
	void SetQA_2e(bool qa=true) { fQA2e = qa;}
	void SetQA_2mu(bool qa=true) { fQA2mu = qa;}
	void SetQA_2gam(bool qa=true) { fQA2gam = qa;}
	
	void SetQA_Pi0(bool qa=true) { fQAPi0 = qa;}
	void SetQA_Eta(bool qa=true) { fQAEta = qa;}
	void SetQA_Ks0(bool qa=true) { fQAKs0 = qa;}
	
	void SetQA_Event(bool qa=true) { fQAEvent = qa;}
	
	void SetQA_All(bool qa=true);

 protected:
	
	
 private: 
	// *** List preparation methods
	void FillGlobalLists();
	
	// *** some helper functions
	int SelectTruePid(RhoCandList &l);
	int SelectPidProb(RhoCandList &l, int pididx, double cut);
	int signalType(RhoCandList &l, int v0pdg, int d1pdg, int d2pdg); 
	double DbMass(TString name) {if (fPdg->GetParticle(name)) return fPdg->GetParticle(name)->Mass();}
		
	// *** Created necessary composites
	int CreateKs0Cands(RhoTuple *n);
	
	// *** Tagging methods
	int Tag_Phi_KK(RhoTuple *n);
	int Tag_Lambda_ppi(RhoTuple *n);
	int Tag_Jpsi_2e(RhoTuple *n);
	int Tag_Jpsi_2mu(RhoTuple *n);
	
	int Tag_D0_Kpi(RhoTuple *n);
	int Tag_D0_Kpipi0(RhoTuple *n);
	int Tag_D0_K3pi(RhoTuple *n);
	
	int Tag_Dpm_Kpipi(RhoTuple *n);
	int Tag_Dpm_K2pipi0(RhoTuple *n);
	int Tag_Dpm_Kspipi0(RhoTuple *n);
	int Tag_Dpm_Ks3pi(RhoTuple *n);
	
	int Tag_Ds_KKpi(RhoTuple *n);
	int Tag_Ds_KKpipi0(RhoTuple *n);
	
	int Tag_Lambdac_pKpi(RhoTuple *n);
	
	int Tag_Etac_KKpi0(RhoTuple *n);
	int Tag_Etac_KKspi(RhoTuple *n);
	int Tag_Etac_etapipi(RhoTuple *n);
	int Tag_Etac_gg(RhoTuple *n);
	
	int Tag_Chic0_2pi2pi0(RhoTuple *n);
	int Tag_Chic0_4pi(RhoTuple *n);
	int Tag_Chic0_2pi2K(RhoTuple *n);
	
	int Tag_2e(RhoTuple *n);
	int Tag_2mu(RhoTuple *n);
	int Tag_2gam(RhoTuple *n);
	
	
	// *** global vars
	int fMode;					// the signal or background mode code, to be set in the constructor
	int fEvtCount;				// global event counter
	int fSigCount;	            // counter, unused for the time being
	double fNsigTag;				// max number of sigmas deviation of candidate to be tagged
	double fNsigAux;				// max number of sigmas deviation for auxilliary particle = pi0, K_S, eta
	
	// *** initial pbar p system info
	TLorentzVector fIniP4;
	double fEcm;
	double fPbarMom;
	
	// *** flags for enable/disable tagging for individual channels
	bool fTagPhiKK;
	bool fTagLamppi;
	
	bool fTagJpsi2e;
	bool fTagJpsi2mu;
	
	bool fTagD0Kpi;
	bool fTagD0Kpipi0;
	bool fTagD0K3pi;
	
	bool fTagDpmKpipi;
	bool fTagDpmK2pipi0;
	bool fTagDpmKspipi0;
	bool fTagDpmKs3pi;	
	
	bool fTagDsKKpi;
	bool fTagDsKKpip0;
	
	bool fTagLamcpKpi;
	
	bool fTagEtacKKpi0;
	bool fTagEtacKKspi;
	bool fTagEtacetapipi;
	bool fTagEtacgg;
	
	bool fTagChic02pi2pi0;
	bool fTagChic04pi;
	bool fTagChic02pi2K;
	
	bool fTag2e;
	bool fTag2mu;
	bool fTag2gam;
	
	// *** flags for enable/disable QA ntuple output
	bool fQAPhiKK;
	bool fQALamppi;
	
	bool fQAJpsi2e;
	bool fQAJpsi2mu;
	
	bool fQAD0Kpi;
	bool fQAD0Kpipi0;
	bool fQAD0K3pi;
	
	bool fQADpmKpipi;
	bool fQADpmK2pipi0;
	bool fQADpmKspipi0;
	bool fQADpmKs3pi;	
	
	bool fQADsKKpi;
	bool fQADsKKpip0;
	
	bool fQALamcpKpi;
	
	bool fQAEtacKKpi0;
	bool fQAEtacKKspi;
	bool fQAEtacetapipi;
	bool fQAEtacgg;
	
	bool fQAChic02pi2pi0;
	bool fQAChic04pi;
	bool fQAChic02pi2K;
	
	bool fQA2e;
	bool fQA2mu;
	bool fQA2gam;
	
	bool fQAPi0;
	bool fQAEta;
	bool fQAKs0;
	bool fQAEvent;
	
    // ----------------------------
	// *** parameters for ntuple QA window	
	double fPi0QaMin;		// min value for pi0 selection
	double fPi0QaMax;		// max value for pi0 selection
	
	double fEtaQaMin;		// min value for eta(gg) selection
	double fEtaQaMax;		// max value for eta(gg) selection
	
	double fKs0QaMin;		// min value for Ks selection
	double fKs0QaMax;		// max value for Ks selection
	
	// *** resonances
	double fPhiQaMin;		// min value for phi selection
	double fPhiQaMax;		// max value for phi selection
	
	// *** open charm
	double fD0QaMin;		// min value for D0 selection
	double fD0QaMax;		// max value for D0 selection
	
	double fDpmQaMin;		// min value for D+ selection
	double fDpmQaMax;		// max value for D+ selection
	
	double fDsQaMin;		// min value for Ds selection
	double fDsQaMax;		// max value for Ds selection
	
	// *** baryons
	double fLamcQaMin;		// min value for Lambdac selection
	double fLamcQaMax;		// max value for Lambda selection

	double fLamQaMin;		// min value for Lambda selection
	double fLamQaMax;		// max value for Lambda selection
	
	// *** charmonia
	double fEtacQaMin;		// min value for eta_c selection
	double fEtacQaMax;		// max value for eta_c selection
	
	double fJpsiQaMin;		// min value for jpsi selection
	double fJpsiQaMax;		// max value for jpsi selection
	
	double fChic0QaMin;		// min value for chic0 selection
	double fChic0QaMax;		// max value for chic0 selection
	
	// *** electro-magnetic 
	double f2eQaMin;		// min value for e+ e- selection
	double f2eQaMax;		// max value for for e+ e- selection
	
	double f2muQaMin;		// min value for mu+ mu- selection
	double f2muQaMax;		// max value for for mu+ mu- selection
	
	double f2gamQaMin;		// min value for gam gam selection
	double f2gamQaMax;		// max value for for gam gam selection


    // ----------------------------
	// *** signal parameters for different channels
	double fPi0Mean;		// mean value for pi0 signal
	double fPi0Sigma;		// sigma value for pi0 signal
	
	double fEtaMean;		// mean value for eta(gg) signal
	double fEtaSigma;		// sigma value for eta(gg) signal
	
	double fKs0Mean;		// mean value for Ks signal
	double fKs0Sigma;		// sigma value for Ks signal
	
	// *** resonances
	double fPhiMean;		// mean value for phi signal
	double fPhiSigma;		// sigma value for phi signal
	
	// *** open charm
	double fD01Mean;		// mean value for D0 signal (K pi)
	double fD01Sigma;		// sigma value for D0 signal
	
	double fD02Mean;		// mean value for D0 signal (K pi pi0)
	double fD02Sigma;		// sigma value for D0 signal
	
	double fD03Mean;		// mean value for D0 signal (K pi pi pi)
	double fD03Sigma;		// sigma value for D0 signal
	
	double fDpm1Mean;		// mean value for D+ signal (K pi pi)
	double fDpm1Sigma;		// sigma value for D+ signal
	
	double fDpm2Mean;		// mean value for D+ signal (K pi pi pi0)
	double fDpm2Sigma;		// sigma value for D+ signal
	
	double fDpm3Mean;		// mean value for D+ signal (KS pi pi0)
	double fDpm3Sigma;		// sigma value for D+ signal
	
	double fDpm4Mean;		// mean value for D+ signal (KS pi pi pi)
	double fDpm4Sigma;		// sigma value for D+ signal
	
	double fDs1Mean;		// mean value for Ds signal (K K pi)
	double fDs1Sigma;		// sigma value for Ds signal
	
	double fDs2Mean;		// mean value for Ds signal (K K pi pi0)
	double fDs2Sigma;		// sigma value for Ds signal
	
	// *** baryons
	double fLamcMean;	// mean value for Lambdac signal
	double fLamcSigma;		// sigma value for Lambda signal

	double fLamMean;		// mean value for Lambda signal
	double fLamSigma;		// sigma value for Lambda signal
	
	// *** charmonia
	double fEtac1Mean;		// mean value for eta_c signal    (K K pi0)
	double fEtac1Sigma;		// sigma value for eta_c signal
	
	double fEtac2Mean;		// mean value for eta_c signal    (KS K pi)    
	double fEtac2Sigma;		// sigma value for eta_c signal
	
	double fEtac3Mean;		// mean value for eta_c signal    (eta pi pi)
	double fEtac3Sigma;		// sigma value for eta_c signal
	
	double fEtac4Mean;		// mean value for eta_c signal    (g g)
	double fEtac4Sigma;		// sigma value for eta_c signal
	
	double fJpsi1Mean;		// mean value for jpsi signal     (e e)
	double fJpsi1Sigma;		// sigma value for jpsi signal
	
	double fJpsi2Mean;		// mean value for jpsi signal     (mu mu)
	double fJpsi2Sigma;		// sigma value for jpsi signal
	
	double fChic01Mean;		// mean value for chic0 signal    (2pi 2pi0)
	double fChic01Sigma;	// sigma value for chic0 signal
	
	double fChic02Mean;		// mean value for chic0 signal    (4pi)
	double fChic02Sigma;	// sigma value for chic0 signal
	
	double fChic03Mean;		// mean value for chic0 signal    (2pi 2K)
	double fChic03Sigma;	// sigma value for chic0 signal
	
	// *** electro-magnetic 
	double f2eMean;			// mean value for e+ e- signal
	double f2eSigma;		// sigma value for for e+ e- signal
	
	double f2muMean;		// mean value for mu+ mu- signal
	double f2muSigma;		// sigma value for for mu+ mu- signal
	
	double f2gamMean;		// mean value for gam gam signal
	double f2gamSigma;		// sigma value for for gam gam signal



	
	// *** general cuts
	double fGammaMinE;		// minimum energy for gamma candidates
	double fPi0MinE;		// minimum energy for pi0 candidates
	double fEtaMinE;		// minimum energy for eta candidates
	double fTrackMinP;		// minimum momentum for charged candidates
	
	
	// *** global mass selectors
	RhoMassParticleSelector 		*fPi0Sel;		// pi0 selector (g g)
	RhoMassParticleSelector 		*fPi0PreSel;	// pi0 pre selector

	RhoMassParticleSelector 		*fEtaSel;		// eta selector (g g)
	RhoMassParticleSelector 		*fEtaPreSel;	// eta pre selector
	
	RhoMassParticleSelector 		*fKs0Sel;		// Ks0 selector (pi pi)
	RhoMassParticleSelector 		*fKs0PreSel;	// Ks0 pre selector

	RhoMassParticleSelector 		*fPhiSel;		// phi selector (K K)
	RhoMassParticleSelector 		*fPhiPreSel;	// phi pre selector

	RhoMassParticleSelector 		*fLamSel;		// Lambda selector (p pi)
	RhoMassParticleSelector 		*fLamPreSel;	// Lambda pre selector
	
	RhoMassParticleSelector 		*fJpsi1Sel;		// Jpsi selector (e+ e-)
	RhoMassParticleSelector 		*fJpsi2Sel;		// Jpsi selector (mu+ mu-)
	RhoMassParticleSelector 		*fJpsiPreSel;	// Jpsi pre selector (all)

	RhoMassParticleSelector 		*fD01Sel;		// D0 selector (K pi)
	RhoMassParticleSelector 		*fD02Sel;		// D0 selector (K pi pi0)
	RhoMassParticleSelector 		*fD03Sel;		// D0 selector (K pi pi pi)
	RhoMassParticleSelector 		*fD0PreSel;		// D0 pre selector (all)

	RhoMassParticleSelector 		*fDpm1Sel;		// D+ selector (K pi pi)
	RhoMassParticleSelector 		*fDpm2Sel;		// D+ selector (K pi pi pi0)
	RhoMassParticleSelector 		*fDpm3Sel;		// D+ selector (KS pi pi0)
	RhoMassParticleSelector 		*fDpm4Sel;		// D+ selector (KS pi pi pi)
	RhoMassParticleSelector 		*fDpmPreSel;	// D+ pre selector (all)
	
	RhoMassParticleSelector 		*fDs1Sel;		// Ds selector (K K pi)
	RhoMassParticleSelector 		*fDs2Sel;		// Ds selector (K K pi pi0)
	RhoMassParticleSelector 		*fDsPreSel;		// Ds pre selector (all)

	RhoMassParticleSelector 		*fEtac1Sel;		// eta_c selector (K K pi0)
	RhoMassParticleSelector 		*fEtac2Sel;		// eta_c selector (KS K pi)
	RhoMassParticleSelector 		*fEtac3Sel;		// eta_c selector (eta pi pi)
	RhoMassParticleSelector 		*fEtac4Sel;		// eta_c selector (g g)
	RhoMassParticleSelector 		*fEtacPreSel;	// eta_c pre selector (all)

	RhoMassParticleSelector 		*fChic01Sel;	// chi_c0 selector (2pi 2pi0)
	RhoMassParticleSelector 		*fChic02Sel;	// chi_c0 selector (4pi)
	RhoMassParticleSelector 		*fChic03Sel;	// chi_c0 selector (2pi 2K)
 	RhoMassParticleSelector 		*fChic0PreSel;	// chi_c0 pre selector (all)
	
	RhoMassParticleSelector 		*fLamcSel;		// Lamc selector (p K pi)
	RhoMassParticleSelector 		*fLamcPreSel;	// Lamc pre selector

	RhoMassParticleSelector 		*f2eSel;		// e+ e- selector
	RhoMassParticleSelector 		*f2ePreSel;		// e+ e- pre selector

	RhoMassParticleSelector 		*f2muSel;		// mu+ mu- selector
	RhoMassParticleSelector 		*f2muPreSel;	// mu+ mu- pre selector

	RhoMassParticleSelector 		*f2gamSel;		// gam gam selector
	RhoMassParticleSelector 		*f2gamPreSel;	// gam gam pre selector

	// *** global kin selectors
	RhoMomentumParticleSelector 	*fMomentumSel;  // cut for minimum p
	RhoEnergyParticleSelector   	*fEnergySel;    // cut for minimum E

	// *** pid algos
	TString fAlgoElectron;	// Pid algo definition string electrons
	TString fAlgoMuon;		// Pid algo definition string muons
	TString fAlgoPion;		// Pid algo definition string pions
	TString fAlgoKaon;		// Pid algo definition string kaons
	TString fAlgoProton;	// Pid algo definition string protons
	
	double  fIniPidCut;		// Tightness of initial PID (cut on probability)
	
	
	// *** declare NTuples
	RhoTuple *ntp;			// overall info
	RhoTuple *nks0;			// Ks0 QA
	RhoTuple *npi0;			// pi0 QA
	RhoTuple *neta;			// eta QA
	
	// BR include BRs from subdecay if necessary (KS, pi0, eta,...)
	//  eta -> gg      : 39%
	//  K0 -> pi+ pi-  : 50% * 70% = 35%
	//
	//							channel						   BR       sim modes
	// ----------------------------------------------------------------------
	RhoTuple *nphi;			// phi -> K+ K-					(49.8 %)    (000)
	RhoTuple *nlam;			// Lambda -> p pi-				(63.9 %)    (400)
	RhoTuple *njpsi1;		// J/psi -> e+e- 				( 5.9 %)    (200)
	RhoTuple *njpsi2;		// J/psi -> mu+ mu-				( 5.9 %)    (201)
	
	RhoTuple *nd01;			// D0 -> K- pi+					( 3.9 %)    (100, 103, 106)
	RhoTuple *nd02;			// D0 -> K- pi+ pi0				(13.9 %)    (101, 104, 107)
	RhoTuple *nd03;			// D0 -> K- pi+ pi+ pi-			( 8.1 %)    (102, 105, 108)
	//													   ---------
	//									Sum D0 BR			(25.9 %)
	
	RhoTuple *ndpm1;		// D+- -> K- pi+ pi+			( 9.4 %)    (120, 124, 128)
	RhoTuple *ndpm2;		// D+- -> K- pi+ pi+ pi0		( 6.1 %)    (121, 125, 129)
	RhoTuple *ndpm3;		// D+- -> K_S pi+ pi0			( 6.9 %)    (122, 126, 130)
	RhoTuple *ndpm4;		// D+- -> K_S pi+ pi+ pi-		( 3.1 %)    (123, 127, 131) 
	//													   ---------
	//									Sum D+- BR			(25.5 %)
	
	RhoTuple *nds1;			// Ds -> K+ K- pi+				( 5.5 %)    (140, 142, 144)
	RhoTuple *nds2;			// Ds -> K+ K- pi+ pi0			( 5.6 %)    (141, 143, 145)
	//													   ---------
	//									Sum Ds BR			(11.1 %)
	
	RhoTuple *nlamc;		// Lambda_c -> p K- pi+			( 5.0 %)    (420)
	
	RhoTuple *netac1;		// eta_c -> K+ K- pi0 			( below)    (220)
	RhoTuple *netac2;		// eta_c -> K+- K_S pi-+        ( 3.2 %)    (221)    = 7%/3 (1) + 7%/3*35% (2)
	RhoTuple *netac3;		// eta_c -> eta (gg) pi+ pi-	( 1.3 %)    (222)    = 4.9% * 39 % * 66%
	RhoTuple *netac4;		// eta_c -> gg					( 0.0 %)    (223)    = 6e-5
	//													   ---------
	//									Sum eta_c BR		( 4.5 %)

	RhoTuple *nchic01;		// chi_c0 -> pi+ pi- pi0 pi0	( 3.4 %)    (240)
	RhoTuple *nchic02;		// chi_c0 -> pi+ pi- pi+ pi-	( 2.3 %)    (241)
	RhoTuple *nchic03;		// chi_c0 -> pi+ pi- K+  K-		( 1.8 %)    (242)
	//													   --------- 
	//									Sum chi_c0 BR		( 7.5 %)
	
	RhoTuple *n2e;			// pbar p -> e+ e-                          (300)
	RhoTuple *n2mu;			// pbar p -> mu+ mu-                        (301)
	RhoTuple *n2gam;		// pbar p -> gam gam                        (302)


	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	// *** simpler vertexer
	PndVtxPoca  *fVtxPoca;
	
	// *** Pointer to event shape object
	PndEventShape *fEventShape;
	
	// *** RhoTuple QA helper class
	PndRhoTupleQA *fQA;
	
	// PDG database object
	TDatabasePDG  *fPdg;
	
	// *** global RhoCandLists
	RhoCandList fMcTruth;
	RhoCandList fAllCands;
	RhoCandList fChargedCands;
	RhoCandList fNeutralCands;
	RhoCandList fGammaCands;
		
	RhoCandList fElectronPlus;
	RhoCandList fElectronMinus;
	
	RhoCandList fMuonPlus;
	RhoCandList fMuonMinus;
	
	RhoCandList fPionPlus;
	RhoCandList fPionMinus;
	
	RhoCandList fKaonPlus;
	RhoCandList fKaonMinus;
	
	RhoCandList fProtonPlus;
	RhoCandList fProtonMinus;
	
	RhoCandList fKs0Cands;
	RhoCandList fPi0Cands;
	RhoCandList fEtaCands;
	
	
	// *** Get parameter containers
	virtual void SetParContainers();
	
    FairRootManager*   fRootManager;
    TClonesArray* fTcaOnlineFilterInfo;
	
	ClassDef(PndSoftTriggerTask,1);
  
};

#endif
