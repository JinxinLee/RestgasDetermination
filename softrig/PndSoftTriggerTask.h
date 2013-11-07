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
	
	// *** set selection parameters for invariant mass cuts
	void SetPi0Selection(double m, double w) {fPi0SelMean=m; fPi0SelWin=w;}
	void SetKs0Selection(double m, double w) {fKs0SelMean=m; fKs0SelWin=w;}
	void SetEtaSelection(double m, double w) {fKs0SelMean=m; fKs0SelWin=w;}

	void SetPhiSelection(double m, double w) {fPhiSelMean=m; fPhiSelWin=w;}
	
	void SetD0Selection(double m, double w) {fD0SelMean=m; fD0SelWin=w;}
	void SetDpmSelection(double m, double w) {fDpmSelMean=m; fDpmSelWin=w;}
	void SetDsSelection(double m, double w) {fDsSelMean=m; fDsSelWin=w;}
	
	void SetLambdaSelection(double m, double w) {fLamSelMean=m; fLamSelWin=w;}
	void SetLambdacSelection(double m, double w) {fLamcSelMean=m; fLamcSelWin=w;}

	void SetJpsiSelection(double m, double w) {fJpsiSelMean=m; fJpsiSelWin=w;}
	void SetEtacSelection(double m, double w) {fEtacSelMean=m; fEtacSelWin=w;}
	void SetChic0Selection(double m, double w) {fChic0SelMean=m; fChic0SelWin=w;}
	
	void SetGammaMinE(double min) {fGammaMinE=min;}
	void SetTrackMinP(double min) {fTrackMinP=min;}
	
	void SetAlgoElectron(TString algo) {fAlgoElectron=algo;}
	void SetAlgoMuon(TString algo) {fAlgoMuon=algo;}
	void SetAlgoPion(TString algo) {fAlgoPion=algo;}
	void SetAlgoKaon(TString algo) {fAlgoKaon=algo;}
	void SetAlgoProton(TString algo) {fAlgoProton=algo;}
	
	void SetInitialPidCut(double cut) {fIniPidCut=cut;}
	
	// *** Switch Tagging methods
	void SetTag_Phi_KK(bool tag) { fTagPhiKK = tag;}
	void SetTag_Lambda_ppi(bool tag) { fTagLamppi = tag;}
	void SetTag_Jpsi_ll(bool tag) { fTagJpsill = tag;}
	
	void SetTag_D0_Kpi(bool tag) { fTagD0Kpi = tag;}
	void SetTag_D0_Kpipi0(bool tag) { fTagD0Kpipi0 = tag;}
	void SetTag_D0_K3pi(bool tag) { fTagD0K3pi = tag;}
	
	void SetTag_Dpm_Kpipi(bool tag) { fTagDpmKpipi = tag;}
	void SetTag_Dpm_K2pipi0(bool tag) { fTagDpmK2pipi0 = tag;}
	void SetTag_Dpm_Kspipi0(bool tag) { fTagDpmKspipi0 = tag;}
	void SetTag_Dpm_Ks3pi(bool tag) { fTagDpmKs3pi = tag;}
	
	void SetTag_Ds_KKpi(bool tag) { fTagDsKKpi = tag;}
	void SetTag_Ds_KKpipi0(bool tag) { fTagDsKKpip0 = tag;}
	
	void SetTag_Lambdac_pKpi(bool tag) { fTagLamcpKpi = tag;}
	
	void SetTag_Etac_KKpi0(bool tag) { fTagEtacKKpi0 = tag;}
	void SetTag_Etac_KKspi(bool tag) { fTagEtacKKspi = tag;}
	void SetTag_Etac_etapipi(bool tag) { fTagEtacetapipi = tag;}
	void SetTag_Etac_gg(bool tag) { fTagEtacgg = tag;}
	
	void SetTag_Chic0_2pi2pi0(bool tag) { fTagChic02pi2pi0 = tag;}
	void SetTag_Chic0_4pi(bool tag) { fTagChic04pi = tag;}
	
	// *** Enable/Disable QA output
	void SetQA_Phi_KK(bool qa=true) { fQAPhiKK = qa;}
	void SetQA_Lambda_ppi(bool qa=true) { fQALamppi = qa;}
	void SetQA_Jpsi_ll(bool qa=true) { fQAJpsill = qa;}
	
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
	
	void SetQA_Etac_KKpi0(bool qa=true) { fQAEtacKKpi0 = qa;}
	void SetQA_Etac_KKspi(bool qa=true) { fQAEtacKKspi = qa;}
	void SetQA_Etac_etapipi(bool qa=true) { fQAEtacetapipi = qa;}
	void SetQA_Etac_gg(bool qa=true) { fQAEtacgg = qa;}
	
	void SetQA_Chic0_2pi2pi0(bool qa=true) { fQAChic02pi2pi0 = qa;}
	void SetQA_Chic0_4pi(bool qa=true) { fQAChic04pi = qa;}
	
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
	void CountPidCutMult(double prob, int &ne, int &nmu, int &npi, int &nk, int &np, double mom=0.0);
	double DbMass(TString name) {if (fPdg->GetParticle(name)) return fPdg->GetParticle(name)->Mass();}
	
	// *** QA methods 
	// *** QA for candidates
	void qaCand(TString pre, RhoCandidate *cc, RhoTuple *n, bool skip=false);
	void qaP4(TString pre, TLorentzVector &c, RhoTuple *n, bool skip=false);
	void qaP4Cms(TString pre, TLorentzVector c, RhoTuple *n, bool skip=false);
	
	// *** QA for 2-prong
	void qaComp(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaKs0(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaPi0(TString pre, RhoCandidate *c, RhoTuple *n);
	
	// *** QA of event shape
	void qaEventShape(RhoTuple *n);
	void qaEventShapeShort(RhoTuple *n);

	// *** QA track, vtx, PID, decay
	void qaVtx(TString pre, RhoCandidate *c, RhoTuple *n);
	void qa2Body(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaTrk(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaPid(TString pre, RhoCandidate *c, RhoTuple *n);
	
	// *** QA PndPidCandidate raw values
	void qaEmc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaMvd(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaStt(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaDrc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaDsc(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaTof(TString pre, RhoCandidate *c, RhoTuple *n);
	void qaMuo(TString pre, RhoCandidate *c, RhoTuple *n);
	
	// *** Created necessary composites
	int CreateKs0Cands(RhoTuple *n);
	
	// *** Tagging methods
	int Tag_Phi_KK(RhoTuple *n);
	int Tag_Lambda_ppi(RhoTuple *n);
	int Tag_Jpsi_ll(RhoTuple *n);
	
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
	
	
	// *** global vars
	int fMode;	
	int fEvtCount;	
	int fSigCount;	
	TLorentzVector fIniP4;
	double fEcm;
	
	// *** flags for enable/disable tagging for individual channels
	bool fTagPhiKK;
	bool fTagLamppi;
	bool fTagJpsill;
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
	
	
	// *** flags for enable/disable QA ntuple output
	bool fQAPhiKK;
	bool fQALamppi;
	bool fQAJpsill;
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
	bool fQAPi0;
	bool fQAEta;
	bool fQAKs0;
	bool fQAEvent;
	
	// *** parameters
	double fPi0SelMean;		// mean value for pi0 selection
	double fPi0SelWin;		// window for pi0 selection
	
	double fEtaSelMean;		// mean value for eta(gg) selection
	double fEtaSelWin;		// window for eta(gg) selection
	
	double fKs0SelMean;		// mean value for Ks selection
	double fKs0SelWin;		// window for Ks selection
	
	// *** resonances
	double fPhiSelMean;		// mean value for phi selection
	double fPhiSelWin;		// window for phi selection
	
	// *** open charm
	double fD0SelMean;		// mean value for D0 selection
	double fD0SelWin;		// window for D0 selection
	
	double fDpmSelMean;		// mean value for D+ selection
	double fDpmSelWin;		// window for D+ selection
	
	double fDsSelMean;		// mean value for Ds selection
	double fDsSelWin;		// window for Ds selection
	
	// *** baryons
	double fLamcSelMean;	// mean value for Lambdac selection
	double fLamcSelWin;		// window for Lambda selection

	double fLamSelMean;		// mean value for Lambda selection
	double fLamSelWin;		// window for Lambda selection
	
	// *** charmonia
	double fEtacSelMean;	// mean value for eta_c selection
	double fEtacSelWin;		// window for eta_c selection
	
	double fJpsiSelMean;	// mean value for jpsi selection
	double fJpsiSelWin;		// window for jpsi selection
	
	double fChic0SelMean;	// mean value for chic0 selection
	double fChic0SelWin;	// window for chic0 selection
	
	// *** general cuts
	double fGammaMinE;		// minimum energy for gamma candidates
	double fPi0MinE;		// minimum energy for pi0 candidates
	double fEtaMinE;		// minimum energy for eta candidates
	double fTrackMinP;		// minimum momentum for charged candidates
	
	// *** global mass selectors
	RhoMassParticleSelector 		*fPi0Sel;		// pi0 selector
	RhoMassParticleSelector 		*fPi0PreSel;	// pi0 pre selector

	RhoMassParticleSelector 		*fEtaSel;		// eta selector
	RhoMassParticleSelector 		*fEtaPreSel;	// eta pre selector
	
	RhoMassParticleSelector 		*fKs0Sel;		// Ks0 selector
	RhoMassParticleSelector 		*fKs0PreSel;	// Ks0 pre selector

	RhoMassParticleSelector 		*fPhiSel;		// phi selector
	RhoMassParticleSelector 		*fPhiPreSel;	// phi pre selector

	RhoMassParticleSelector 		*fLamSel;		// Lambda selector
	RhoMassParticleSelector 		*fLamPreSel;	// Lambda pre selector
	
	RhoMassParticleSelector 		*fJpsiSel;		// Jpsi selector
	RhoMassParticleSelector 		*fJpsiPreSel;	// Jpsi pre selector

	RhoMassParticleSelector 		*fD0Sel;		// D0 selector
	RhoMassParticleSelector 		*fD0PreSel;		// D0 pre selector

	RhoMassParticleSelector 		*fDpmSel;		// D+ selector
	RhoMassParticleSelector 		*fDpmPreSel;	// D+ pre selector
	
	RhoMassParticleSelector 		*fDsSel;		// Ds selector
	RhoMassParticleSelector 		*fDsPreSel;		// Ds pre selector

	RhoMassParticleSelector 		*fEtacSel;		// eta_c selector
	RhoMassParticleSelector 		*fEtacPreSel;	// eta_c pre selector

	RhoMassParticleSelector 		*fChic0Sel;		// chi_c0 selector
	RhoMassParticleSelector 		*fChic0PreSel;	// chi_c0 pre selector
	
	RhoMassParticleSelector 		*fLamcSel;		// Lamc selector
	RhoMassParticleSelector 		*fLamcPreSel;	// Lamc pre selector

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
	//							channel						   BR       mode 
	// ----------------------------------------------------------------------
	RhoTuple *nphi;			// phi -> K+ K-					(49.8 %)    (00)
	RhoTuple *nlam;			// Lambda -> p pi-				(63.9 %)    (31)
	RhoTuple *njpsi;		// J/psi -> e+e- / mu+ mu-		(11.9 %)    (21)
	
	RhoTuple *nd01;			// D0 -> K- pi+					( 3.9 %)    (18)
	RhoTuple *nd02;			// D0 -> K- pi+ pi0				(13.9 %)   (182)
	RhoTuple *nd03;			// D0 -> K- pi+ pi+ pi-			( 8.1 %)   (183)
	//													   ---------
	//									Sum D0 BR			(25.9 %)
	
	RhoTuple *ndpm1;		// D+- -> K- pi+ pi+			( 9.4 %)    (14)
	RhoTuple *ndpm2;		// D+- -> K- pi+ pi+ pi0		( 6.1 %)   (142)
	RhoTuple *ndpm3;		// D+- -> K_S pi+ pi0			( 6.9 %)   (143)
	RhoTuple *ndpm4;		// D+- -> K_S pi+ pi+ pi-		( 3.1 %)   (144) 
	//													   ---------
	//									Sum D+- BR			(25.5 %)
	
	RhoTuple *nds1;			// Ds -> K+ K- pi+				( 5.5 %)    (17)
	RhoTuple *nds2;			// Ds -> K+ K- pi+ pi0			( 5.6 %)   (172)
	//													   ---------
	//									Sum Ds BR			(11.1 %)
	
	RhoTuple *nlamc;		// Lambda_c -> p K- pi+			( 5.0 %)    (34)
	
	RhoTuple *netac1;		// eta_c -> K+ K- pi0 			( below)    (70)
	RhoTuple *netac2;		// eta_c -> K+- K_S pi-+        ( 3.2 %)    (71)    = 7%/3 (1) + 7%/3*35% (2)
	RhoTuple *netac3;		// eta_c -> eta (gg) pi+ pi-	( 1.3 %)    (72)    = 4.9% * 39 % * 66%
	RhoTuple *netac4;		// eta_c -> gg					( 0.0 %)    (73)    = 6e-5
	//													   ---------
	//									Sum eta_c BR		( 4.5 %)

	RhoTuple *nchic01;		// chi_c0 -> pi+ pi- pi0 pi0	( 3.4 %)    (80)
	RhoTuple *nchic02;		// chi_c0 -> pi+ pi- pi+ pi-	( 2.3 %)    (81)
	//													   --------- 
	//									Sum chi_c0 BR		( 5.7 %)


	// *** the PndAnalysis object
	PndAnalysis *fAnalysis;
	
	// *** simpler vertexer
	PndVtxPoca  *fVtxPoca;
	
	// *** Pointer to event shape object
	PndEventShape *fEventShape;
	
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
