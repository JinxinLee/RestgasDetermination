// ************************************************************************
//
// Online SoftTrigger reco and tagging for several channels 
//
// K.Goetzen 10/2013
// ************************************************************************


// The header file
#include "PndSoftTriggerTask.h"

// Package headers
#include "PndOnlineFilterInfo.h"

// C++ headers
#include <string>
#include <iostream>

// FAIR headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// ROOT headers
#include "TClonesArray.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

// RHO headers
#include "RhoCandidate.h"
#include "RhoHistogram/RhoTuple.h"
#include "RhoFactory.h"
#include "RhoMassParticleSelector.h"
#include "RhoMomentumParticleSelector.h"
#include "RhoEnergyParticleSelector.h"
#include "RhoTuple.h"

// Analysis headers
#include "PndAnalysis.h"
#include "Pnd4CFitter.h"
#include "PndKinVtxFitter.h"
#include "PndChiVtxFitter.h"
#include "PndVtxPRG.h"
#include "PndKinFitter.h"
#include "PndVtxPoca.h"
#include "PndPidCandidate.h"
#include "PndEventShape.h"
#include "PndRhoTupleQA.h"
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndSoftTriggerTask::PndSoftTriggerTask(double pmom, int mode, int runnum) :
	FairTask("Panda Softtrigger Task"),
	fMode(mode), fEvtCount(0), fRunNum(runnum), fSigCount(0), fNsigTag(8.0),	fNsigAux(5.0),	
	fIniP4(0,0,0,0), fEcm(0.), fPbarMom(pmom),
	fTagPhiKK(true), fTagLamppi(true), fTagJpsi2e(true), fTagJpsi2mu(true),
	fTagD0Kpi(true), fTagD0Kpipi0(true), fTagD0K3pi(true),
	fTagDpmKpipi(true), fTagDpmK2pipi0(true), fTagDpmKspipi0(true),
	fTagDpmKs3pi(true),	fTagDsKKpi(true), fTagDsKKpip0(true),
	fTagLamcpKpi(true), fTagEtacKKpi0(true), fTagEtacKKspi(true),
	fTagEtacetapipi(true), fTagEtacgg(true), fTagChic02pi2pi0(true), fTagChic04pi(true),fTagChic02pi2K(true),
	fTag2e(true),fTag2mu(true),fTag2gam(true),
	fQAPhiKK(false), fQALamppi(false), fQAJpsi2e(false),fQAJpsi2mu(false),
	fQAD0Kpi(false), fQAD0Kpipi0(false), fQAD0K3pi(false),
	fQADpmKpipi(false), fQADpmK2pipi0(false), fQADpmKspipi0(false),
	fQADpmKs3pi(false),	fQADsKKpi(false), fQADsKKpip0(false),
	fQALamcpKpi(false), fQAEtacKKpi0(false), fQAEtacKKspi(false),
	fQAEtacetapipi(false), fQAEtacgg(false), fQAChic02pi2pi0(false), fQAChic04pi(false), fQAChic02pi2K(false), 
	fQA2e(false),fQA2mu(false),fQA2gam(false),
	fQAPi0(false),fQAEta(false),fQAKs0(false),fQAEvent(false),
	fGammaMinE(0.03), fTrackMinP(0.15),
	fEventShape(NULL), 
	fQA(NULL),
	fPi0Sel(NULL), fKs0Sel(NULL),
	fMomentumSel(NULL), fEnergySel(NULL),
	ntp(0),	nks0(0), npi0(0), neta(0),		
	nphi(0), nlam(0), njpsi1(0), njpsi2(0),	
	nd01(0), nd02(0), nd03(0),	
	ndpm1(0), ndpm2(0), ndpm3(0), ndpm4(0),		
	nds1(0), nds2(0),			
	nlamc(0),		
	netac1(0), netac2(0), netac3(0), netac4(0),		
	nchic01(0),	nchic02(0), nchic03(0),
	n2e(0), n2mu(0), n2gam(0)
{
	fPdg = TDatabasePDG::Instance();
	double mp = fPdg->GetParticle("proton")->Mass(); //Proton mass for computation of p4_ini
	
	// set 4-vector of pbar-p-system
	fIniP4.SetPz(pmom);
	fIniP4.SetE(sqrt(pmom*pmom+mp*mp)+mp);
	fEcm = fIniP4.M();
	
	// set default algorithms for pid
	fAlgoElectron	= "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts";
	fAlgoMuon		= fAlgoElectron;
	fAlgoPion		= fAlgoElectron; 
	fAlgoKaon		= fAlgoElectron;
	fAlgoProton		= fAlgoElectron; 
	
	fIniPidCut		= 0.0; 
	
	// *** set default signal parameters (mean, sigma)
	SetSignalParamsDefaults();
	SetQASelectionDefaults();	
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSoftTriggerTask::~PndSoftTriggerTask() { }
// -------------------------------------------------------------------------


// ----Defaul parameters for QA--------------------------------------------------------------
void PndSoftTriggerTask::SetQASelectionDefaults()
{
	double Pi0Mass = DbMass("pi0");
	double EtaMass = DbMass("eta");
	double Ks0Mass = DbMass("K_S0");
	
	double PhiMass = DbMass("phi");
	
	double D0Mass  = DbMass("D0");
	double DpmMass = DbMass("D+");
	double DsMass  = DbMass("D_s+");
	
	double JpsiMass  = DbMass("J/psi");
	double EtacMass  = DbMass("eta_c");
	double Chic0Mass = DbMass("chi_0c");
	
	double LamMass  = DbMass("Lambda0");
	double LamcMass = DbMass("Lambda_c+");
	
	// default windows are roughly +-15*sigma, and 15*sigma_max, if different channels are reconstructed
	// should not depend explicitly from set mean and sigma values, therefore fixed values are chosen
	
	fPi0QaMin = Pi0Mass - 0.1;		
	fPi0QaMax = Pi0Mass + 0.1;		
	
	fEtaQaMin = EtaMass - 0.2;		
	fEtaQaMax = EtaMass + 0.2;		
	
	fKs0QaMin = Ks0Mass - 0.2;		
	fKs0QaMax = Ks0Mass + 0.2;		
	
	// *** resonances
	fPhiQaMin = PhiMass - 0.1;		
	fPhiQaMax = PhiMass + 0.1;		
	
	// *** open charm
	fD0QaMin = D0Mass - 0.3;		
	fD0QaMax = D0Mass + 0.3;		
	
	fDpmQaMin = DpmMass - 0.3;		
	fDpmQaMax = DpmMass + 0.3;		
	
	fDsQaMin = DsMass - 0.25;		
	fDsQaMax = DsMass + 0.25;		
	
	// *** baryons
	fLamcQaMin = LamcMass - 0.2;		
	fLamcQaMax = LamcMass + 0.2;		

	fLamQaMin = LamMass - 0.15;		
	fLamQaMax = LamMass + 0.15;		
	
	// *** charmonia
	fEtacQaMin = EtacMass - 0.65;		
	fEtacQaMax = EtacMass + 0.65;		
	
	fJpsiQaMin = JpsiMass - 1.00;		
	fJpsiQaMax = JpsiMass + 0.75;		
	
	fChic0QaMin = Chic0Mass - 0.6;		
	fChic0QaMax = Chic0Mass + 0.6;		
	
	// *** electro-magnetic 
	f2eQaMin = fEcm - 1.5;		
	f2eQaMax = fEcm + 1.0;		
	
	f2muQaMin = fEcm - 1.5;		
	f2muQaMax = fEcm + 1.0;		
	
	f2gamQaMin = fEcm - 1.0;		
	f2gamQaMax = fEcm + 1.0;		
}

// ----Defaul signal parameters --------------------------------------------------------------
void PndSoftTriggerTask::SetSignalParamsDefaults()
{
	// *** fitted peak values from previous studies
	
	// In case signals are not gaussian (sometimes double-gauss or with tail), the narrow core distribution
	// was fitted to extract a guassian sigma. Should be taken as lower estimate
/*	
Phi					0,0037
ee					0,1000
Lamba					0,0041
etac					0,0355
Jpsi e					0,0700
Jpsi mu					0,0448
D+					0,0160
D0					0,0215
Ds					0,0141
Lamc					0,0110
pi0					0,0043
Ks					0,0077
*/
	
	fPi0Mean = 0.136;		// mean value for pi0 signal
	fPi0Sigma = 0.0045;		// sigma value for pi0 signal
	
	fEtaMean = 0.552;		// mean value for eta(gg) signal
	fEtaSigma = 0.009;		// sigma value for eta(gg) signal
	
	fKs0Mean = 0.497;		// mean value for Ks signal
	fKs0Sigma = 0.008;		// sigma value for Ks signal
	
	// *** resonances
	fPhiMean = 1.0195;		// mean value for phi signal
	fPhiSigma = 0.004;		// sigma value for phi signal
	
	// *** open charm
	fD01Mean = 1.863;		// mean value for D0 signal (K pi)
	fD01Sigma = 0.022;		// sigma value for D0 signal
	
	fD02Mean = 1.866;		// mean value for D0 signal (K pi pi0)
	fD02Sigma = 0.019;		// sigma value for D0 signal
	
	fD03Mean = 1.863;		// mean value for D0 signal (K pi pi pi)
	fD03Sigma = 0.017;		// sigma value for D0 signal
	
	fDpm1Mean = 1.868;		// mean value for D+ signal (K pi pi)
	fDpm1Sigma = 0.016;		// sigma value for D+ signal
	
	fDpm2Mean = 1.870;		// mean value for D+ signal (K pi pi pi0)
	fDpm2Sigma = 0.019;		// sigma value for D+ signal
	
	fDpm3Mean = 1.873;		// mean value for D+ signal (KS pi pi0)
	fDpm3Sigma = 0.020;		// sigma value for D+ signal
	
	fDpm4Mean = 1.867;		// mean value for D+ signal (KS pi pi pi)
	fDpm4Sigma = 0.017;		// sigma value for D+ signal
	
	fDs1Mean = 1.967;		// mean value for Ds signal (K K pi)
	fDs1Sigma = 0.0145;		// sigma value for Ds signal
	
	fDs2Mean = 1.970;		// mean value for Ds signal (K K pi pi0)
	fDs2Sigma = 0.016;		// sigma value for Ds signal
	
	// *** baryons
	fLamcMean = 2.284;	// mean value for Lambdac signal
	fLamcSigma = 0.011;		// sigma value for Lambda signal

	fLamMean = 1.116;		// mean value for Lambda signal
	fLamSigma = 0.0045;		// sigma value for Lambda signal
	
	// *** charmonia
	fEtac1Mean = 2.981;		// mean value for eta_c signal    (K K pi0)
	fEtac1Sigma = 0.042;		// sigma value for eta_c signal
	
	fEtac2Mean = 2.976;		// mean value for eta_c signal    (KS K pi)    
	fEtac2Sigma = 0.036;		// sigma value for eta_c signal
	
	fEtac3Mean = 2.980;		// mean value for eta_c signal    (eta pi pi)
	fEtac3Sigma = 0.042;		// sigma value for eta_c signal
	
	fEtac4Mean = 2.999;		// mean value for eta_c signal    (g g)
	fEtac4Sigma = 0.041;		// sigma value for eta_c signal
	
	fJpsi1Mean = 3.070;		// mean value for jpsi signal     (e e)
	fJpsi1Sigma = 0.1;		// sigma value for jpsi signal
	
	fJpsi2Mean = 3.094;		// mean value for jpsi signal     (mu mu)
	fJpsi2Sigma = 0.045;		// sigma value for jpsi signal
	
	fChic01Mean = 3.421;		// mean value for chic0 signal    (2pi 2pi0)
	fChic01Sigma = 0.040;	// sigma value for chic0 signal
	
	fChic02Mean = 3.412;		// mean value for chic0 signal    (4pi)
	fChic02Sigma = 0.037;	// sigma value for chic0 signal
	
	fChic03Mean = 3.414;		// mean value for chic0 signal    (2pi 2K)
	fChic03Sigma = 0.04;	// sigma value for chic0 signal
	
	// *** electro-magnetic 
	f2eMean = fEcm;			// mean value for e+ e- signal
	f2eSigma = 0.125;		// sigma value for for e+ e- signal
	
	f2muMean = fEcm;		// mean value for mu+ mu- signal
	f2muSigma = 0.9;		// sigma value for for mu+ mu- signal
	
	f2gamMean = fEcm;		// mean value for gam gam signal
	f2gamSigma = 0.65;		// sigma value for for gam gam signal
	
}

// ----Method to enable/disable full QA--------------------------------------------------------------
void PndSoftTriggerTask::SetQA_All(bool qa)
{
	SetQA_Phi_KK(qa);
	SetQA_Lambda_ppi(qa);
	SetQA_Jpsi_2e(qa);
	SetQA_Jpsi_2mu(qa);
	
	SetQA_D0_Kpi(qa);
	SetQA_D0_Kpipi0(qa);
	SetQA_D0_K3pi(qa);
	
	SetQA_Dpm_Kpipi(qa);
	SetQA_Dpm_K2pipi0(qa);
	SetQA_Dpm_Kspipi0(qa);
	SetQA_Dpm_Ks3pi(qa);
	
	SetQA_Ds_KKpi(qa);
	SetQA_Ds_KKpipi0(qa);
	
	SetQA_Lambdac_pKpi(qa);
	
	SetQA_Etac_KKpi0(qa);
	SetQA_Etac_KKspi(qa);
	SetQA_Etac_etapipi(qa);
	SetQA_Etac_gg(qa); 
	
	SetQA_Chic0_2pi2pi0(qa); 
	SetQA_Chic0_4pi(qa); 
	SetQA_Chic0_2pi2K(qa); 

	SetQA_2e(qa); 
	SetQA_2mu(qa); 
	SetQA_2gam(qa); 
	
	SetQA_Pi0(qa);
	SetQA_Eta(qa);
	SetQA_Ks0(qa);
	SetQA_Event(qa);
}

// ----Method to enable/disable full Tagging--------------------------------------------------------------
void PndSoftTriggerTask::SetTag_All(bool qa)
{
	SetTag_Phi_KK(qa);
	SetTag_Lambda_ppi(qa);
	SetTag_Jpsi_2e(qa);
	SetTag_Jpsi_2mu(qa);
	
	SetTag_D0_Kpi(qa);
	SetTag_D0_Kpipi0(qa);
	SetTag_D0_K3pi(qa);
	
	SetTag_Dpm_Kpipi(qa);
	SetTag_Dpm_K2pipi0(qa);
	SetTag_Dpm_Kspipi0(qa);
	SetTag_Dpm_Ks3pi(qa);
	
	SetTag_Ds_KKpi(qa);
	SetTag_Ds_KKpipi0(qa);
	
	SetTag_Lambdac_pKpi(qa);
	
	SetTag_Etac_KKpi0(qa);
	SetTag_Etac_KKspi(qa);
	SetTag_Etac_etapipi(qa);
	SetTag_Etac_gg(qa); 
	
	SetTag_Chic0_2pi2pi0(qa); 
	SetTag_Chic0_4pi(qa); 
	SetTag_Chic0_2pi2K(qa); 

	SetTag_2e(qa); 
	SetTag_2mu(qa); 
	SetTag_2gam(qa); 
}


// ----- Method to select true PID candidates
int PndSoftTriggerTask::SelectTruePid(RhoCandList &l)
{
	int removed = 0;
	
	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if ( !(fAnalysis->McTruthMatch(l[ii])) )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	
	return removed;
}
// -------------------------------------------------------------------------

// ----- Method to select candidate with certain PID probability
int PndSoftTriggerTask::SelectPidProb(RhoCandList &l, int pididx, double cut)
{
	int removed = 0;
	
	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if (pididx>-1 && pididx<5 && l[ii]->GetPidInfo(pididx)<cut )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	
	return removed;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSoftTriggerTask::Init() 
{		
	fRootManager = FairRootManager::Instance();
	
	// Register TCA for tagging info
    fTcaOnlineFilterInfo = new TClonesArray ( "PndOnlineFilterInfo" );
	if (fRootManager)
	{
		fRootManager->Register ( "OnlineFilterInfo","PndOnlineFolder", fTcaOnlineFilterInfo, kTRUE );
	}
	
	// *** add several pbar p/n/dd Systems for MC truth match
	double ppwidth = 0.01;
	fPdg->AddParticle("pbarpSystem", "pbar p", fEcm, false, ppwidth,0,"",88888);
	fPdg->AddParticle("pbarpSystem0","pbar p", fEcm, false, ppwidth,0,"",88880);
	fPdg->AddParticle("pbarpSystem1","pbar p", fEcm, false, ppwidth,0,"",88881);
	fPdg->AddParticle("pbarpSystem2","pbar p", fEcm, false, ppwidth,0,"",88882);
	
	fPdg->AddParticle("pbarnSystem", "pbar n", fEcm, false, ppwidth,0,"",88887);
	fPdg->AddParticle("pbardSystem", "pbar d", fEcm, false, ppwidth,0,"",88889);
	
	// *** initialize analysis object
	fAnalysis = new PndAnalysis();
	
	// *** vertexer
	fVtxPoca = new PndVtxPoca();
	
	// *** RhoTuple QA helper
	fQA = new PndRhoTupleQA(fAnalysis,fPbarMom);
	
	// *** create ntuple
	if (fQAEvent) ntp  = new RhoTuple("ntp","Soft Trigger Common");
	if (fQAKs0) nks0 = new RhoTuple("nks0","K_S -> pi+ pi-");
	if (fQAPi0) npi0 = new RhoTuple("npi0","pi0 -> g g");
	if (fQAEta) neta = new RhoTuple("neta","eta -> g g");
	
	if (fQAPhiKK) nphi = new RhoTuple("nphi",			"phi -> K+ K-");	
	if (fQALamppi) nlam = new RhoTuple("nlam",			"Lambda -> p pi");	
	if (fQAJpsi2e) njpsi1 = new RhoTuple("njpsi1",		"J/psi -> e+ e-");	
	if (fQAJpsi2mu) njpsi2 = new RhoTuple("njpsi2",		"J/psi -> mu+ mu-");	
	
	if (fQAD0Kpi) nd01 = new RhoTuple("nd01",			"D0 -> K- pi+");		
	if (fQAD0Kpipi0) nd02 = new RhoTuple("nd02",			"D0 -> K- pi+ pi0");		
	if (fQAD0K3pi) nd03 = new RhoTuple("nd03",			"D0 -> K- pi+ pi- pi-");		
	
	if (fQADpmKpipi) ndpm1 = new RhoTuple("ndpm1",		"D+- -> K- pi+ pi+");		
	if (fQADpmK2pipi0) ndpm2 = new RhoTuple("ndpm2",		"D+- -> K- pi+ pi+ pi0");	
	if (fQADpmKspipi0) ndpm3 = new RhoTuple("ndpm3",		"D+- -> K_S pi+ pi0");		
	if (fQADpmKs3pi) ndpm4 = new RhoTuple("ndpm4",		"D+- -> K_S pi+ pi+ pi-");		

	if (fQADsKKpi) nds1 = new RhoTuple("nds1",			"Ds -> K+ K- pi+");			
	if (fQADsKKpip0) nds2 = new RhoTuple("nds2",			"Ds -> K+ K- pi+ pi0");			
	
	if (fQALamcpKpi) nlamc = new RhoTuple("nlamc",		"Lambda_c -> p K- pi+");		
	
	if (fQAEtacKKpi0) netac1 = new RhoTuple("netac1",		"eta_c -> K+ K- pi0");		
	if (fQAEtacKKspi) netac2 = new RhoTuple("netac2",		"eta_c -> K+ K_S pi-");		
	if (fQAEtacetapipi) netac3 = new RhoTuple("netac3",		"eta_c -> eta pi+ pi-");		
	if (fQAEtacgg) netac4 = new RhoTuple("netac4",		"eta_c -> g g");		

	if (fQAChic02pi2pi0) nchic01 = new RhoTuple("nchic01",	"chi_c0 -> pi+ pi- pi0 pi0");		
	if (fQAChic04pi)   nchic02 = new RhoTuple("nchic02",	"chi_c0 -> pi+ pi- pi+ pi-");		
	if (fQAChic02pi2K) nchic03 = new RhoTuple("nchic03",	"chi_c0 -> pi+ pi- K+ K-");		

	if (fQA2e)   n2e   = new RhoTuple("n2e",	"pbar p -> e+ e-");		
	if (fQA2mu)  n2mu  = new RhoTuple("n2mu",	"pbar p -> mu+ mu-");		
	if (fQA2gam) n2gam = new RhoTuple("n2gam", 	"pbar p -> gamma gamma");		
	
	// *** create mass pre selectors for QA (formular takes into account RhoSelector definition mean +- win/2
	fPi0PreSel   = new RhoMassParticleSelector("pi0PreSel",  (fPi0QaMax + fPi0QaMin)/2.0, 	fPi0QaMax - fPi0QaMin );  
	fEtaPreSel   = new RhoMassParticleSelector("etaPreSel",  (fEtaQaMax + fEtaQaMin)/2.0, 	fEtaQaMax - fEtaQaMin ); 
	fKs0PreSel   = new RhoMassParticleSelector("Ks0PreSel",  (fKs0QaMax + fKs0QaMin)/2.0, 	fKs0QaMax - fKs0QaMin);  
	fPhiPreSel   = new RhoMassParticleSelector("PhiPreSel",  (fPhiQaMax + fPhiQaMin)/2.0, 	fPhiQaMax - fPhiQaMin); 
	fLamPreSel   = new RhoMassParticleSelector("LamPreSel",  (fLamQaMax + fLamQaMin)/2.0, 	fLamQaMax - fLamQaMin);  
	fJpsiPreSel  = new RhoMassParticleSelector("JpsiPreSel", (fJpsiQaMax + fJpsiQaMin)/2.0, fJpsiQaMax - fJpsiQaMin);  
	fD0PreSel    = new RhoMassParticleSelector("D0PreSel",   (fD0QaMax + fD0QaMin)/2.0, 	fD0QaMax - fD0QaMin);  
	fDpmPreSel   = new RhoMassParticleSelector("DpmPreSel",  (fDpmQaMax + fDpmQaMin)/2.0,	fDpmQaMax - fDpmQaMin);  
	fDsPreSel    = new RhoMassParticleSelector("DsPreSel",   (fDsQaMax + fDsQaMin)/2.0, 	fDsQaMax - fDsQaMin);  
	fEtacPreSel  = new RhoMassParticleSelector("EtacPreSel", (fEtacQaMax + fEtacQaMin)/2.0, fEtacQaMax - fEtacQaMin);  
	fChic0PreSel = new RhoMassParticleSelector("Chic0PreSel",(fChic0QaMax + fChic0QaMin)/2.0,fChic0QaMax - fChic0QaMin);  
	fLamcPreSel  = new RhoMassParticleSelector("LamcPreSel", (fLamcQaMax + fLamcQaMin)/2.0, fLamcQaMax - fLamcQaMin); 
	f2ePreSel    = new RhoMassParticleSelector("2ePreSel",   (f2eQaMax + f2eQaMin)/2.0, 	f2eQaMax - f2eQaMin);  
	f2muPreSel   = new RhoMassParticleSelector("2muPreSel",  (f2muQaMax + f2muQaMin)/2.0, 	f2muQaMax - f2muQaMin);  
	f2gamPreSel  = new RhoMassParticleSelector("2gamPreSel", (f2gamQaMax + f2gamQaMin)/2.0,	f2gamQaMax - f2gamQaMin); 
	
	
	// *** number of sigmas deviation for tag
	double tagNumSig   = fNsigTag;

	// *** create final selectors for pi0, eta, KS 
	fPi0Sel     = new RhoMassParticleSelector("pi0Sel",   fPi0Mean, fPi0Sigma*2.0*3.0);
	fEtaSel     = new RhoMassParticleSelector("etaSel",   fEtaMean, fEtaSigma*2.0*3.0);
	fKs0Sel     = new RhoMassParticleSelector("Ks0Sel",   fKs0Mean, fKs0Sigma*2.0*3.0);

	
	// *** create final selectors for different tag modes (factor 2.0 comes from RhoSelector definition mean +- win/2)
	fPhiSel     = new RhoMassParticleSelector("PhiSel",   fPhiMean, fPhiSigma*2.0*tagNumSig);	
	fLamSel     = new RhoMassParticleSelector("LamSel",   fLamMean, fLamSigma*2.0*tagNumSig);
	
	fJpsi1Sel   = new RhoMassParticleSelector("Jpsi1Sel",   fJpsi1Mean, fJpsi1Sigma*2.0*tagNumSig);
	fJpsi2Sel   = new RhoMassParticleSelector("Jpsi2Sel",   fJpsi2Mean, fJpsi2Sigma*2.0*tagNumSig);
	
	fD01Sel     = new RhoMassParticleSelector("D01Sel",   fD01Mean, fD01Sigma*2.0*tagNumSig);
	fD02Sel     = new RhoMassParticleSelector("D02Sel",   fD02Mean, fD02Sigma*2.0*tagNumSig);
	fD03Sel     = new RhoMassParticleSelector("D03Sel",   fD03Mean, fD03Sigma*2.0*tagNumSig);
	
	fDpm1Sel    = new RhoMassParticleSelector("Dpm1Sel",   fDpm1Mean, fDpm1Sigma*2.0*tagNumSig);
	fDpm2Sel    = new RhoMassParticleSelector("Dpm2Sel",   fDpm2Mean, fDpm2Sigma*2.0*tagNumSig);
	fDpm3Sel    = new RhoMassParticleSelector("Dpm3Sel",   fDpm3Mean, fDpm3Sigma*2.0*tagNumSig);
	fDpm4Sel    = new RhoMassParticleSelector("Dpm4Sel",   fDpm4Mean, fDpm4Sigma*2.0*tagNumSig);
	
	fDs1Sel     = new RhoMassParticleSelector("Ds1Sel",   fDs1Mean, fDs1Sigma*2.0*tagNumSig);
	fDs2Sel     = new RhoMassParticleSelector("Ds2Sel",   fDs2Mean, fDs2Sigma*2.0*tagNumSig);
	
	fEtac1Sel   = new RhoMassParticleSelector("Etac1Sel",   fEtac1Mean, fEtac1Sigma*2.0*tagNumSig);
	fEtac2Sel   = new RhoMassParticleSelector("Etac2Sel",   fEtac2Mean, fEtac2Sigma*2.0*tagNumSig);
	fEtac3Sel   = new RhoMassParticleSelector("Etac3Sel",   fEtac3Mean, fEtac3Sigma*2.0*tagNumSig);
	fEtac4Sel   = new RhoMassParticleSelector("Etac4Sel",   fEtac4Mean, fEtac4Sigma*2.0*tagNumSig);

	fChic01Sel  = new RhoMassParticleSelector("Chic01Sel",   fChic01Mean, fChic01Sigma*2.0*tagNumSig);
	fChic02Sel  = new RhoMassParticleSelector("Chic02Sel",   fChic02Mean, fChic02Sigma*2.0*tagNumSig);
	fChic03Sel  = new RhoMassParticleSelector("Chic03Sel",   fChic03Mean, fChic03Sigma*2.0*tagNumSig);
	
	fLamcSel    = new RhoMassParticleSelector("LamcSel",   fLamcMean, fLamcSigma*2.0*tagNumSig);

	f2eSel      = new RhoMassParticleSelector("2eSel",   f2eMean, f2eSigma*2.0*tagNumSig);
	f2muSel     = new RhoMassParticleSelector("2muSel",   f2muMean, f2muSigma*2.0*tagNumSig);
	f2gamSel    = new RhoMassParticleSelector("2gamSel",   f2gamMean, f2gamSigma*2.0*tagNumSig);

	// *** basic selectors for preselection
	fMomentumSel = new RhoMomentumParticleSelector("PSel",50.+fTrackMinP,100.);
	fEnergySel   = new RhoEnergyParticleSelector("ESel",50.+fGammaMinE,100.);
		
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndSoftTriggerTask::SetParContainers() 
{
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndSoftTriggerTask::Exec(Option_t* opt)
{
	// *** prepare TCA for writing OnlineFilterInfo
	if ( fTcaOnlineFilterInfo->GetEntriesFast() != 0 ) fTcaOnlineFilterInfo->Delete();
	
	// *** some variables
	int i=0,j=0, k=0, l=0;
	
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	
	// *** read the next event
	fAnalysis->GetEvent();
	
	// *** fill all lists necessary for combinatorics
	FillGlobalLists();
	
	// *** setup eventshape object
	PndEventShape evtShape(fAllCands,fIniP4,fGammaMinE,fTrackMinP);
	fEventShape = &evtShape;
	
	// *** tag all Soft Trigger Channels
	int tag_phi   = Tag_Phi_KK(nphi);
	int tag_lam   = Tag_Lambda_ppi(nlam);
	
	int tag_jpsi1  = Tag_Jpsi_2e(njpsi1);
	int tag_jpsi2  = Tag_Jpsi_2mu(njpsi2);
	int tag_jpsi   = tag_jpsi1 + tag_jpsi2;
	
	int tag_d01   = Tag_D0_Kpi(nd01);
	int tag_d02   = Tag_D0_Kpipi0(nd02);
	int tag_d03   = Tag_D0_K3pi(nd03);
	int tag_d0    = tag_d01 + tag_d02 +tag_d03;
	
	int tag_dpm1  = Tag_Dpm_Kpipi(ndpm1);
	int tag_dpm2  = Tag_Dpm_K2pipi0(ndpm2);
	int tag_dpm3  = Tag_Dpm_Kspipi0(ndpm3);
	int tag_dpm4  = Tag_Dpm_Ks3pi(ndpm4);
	int tag_dpm   = tag_dpm1 + tag_dpm2 +tag_dpm3 +tag_dpm4;
	
	int tag_ds1   = Tag_Ds_KKpi(nds1);
	int tag_ds2   = Tag_Ds_KKpipi0(nds2);
	int tag_ds    = tag_ds1 + tag_ds2;
	
	int tag_lamc  = Tag_Lambdac_pKpi(nlamc);
	
	int tag_etac1 = Tag_Etac_KKpi0(netac1);
	int tag_etac2 = Tag_Etac_KKspi(netac2);
	int tag_etac3 = Tag_Etac_etapipi(netac3);
	int tag_etac4 = Tag_Etac_gg(netac4);
	int tag_etac  = tag_etac1 + tag_etac2 + tag_etac3 + tag_etac4; 
	
	int tag_chic01 = Tag_Chic0_2pi2pi0(nchic01);
	int tag_chic02 = Tag_Chic0_4pi(nchic02);
	int tag_chic03 = Tag_Chic0_2pi2K(nchic03);
	int tag_chic0  = tag_chic01 + tag_chic02 + tag_chic03;
	
	int tag_2e     = Tag_2e(n2e);
	int tag_2mu    = Tag_2mu(n2mu);
	int tag_2gam   = Tag_2gam(n2gam);
	int tag_em     = tag_2e + tag_2mu + tag_2gam;
	
	int tag_glob   = tag_phi + tag_lam + tag_jpsi + tag_d0 + tag_dpm
				   + tag_ds + tag_lamc + tag_etac + tag_chic0
				   + tag_em;
	
	Float_t tagged = (tag_glob>0);
	
	// *** write common information	
	if (fQAEvent)
	{
		ntp->Column("ev",		(Float_t) 	fEvtCount,	0.0f);
		ntp->Column("run",  	(Float_t) 	fRunNum,	0.0f);
		ntp->Column("mode",		(Float_t)	fMode,		0.0f);
		ntp->Column("ecm",		(Float_t)	fEcm,		0.0f);

		fQA->qaP4("beam", fIniP4, ntp);
		
		ntp->Column("tagphi",	(Float_t)	tag_phi,	0.0f);
		ntp->Column("taglam",	(Float_t)	tag_lam,	0.0f);
		ntp->Column("tagjpsi1",	(Float_t)	tag_jpsi1,	0.0f);
		ntp->Column("tagjpsi2",	(Float_t)	tag_jpsi2,	0.0f);
		ntp->Column("tagjpsi",	(Float_t)	tag_jpsi,	0.0f);
		
		ntp->Column("tagd01",	(Float_t)	tag_d01,	0.0f);
		ntp->Column("tagd02",	(Float_t)	tag_d02,	0.0f);
		ntp->Column("tagd03",	(Float_t)	tag_d03,	0.0f);
		ntp->Column("tagd0",	(Float_t)	tag_d0,		0.0f);
		
		ntp->Column("tagdpm1",	(Float_t)	tag_dpm1,	0.0f);
		ntp->Column("tagdpm2",	(Float_t)	tag_dpm2,	0.0f);
		ntp->Column("tagdpm3",	(Float_t)	tag_dpm3,	0.0f);
		ntp->Column("tagdpm4",	(Float_t)	tag_dpm4,	0.0f);
		ntp->Column("tagdpm",	(Float_t)	tag_dpm,	0.0f);
		
		ntp->Column("tagds1",	(Float_t)	tag_ds1,	0.0f);
		ntp->Column("tagds2",	(Float_t)	tag_ds2,	0.0f);
		ntp->Column("tagds",	(Float_t)	tag_ds,		0.0f);
		
		ntp->Column("taglamc",	(Float_t)	tag_lamc,	0.0f);
		
		ntp->Column("tagetac1",	(Float_t)	tag_etac1,	0.0f);
		ntp->Column("tagetac2",	(Float_t)	tag_etac2,	0.0f);
		ntp->Column("tagetac3",	(Float_t)	tag_etac3,	0.0f);
		ntp->Column("tagetac4",	(Float_t)	tag_etac4,	0.0f);
		ntp->Column("tagetac",	(Float_t)	tag_etac,	0.0f);

		ntp->Column("tagchic01",(Float_t)	tag_chic01,	0.0f);
		ntp->Column("tagchic02",(Float_t)	tag_chic02,	0.0f);
		ntp->Column("tagchic03",(Float_t)	tag_chic03,	0.0f);
		ntp->Column("tagchic0",	(Float_t)	tag_chic0,	0.0f);
		
		ntp->Column("tag2e",	(Float_t)	tag_2e,		0.0f);
		ntp->Column("tag2mu",	(Float_t)	tag_2mu,	0.0f);
		ntp->Column("tag2gam",	(Float_t)	tag_2gam,	0.0f);
		
		ntp->Column("tagem",	(Float_t)	tag_em,		0.0f);

		ntp->Column("tagall",	(Float_t)	tag_glob,	0.0f);
		ntp->Column("tag",		(Float_t)	tagged,		0.0f);
		
		fQA->qaEventShape("es", fEventShape, ntp);
		
		ntp->DumpData();
	}
	
	// Create PndOnlineFilterInfo entry in TCA
	
	PndOnlineFilterInfo* info=new ( (*fTcaOnlineFilterInfo)[0] ) PndOnlineFilterInfo();

	info->SetTagPhiKK(tag_phi);
	info->SetTagLamppi(tag_lam);
	info->SetTagJpsi2e(tag_jpsi1);
	info->SetTagJpsi2mu(tag_jpsi2);
	info->SetTagD0Kpi(tag_d01);
	info->SetTagD0Kpipi0(tag_d02);
	info->SetTagD0K3pi(tag_d03);
	info->SetTagDpmKpipi(tag_dpm1);
	info->SetTagDpmK2pipi0(tag_dpm2);
	info->SetTagDpmKspipi0(tag_dpm3);
	info->SetTagDpmKs3pi(tag_dpm4);
	info->SetTagDsKKpi(tag_ds1);
	info->SetTagDsKKpip0(tag_ds2);
	info->SetTagLamcpKpi(tag_lamc);
	info->SetTagEtacKKpi0(tag_etac1);
	info->SetTagEtacKKspi(tag_etac2);
	info->SetTagEtacetapipi(tag_etac3);
	info->SetTagEtacgg(tag_etac4);
	info->SetTagChic02pi2pi0(tag_chic01);
	info->SetTagChic04pi(tag_chic02);
	info->SetTagChic02pi2K(tag_chic03);
	info->SetTag2e(tag_2e);
	info->SetTag2mu(tag_2mu);
	info->SetTag2gam(tag_2gam);

}


void PndSoftTriggerTask::Finish()
{	
	if (ntp) ntp->GetInternalTree()->Write();			  // overall info
	if (nks0) nks0->GetInternalTree()->Write();		      // Ks0 QA
	if (npi0) npi0->GetInternalTree()->Write();			  // pi0 QA
	if (neta) neta->GetInternalTree()->Write();			  // eta QA
	
	if (nphi) nphi->GetInternalTree()->Write();		      // phi -> K+ K-					(49.8 %)
	if (nlam) nlam->GetInternalTree()->Write();			  // Lambda -> p pi-				(63.9 %)

	if (njpsi1) njpsi1->GetInternalTree()->Write();		  // J/psi -> e+e-		(11.9 %)
	if (njpsi2) njpsi2->GetInternalTree()->Write();		  // J/psi -> mu+ mu-		(11.9 %)
	
	if (nd01) nd01->GetInternalTree()->Write();		      // D0 -> K- pi+					( 3.9 %)
	if (nd02) nd02->GetInternalTree()->Write();		      // D0 -> K- pi+ pi0				(13.9 %)
	if (nd03) nd03->GetInternalTree()->Write();		      // D0 -> K- pi+ pi+ pi-			( 8.1 %)
	if (ndpm1) ndpm1->GetInternalTree()->Write();		  // D+- -> K- pi+ pi+			( 9.4 %)
	if (ndpm2) ndpm2->GetInternalTree()->Write();		  // D+- -> K- pi+ pi+ pi0		( 6.1 %)
	if (ndpm3) ndpm3->GetInternalTree()->Write();		  // D+- -> K_S pi+ pi0			( 6.9 %) 
	if (ndpm4) ndpm4->GetInternalTree()->Write();		  // D+- -> K_S pi+ pi+ pi-		( 3.1 %) 
	if (nds1) nds1->GetInternalTree()->Write();		      // Ds -> K+ K- pi+				( 5.5 %)
	if (nds2) nds2->GetInternalTree()->Write();		      // Ds -> K+ K- pi+ pi0			( 5.6 %)
	
	if (nlamc) nlamc->GetInternalTree()->Write();		  // Lambda_c -> p K- pi+			( 5.0 %)
	
	if (netac1) netac1->GetInternalTree()->Write();		  // eta_c -> K+ K- pi0 			( below) 
	if (netac2) netac2->GetInternalTree()->Write();		  // eta_c -> K+- K_S pi-+        ( 3.2 %) = 7%/3 (1) + 7%/3*35% (2)
	if (netac3) netac3->GetInternalTree()->Write();		  // eta_c -> eta (gg) pi+ pi-	( 1.9 %) = 4.9% * 39 %
	if (netac4) netac4->GetInternalTree()->Write();		  // eta_c -> gg					( 0.0 %) = 6e-5

	if (nchic01) nchic01->GetInternalTree()->Write();	  // chi_c0 -> pi+ pi- pi0 pi0	( 3.4 %)
	if (nchic02) nchic02->GetInternalTree()->Write();	  // chi_c0 -> pi+ pi- pi+ pi-	( 2.3 %)
	if (nchic03) nchic03->GetInternalTree()->Write();	  // chi_c0 -> pi+ pi- K+ K-	( 1.8 %)
	
	if (n2e) n2e->GetInternalTree()->Write();		      // pbar p -> e+ e-	
	if (n2mu) n2mu->GetInternalTree()->Write();		      // pbar p -> mu+ mu-	
	if (n2gam) n2gam->GetInternalTree()->Write();		  // pbar p -> gam gam	
}

// -------------------------------------------------------------------------

int PndSoftTriggerTask::CreateKs0Cands(RhoTuple *n)
{
	// *** basic KS0 Reco
	fKs0Cands.Combine(fPionPlus, fPionMinus);
	// *** pre select for qa
	fKs0Cands.Select(fKs0PreSel);
	fKs0Cands.SetType(310);
	
	if (n)
	{
		for (int i=0; i<fKs0Cands.GetLength();++i)
		{			
			Float_t nsig = (Float_t) fabs(fKs0Cands[i]->Mass()-fKs0Mean)/fKs0Sigma;
			Float_t tag  = nsig<fNsigAux;  // instead of fNsigTag, fNsigAux cut is applied for KS
			
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("ksmean", (Float_t) fKs0Mean,		0.0f);
			n->Column("kssig", 	(Float_t) fKs0Sigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, n);
			fQA->qaKs0("ks", fKs0Cands[i], n);
			
			n->DumpData();
		}
	}
	
	// *** final selection
	fKs0Cands.Select(fKs0Sel);
	
	return fKs0Cands.GetLength();
}

// -------------------------------------------------------------------------


void PndSoftTriggerTask::FillGlobalLists()
{
	int i;
	
	// *** fill standard lists
	fAnalysis->FillList( fMcTruth, 			"McTruth" );
	fAnalysis->FillList( fAllCands,			"All" );
	fAnalysis->FillList( fChargedCands,		"Charged" );
	fAnalysis->FillList( fNeutralCands,		"Neutral" );
		
	// *** fill PID lists
	fAnalysis->FillList( fElectronPlus, 	"ElectronAllPlus", 	fAlgoElectron );
	fAnalysis->FillList( fElectronMinus,	"ElectronAllMinus", fAlgoElectron );
	
	fAnalysis->FillList( fMuonPlus,			"MuonAllPlus", 		fAlgoMuon );
	fAnalysis->FillList( fMuonMinus,		"MuonAllMinus", 	fAlgoMuon );
	
	fAnalysis->FillList( fPionPlus,			"PionAllPlus", 		fAlgoPion );
	fAnalysis->FillList( fPionMinus,		"PionAllMinus", 	fAlgoPion );
	
	fAnalysis->FillList( fKaonPlus,			"KaonAllPlus", 		fAlgoKaon );
	fAnalysis->FillList( fKaonMinus,		"KaonAllMinus", 	fAlgoKaon );
	
	fAnalysis->FillList( fProtonPlus,		"ProtonAllPlus", 	fAlgoProton );
	fAnalysis->FillList( fProtonMinus,		"ProtonAllMinus", 	fAlgoProton );
	
	if (fIniPidCut>0)
	{
		SelectPidProb(fElectronPlus, 0, fIniPidCut);	
		SelectPidProb(fElectronMinus, 0, fIniPidCut);
		
		SelectPidProb(fMuonPlus, 1, fIniPidCut);
		SelectPidProb(fMuonMinus, 1, fIniPidCut);
		
		SelectPidProb(fPionPlus, 2, fIniPidCut);
		SelectPidProb(fPionMinus, 2, fIniPidCut);
		
		SelectPidProb(fKaonPlus, 3, fIniPidCut);
		SelectPidProb(fKaonMinus, 3, fIniPidCut);
		
		SelectPidProb(fProtonPlus, 4, fIniPidCut);
		SelectPidProb(fProtonMinus, 4, fIniPidCut);	
	}
	
	// *** select on lists
	fGammaCands.Select(fNeutralCands, fEnergySel);
	
	if (fTrackMinP>0)
	{
		fElectronPlus.Select(fMomentumSel); fElectronMinus.Select(fMomentumSel);
		fMuonPlus.Select(fMomentumSel);     fMuonMinus.Select(fMomentumSel);
		fPionPlus.Select(fMomentumSel);     fPionMinus.Select(fMomentumSel);
		fKaonPlus.Select(fMomentumSel);     fKaonMinus.Select(fMomentumSel);
		fProtonPlus.Select(fMomentumSel);   fProtonMinus.Select(fMomentumSel);
	}
	
	// *** Ks reco
	CreateKs0Cands(nks0);
	
	// *** pi0 reco
	fPi0Cands.Combine(fGammaCands, fGammaCands);
	fPi0Cands.Select(fPi0PreSel);
	fPi0Cands.SetType(111);
	if (fQAPi0)
	{
		for (i=0; i<fPi0Cands.GetLength();++i)
		{
			Float_t nsig = (Float_t) fabs(fPi0Cands[i]->Mass()-fPi0Mean)/fPi0Sigma;
			Float_t tag  = nsig<fNsigAux;  // instead of fNsigTag, a fNsigAux cut is applied for pi0
			
			npi0->Column("tag", 	(Float_t) tag,			0.0f);
			npi0->Column("nsig", 	(Float_t) nsig,			0.0f);
			npi0->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			npi0->Column("run",  	(Float_t) fRunNum,		0.0f);
			npi0->Column("num", 	(Float_t) i,			0.0f);
			npi0->Column("mode",	(Float_t) fMode,		0.0f);
			npi0->Column("pi0mean", (Float_t) fPi0Mean,		0.0f);
			npi0->Column("pi0sig", 	(Float_t) fPi0Sigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, npi0);
			fQA->qaPi0("pi0", fPi0Cands[i], npi0);
			npi0->DumpData();
		}
	}
	fPi0Cands.Select(fPi0Sel);
	
	// *** era reco
	fEtaCands.Combine(fGammaCands, fGammaCands);
	fEtaCands.Select(fEtaPreSel);
	fEtaCands.SetType(221);
	if (fQAEta)
	{
		for (i=0; i<fEtaCands.GetLength();++i)
		{
			Float_t nsig = (Float_t) fabs(fEtaCands[i]->Mass()-fEtaMean)/fEtaSigma;
			Float_t tag  = nsig<fNsigAux;  // instead of fNsigTag, a fNsigAux cut is applied for eta
			
			neta->Column("tag", 	(Float_t) tag,			0.0f);
			neta->Column("nsig", 	(Float_t) nsig,			0.0f);
			neta->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			neta->Column("run",  	(Float_t) fRunNum,		0.0f);
			neta->Column("num", 	(Float_t) i,			0.0f);
			neta->Column("mode",	(Float_t) fMode,		0.0f);
			neta->Column("etamean", (Float_t) fEtaMean,		0.0f);
			neta->Column("etasig", 	(Float_t) fEtaSigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, neta);
			fQA->qaPi0("eta", fEtaCands[i], neta);
			
			neta->DumpData();
		}
	}
	fEtaCands.Select(fEtaSel);
	
}

// -------------------------------------------------------------------------
// *** phi -> K+ K- decays

int PndSoftTriggerTask::Tag_Phi_KK(RhoTuple *n)
{		
	if (!fTagPhiKK) return 0;
	if ( fEcm < DbMass("phi") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fKaonPlus, fKaonMinus);
	l.SetType("phi");
	
	// *** pre selection for QA
	l.Select(fPhiPreSel);
	
	// *** store QA
	if (n)
	{
		for (int i=0;i<l.GetLength();++i)
		{
			fQA->qaComp("phi", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);

			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fPhiMean)/fPhiSigma;
			Float_t tag  = nsig<fNsigTag;
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("phimean",(Float_t) fPhiMean,		0.0f);
			n->Column("phisig", (Float_t) fPhiSigma,	0.0f);
			
			n->DumpData();
		}
	}
	// *** final tag selection
	l.Select(fPhiSel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** Lambda -> p pi- decays

int PndSoftTriggerTask::Tag_Lambda_ppi(RhoTuple *n)
{
	if (!fTagLamppi) return 0;
	if ( fEcm < (DbMass("Lambda0") + DbMass("proton")) ) return 0;
	
	RhoCandList l, l2;
	
	// *** combinatorics
	l.Combine(fProtonPlus, fPionMinus);
	l.SetType(3122);
	l2.Combine(fProtonMinus, fPionPlus);
	l2.SetType(-3122);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fLamPreSel);
	
	if (n)
	{
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("lam", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);

			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fLamMean)/fLamSigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("lammean",(Float_t) fLamMean,		0.0f);
			n->Column("lamsig", (Float_t) fLamSigma,	0.0f);
			
			n->DumpData();
		}
	}
	
	// *** final tag selection
	l.Select(fLamSel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** J/psi -> e+ e- decays

int PndSoftTriggerTask::Tag_Jpsi_2e(RhoTuple *n)
{
	if (!fTagJpsi2e) return 0;
	if ( fEcm < DbMass("J/psi") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fElectronPlus, fElectronMinus);
	l.SetType(443);
	
	// *** pre selection for QA
	l.Select(fJpsiPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("jpsi", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fJpsi1Mean)/fJpsi1Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("jpsimean",(Float_t) fJpsi1Mean,   0.0f);
			n->Column("jpsisig", (Float_t) fJpsi1Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fJpsi1Sel);
	
	return l.GetLength();
}
// -------------------------------------------------------------------------
// *** J/psi -> mu+ mu- decays

int PndSoftTriggerTask::Tag_Jpsi_2mu(RhoTuple *n)
{
	if (!fTagJpsi2mu) return 0;
	if ( fEcm < DbMass("J/psi") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fMuonPlus, fMuonMinus);
	l.SetType(443);
	
	// *** pre selection for QA
	l.Select(fJpsiPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("jpsi", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fJpsi2Mean)/fJpsi2Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("jpsimean",(Float_t) fJpsi2Mean,  0.0f);
			n->Column("jpsisig", (Float_t) fJpsi2Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fJpsi2Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** D0 -> K- pi+ decays
	
int PndSoftTriggerTask::Tag_D0_Kpi(RhoTuple *n)
{
	if (!fTagD0Kpi) return 0;
	if ( fEcm < (DbMass("D0") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKaonMinus, fPionPlus);
	l.SetType(421);
	l2.Combine(fKaonPlus, fPionMinus);
	l2.SetType(-421);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fD0PreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("d0", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fD01Mean)/fD01Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("d0mean",(Float_t) fD01Mean,0.0f);
			n->Column("d0sig", (Float_t) fD01Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fD01Sel);
		
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** D0 -> K- pi+ pi0 decays

int PndSoftTriggerTask::Tag_D0_Kpipi0(RhoTuple *n)
{
	if (!fTagD0Kpipi0) return 0;
	if ( fEcm < (DbMass("D0") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKaonMinus, fPionPlus, fPi0Cands);
	l.SetType(421);
	l2.Combine(fKaonPlus, fPionMinus, fPi0Cands);
	l2.SetType(-421);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fD0PreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("d0", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fD02Mean)/fD02Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("d0mean",(Float_t) fD02Mean,0.0f);
			n->Column("d0sig", (Float_t) fD02Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fD02Sel);
		
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** D0 -> K- pi+ pi+ pi- decays

int PndSoftTriggerTask::Tag_D0_K3pi(RhoTuple *n)
{
	if (!fTagD0K3pi) return 0;
	if ( fEcm < (DbMass("D0") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKaonMinus, fPionPlus, fPionPlus, fPionMinus);
	l.SetType(421);
	l2.Combine(fKaonPlus, fPionMinus, fPionMinus, fPionPlus);
	l2.SetType(-421);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fD0PreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("d0", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fD03Mean)/fD03Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("d0mean",(Float_t) fD03Mean,0.0f);
			n->Column("d0sig", (Float_t) fD03Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fD03Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** D+ -> K- pi+ pi+ decays
	
int PndSoftTriggerTask::Tag_Dpm_Kpipi(RhoTuple *n)
{
	if (!fTagDpmKpipi) return 0;
	if ( fEcm < (DbMass("D+") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKaonMinus, fPionPlus, fPionPlus);
	l.SetType(411);
	l2.Combine(fKaonPlus, fPionMinus, fPionMinus);
	l2.SetType(-411);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fDpmPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("dpm", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fDpm1Mean)/fDpm1Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("dpmmean",(Float_t) fDpm1Mean,0.0f);
			n->Column("dpmsig", (Float_t) fDpm1Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fDpm1Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** D+ -> K- pi+ pi+ pi0 decays

int PndSoftTriggerTask::Tag_Dpm_K2pipi0(RhoTuple *n)
{
	if (!fTagDpmK2pipi0) return 0;
	if ( fEcm < (DbMass("D+") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKaonMinus, fPionPlus, fPionPlus, fPi0Cands);
	l.SetType(411);
	l2.Combine(fKaonPlus, fPionMinus, fPionMinus, fPi0Cands);
	l2.SetType(-411);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fDpmPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("dpm", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fDpm2Mean)/fDpm2Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("dpmmean",(Float_t) fDpm2Mean,0.0f);
			n->Column("dpmsig", (Float_t) fDpm2Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fDpm2Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** D+ -> K_S pi+ pi0 decays

int PndSoftTriggerTask::Tag_Dpm_Kspipi0(RhoTuple *n)
{
	if (!fTagDpmKspipi0) return 0;
	if ( fEcm < (DbMass("D+") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKs0Cands, fPionPlus, fPi0Cands);
	l.SetType(411);
	l2.Combine(fKs0Cands, fPionMinus, fPi0Cands);
	l2.SetType(-411);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fDpmPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("dpm", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fDpm3Mean)/fDpm3Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("dpmmean",(Float_t) fDpm3Mean,0.0f);
			n->Column("dpmsig", (Float_t) fDpm3Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fDpm3Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** D+ -> K_S pi+ pi+ pi- decays

int PndSoftTriggerTask::Tag_Dpm_Ks3pi(RhoTuple *n)
{
	if (!fTagDpmKs3pi) return 0;
	if ( fEcm < (DbMass("D+") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKs0Cands, fPionPlus, fPionPlus, fPionMinus);
	l.SetType(411);
	l2.Combine(fKs0Cands, fPionMinus, fPionMinus, fPionPlus);
	l2.SetType(-411);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fDpmPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("dpm", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fDpm4Mean)/fDpm4Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("dpmmean",(Float_t) fDpm4Mean,0.0f);
			n->Column("dpmsig", (Float_t) fDpm4Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fDpm4Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** Ds -> K+ K- pi+ decays
	
int PndSoftTriggerTask::Tag_Ds_KKpi(RhoTuple *n)
{
	if (!fTagDsKKpi) return 0;
	if ( fEcm < (DbMass("D_s+") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKaonPlus, fKaonMinus, fPionPlus);
	l.SetType(431);
	l2.Combine(fKaonMinus, fKaonPlus, fPionMinus);
	l2.SetType(-431);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fDsPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("ds", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fDs1Mean)/fDs1Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("dsmean",(Float_t) fDs1Mean,0.0f);
			n->Column("dssig", (Float_t) fDs1Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fDs1Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** Ds -> K+ K- pi+ pi0 decays

int PndSoftTriggerTask::Tag_Ds_KKpipi0(RhoTuple *n)
{
	if (!fTagDsKKpip0) return 0;
	if ( fEcm < (DbMass("D_s+") + DbMass("c")) ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fKaonPlus, fKaonMinus, fPionPlus, fPi0Cands);
	l.SetType(431);
	l2.Combine(fKaonMinus, fKaonPlus, fPionMinus, fPi0Cands);
	l2.SetType(-431);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fDsPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("ds", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fDs2Mean)/fDs2Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("dsmean",(Float_t) fDs2Mean,0.0f);
			n->Column("dssig", (Float_t) fDs2Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fDs2Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** Lambda_c -> p K- pi+ decays
	
int PndSoftTriggerTask::Tag_Lambdac_pKpi(RhoTuple *n)
{
	if (!fTagLamcpKpi) return 0;
	if ( fEcm < DbMass("Lambda_c+") ) return 0;
	
	RhoCandList l,l2;
	
	// *** combinatorics
	l.Combine(fProtonPlus, fKaonMinus, fPionPlus);
	l.SetType(4122);
	l2.Combine(fProtonMinus, fKaonPlus, fPionMinus);
	l2.SetType(-4122);
	l.Append(l2);
	
	// *** pre selection for QA
	l.Select(fLamcPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("lamc", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fLamcMean)/fLamcSigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("lamcmean",(Float_t) fLamcMean,0.0f);
			n->Column("lamcsig", (Float_t) fLamcSigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fLamcSel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** eta_c -> K+ K- pi0 decays
	
int PndSoftTriggerTask::Tag_Etac_KKpi0(RhoTuple *n)
{
	if (!fTagEtacKKpi0) return 0;
	if ( fEcm < DbMass("eta_c") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fKaonPlus, fKaonMinus, fPi0Cands);
	l.SetType(441);
	
	// *** pre selection for QA
	l.Select(fEtacPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("etac", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fEtac1Mean)/fEtac1Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("etacmean",(Float_t) fEtac1Mean,0.0f);
			n->Column("etacsig", (Float_t) fEtac1Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fEtac1Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** eta_c -> K+ KS pi- decays

int PndSoftTriggerTask::Tag_Etac_KKspi(RhoTuple *n)
{
	if (!fTagEtacKKspi) return 0;
	if ( fEcm < DbMass("eta_c") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fKs0Cands, fKaonMinus, fPionPlus);
	l.CombineAndAppend(fKs0Cands, fKaonPlus, fPionMinus);
	l.SetType(441);
	
	// *** pre selection for QA
	l.Select(fEtacPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("etac", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fEtac2Mean)/fEtac2Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("etacmean",(Float_t) fEtac2Mean,	0.0f);
			n->Column("etacsig", (Float_t) fEtac2Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fEtac2Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** eta_c -> eta pi+ pi- decays

int PndSoftTriggerTask::Tag_Etac_etapipi(RhoTuple *n)
{
	if (!fTagEtacetapipi) return 0;
	if ( fEcm < DbMass("eta_c") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fEtaCands, fPionPlus, fPionMinus);
	l.SetType(441);
	
	// *** pre selection for QA
	l.Select(fEtacPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("etac", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fEtac3Mean)/fEtac3Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("etacmean",(Float_t) fEtac3Mean,	0.0f);
			n->Column("etacsig", (Float_t) fEtac3Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fEtac3Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** eta_c -> gg decays

int PndSoftTriggerTask::Tag_Etac_gg(RhoTuple *n)
{
	if (!fTagEtacgg) return 0;
	if ( fEcm < DbMass("eta_c") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fGammaCands, fGammaCands);
	l.SetType(441);
	
	// *** pre selection for QA
	l.Select(fEtacPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("etac", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fEtac4Mean)/fEtac4Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("etacmean",(Float_t) fEtac4Mean,	0.0f);
			n->Column("etacsig", (Float_t) fEtac4Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fEtac4Sel);
	
	return l.GetLength();
}
	
// -------------------------------------------------------------------------
// *** chi_c0 -> pi+ pi- pi0 pi0 decays

int PndSoftTriggerTask::Tag_Chic0_2pi2pi0(RhoTuple *n)
{
	if (!fTagChic02pi2pi0) return 0;
	if ( fEcm < DbMass("chi_0c") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fPionPlus, fPionMinus, fPi0Cands, fPi0Cands);
	l.SetType(10441);
	
	// *** pre selection for QA
	l.Select(fChic0PreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("chic0", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fChic01Mean)/fChic01Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,				0.0f);
			n->Column("tag", 	(Float_t) tag,				0.0f);
			n->Column("num", 	  (Float_t) i,				0.0f);
			n->Column("chic0mean",(Float_t) fChic01Mean,	0.0f);
			n->Column("chic0sig", (Float_t) fChic01Sigma,	0.0f);
			
			n->DumpData();
		}
		
	// *** final tag selection
	l.Select(fChic01Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** chi_c0 -> pi+ pi- pi+ pi- decays

int PndSoftTriggerTask::Tag_Chic0_4pi(RhoTuple *n)
{
	if (!fTagChic04pi) return 0;
	if ( fEcm < DbMass("chi_0c") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fPionPlus, fPionMinus, fPionPlus, fPionMinus);
	l.SetType(10441);
	
	// *** pre selection for QA
	l.Select(fChic0PreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("chic0", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fChic02Mean)/fChic02Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,				0.0f);
			n->Column("tag", 	(Float_t) tag,				0.0f);
			n->Column("num", 	  (Float_t) i,				0.0f);
			n->Column("chic0mean",(Float_t) fChic02Mean,	0.0f);
			n->Column("chic0sig", (Float_t) fChic02Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fChic02Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** chi_c0 -> pi+ pi- K+ K- decays

int PndSoftTriggerTask::Tag_Chic0_2pi2K(RhoTuple *n)
{
	if (!fTagChic02pi2K) return 0;
	if ( fEcm < DbMass("chi_0c") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fPionPlus, fPionMinus, fKaonPlus, fKaonMinus);
	l.SetType(10441);
	
	// *** pre selection for QA
	l.Select(fChic0PreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("chic0", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-fChic03Mean)/fChic03Sigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	  (Float_t) nsig,			0.0f);
			n->Column("tag", 	  (Float_t) tag,			0.0f);
			n->Column("num", 	  (Float_t) i,				0.0f);
			n->Column("chic0mean",(Float_t) fChic03Mean,	0.0f);
			n->Column("chic0sig", (Float_t) fChic03Sigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(fChic03Sel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** pbar p -> e+ e- decays

int PndSoftTriggerTask::Tag_2e(RhoTuple *n)
{
	if (!fTag2e) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fElectronPlus, fElectronMinus);
	l.SetType(88880);
	
	// *** pre selection for QA
	l.Select(f2ePreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("ee", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-f2eMean)/f2eSigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,		0.0f);
			n->Column("tag", 	(Float_t) tag,		0.0f);
			n->Column("num", 	(Float_t) i,		0.0f);
			n->Column("eemean",	(Float_t) f2eMean,	0.0f);
			n->Column("eesig", 	(Float_t) f2eSigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(f2eSel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** pbar p -> mu+ mu- decays

int PndSoftTriggerTask::Tag_2mu(RhoTuple *n)
{
	if (!fTag2mu) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fMuonPlus, fMuonMinus);
	l.SetType(88880);
	
	// *** pre selection for QA
	l.Select(f2muPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("mumu", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-f2muMean)/f2muSigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 		(Float_t) nsig,			0.0f);
			n->Column("tag", 		(Float_t) tag,			0.0f);
			n->Column("num", 		(Float_t) i,			0.0f);
			n->Column("mumumean",	(Float_t) f2muMean,		0.0f);
			n->Column("mumusig", 	(Float_t) f2muSigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(f2muSel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** pbar p -> gam gam decays

int PndSoftTriggerTask::Tag_2gam(RhoTuple *n)
{
	if (!fTag2e) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fGammaCands, fGammaCands);
	l.SetType(88880);
	
	// *** pre selection for QA
	l.Select(f2gamPreSel);
	
	if (n)
		for (int i=0;i<l.GetLength();++i)
		{
			// *** store QA info
			fQA->qaComp("gg", l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			fQA->qaP4("beam", fIniP4, n);
			
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-f2gamMean)<f2gamSigma;
			Float_t tag  = (nsig<fNsigTag);
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("ggmean",	(Float_t) f2gamMean,	0.0f);
			n->Column("ggsig", 	(Float_t) f2gamSigma,	0.0f);
			
			n->DumpData();
		}
	
	// *** final tag selection
	l.Select(f2eSel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------


int PndSoftTriggerTask::signalType(RhoCandList &l, int v0pdg, int d1pdg, int d2pdg)
{
	int sig=0;
	
	for (int i=0;i<l.GetLength();++i)
	{
		RhoCandidate *d0 = l[i]->Daughter(0);
		RhoCandidate *d1 = l[i]->Daughter(1);
		if (d0==0x0 || d1==0x0) continue;
		
		int v0c = l[i]->PdgCode();
		int d1c = d0->PdgCode();
		int d2c = d1->PdgCode();
		
		if (v0c==v0pdg && ((d1c==d1pdg && d2c==d2pdg) || (d2c==d1pdg && d1c==d2pdg)))
		{
			sig = 1;
			break;
		}
	}
	return sig;
}

// -------------------------------------------------------------------------

ClassImp(PndSoftTriggerTask)
