// ************************************************************************
//
// Online SoftTrigger reco and tagging for several channels 
//
// K.Goetzen 10/2013
// ************************************************************************


// The header file
#include "PndSoftTriggerTask.h"

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
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndSoftTriggerTask::PndSoftTriggerTask(double pmom, int mode) :
	FairTask("Panda Softtrigger Task"),
	fMode(mode), fEvtCount(0), fSigCount(0),	
	fIniP4(0,0,0,0),
	fTagPhiKK(true), fTagLamppi(true), fTagJpsill(true),
	fTagD0Kpi(true), fTagD0Kpipi0(true), fTagD0K3pi(true),
	fTagDpmKpipi(true), fTagDpmK2pipi0(true), fTagDpmKspipi0(true),
	fTagDpmKs3pi(true),	fTagDsKKpi(true), fTagDsKKpip0(true),
	fTagLamcpKpi(true), fTagEtacKKpi0(true), fTagEtacKKspi(true),
	fTagEtacetapipi(true), fTagEtacgg(true), fTagChic02pi2pi0(true), fTagChic04pi(true),
	fPi0SelMean(0),   fPi0SelWin(0.05),
	fEtaSelMean(0),   fEtaSelWin(0.05),
	fKs0SelMean(0),   fKs0SelWin(0.05),
	fPhiSelMean(0),	  fPhiSelWin(0.02),		
	fD0SelMean(0),    fD0SelWin(0.07),		
	fDpmSelMean(0),   fDpmSelWin(0.07),		
	fDsSelMean(0),    fDsSelWin(0.07),		
	fLamcSelMean(0),  fLamcSelWin(0.05),		
	fLamSelMean(0),	  fLamSelWin(0.05),		
	fEtacSelMean(0),  fEtacSelWin(0.15),		
	fJpsiSelMean(0),  fJpsiSelWin(0.15),		
	fChic0SelMean(0), fChic0SelWin(0.15),		
	fGammaMinE(0.03), fTrackMinP(0.15),
	fEventShape(NULL), 
	fPi0Sel(NULL), fKs0Sel(NULL),
	fMomentumSel(NULL), fEnergySel(NULL)
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
	
	// *** set the mean masses for mass cut to nominal values
	
	fPi0SelMean = DbMass("pi0");
	fEtaSelMean = DbMass("eta");
	fKs0SelMean = DbMass("K_S0");
	
	fPhiSelMean = DbMass("phi");
	
	fD0SelMean  = DbMass("D0");
	fDpmSelMean = DbMass("D+");
	fDsSelMean  = DbMass("D_s+");
	
	fJpsiSelMean  = DbMass("J/psi");
	fEtacSelMean  = DbMass("eta_c");
	fChic0SelMean = DbMass("chi_0c");
	
	fLamSelMean  = DbMass("Lambda0");
	fLamcSelMean = DbMass("Lambda_c+");
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSoftTriggerTask::~PndSoftTriggerTask() { }
// -------------------------------------------------------------------------


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
	// *** initialize analysis object
	fAnalysis = new PndAnalysis();
	
	// *** vertexer
	fVtxPoca = new PndVtxPoca();
	
	// *** create ntuple
	ntp  = new RhoTuple("ntp","Soft Trigger Common");
	nks0 = new RhoTuple("nks0","K_S -> pi+ pi-");
	npi0 = new RhoTuple("npi0","pi0 -> g g");
	neta = new RhoTuple("neta","eta -> g g");
	
	nphi = new RhoTuple("nphi",			"phi -> K+ K-");	
	nlam = new RhoTuple("nlam",			"Lambda -> p pi");	
	njpsi = new RhoTuple("njpsi",		"J/psi -> l+ l-");	
	
	nd01 = new RhoTuple("nd01",			"D0 -> K- pi+");		
	nd02 = new RhoTuple("nd02",			"D0 -> K- pi+ pi0");		
	nd03 = new RhoTuple("nd03",			"D0 -> K- pi+ pi- pi-");		
	
	ndpm1 = new RhoTuple("ndpm1",		"D+- -> K- pi+ pi+");		
	ndpm2 = new RhoTuple("ndpm2",		"D+- -> K- pi+ pi+ pi0");	
	ndpm3 = new RhoTuple("ndpm3",		"D+- -> K_S pi+ pi0");		
	ndpm4 = new RhoTuple("ndpm4",		"D+- -> K_S pi+ pi+ pi-");		

	nds1 = new RhoTuple("nds1",			"Ds -> K+ K- pi+");			
	nds2 = new RhoTuple("nds2",			"Ds -> K+ K- pi+ pi0");			
	
	nlamc = new RhoTuple("nlamc",		"Lambda_c -> p K- pi+");		
	
	netac1 = new RhoTuple("netac1",		"eta_c -> K+ K- pi0");		
	netac2 = new RhoTuple("netac2",		"eta_c -> K+ K_S pi-");		
	netac3 = new RhoTuple("netac3",		"eta_c -> eta pi+ pi-");		
	netac4 = new RhoTuple("netac4",		"eta_c -> g g");		

	nchic01 = new RhoTuple("nchic01",	"chi_c0 -> pi+ pi- pi0 pi0");		
	nchic02 = new RhoTuple("nchic02",	"chi_c0 -> pi+ pi- pi+ pi-");		
	
	// *** create selectors
	fPi0Sel     = new RhoMassParticleSelector("pi0Sel",   fPi0SelMean, fPi0SelWin*2.);
	fPi0PreSel  = new RhoMassParticleSelector("pi0PreSel",fPi0SelMean, fPi0SelWin*8.);  // 4 times wider than narrow
	
	fEtaSel     = new RhoMassParticleSelector("etaSel",   fEtaSelMean, fEtaSelWin*2.);
	fEtaPreSel  = new RhoMassParticleSelector("etaPreSel",fEtaSelMean, fEtaSelWin*8.);  // 4 times wider than narrow
	
	fKs0Sel     = new RhoMassParticleSelector("Ks0Sel",   fKs0SelMean, fKs0SelWin*2.);
	fKs0PreSel  = new RhoMassParticleSelector("Ks0PreSel",fKs0SelMean, fKs0SelWin*8.);  // 4 times wider than narrow
	
	
	fPhiSel     = new RhoMassParticleSelector("PhiSel",   fPhiSelMean, fPhiSelWin*2.);
	fPhiPreSel  = new RhoMassParticleSelector("PhiPreSel",fPhiSelMean, fPhiSelWin*8.);  // 4 times wider than narrow
	
	fLamSel     = new RhoMassParticleSelector("LamSel",   fLamSelMean, fLamSelWin*2.);
	fLamPreSel  = new RhoMassParticleSelector("LamPreSel",fLamSelMean, fLamSelWin*8.);  // 4 times wider than narrow
	
	fJpsiSel     = new RhoMassParticleSelector("JpsiSel",   fJpsiSelMean, fJpsiSelWin*2.);
	fJpsiPreSel  = new RhoMassParticleSelector("JpsiPreSel",fJpsiSelMean, fJpsiSelWin*8.);  // 4 times wider than narrow
	
	fD0Sel     = new RhoMassParticleSelector("D0Sel",   fD0SelMean, fD0SelWin*2.);
	fD0PreSel  = new RhoMassParticleSelector("D0PreSel",fD0SelMean, fD0SelWin*8.);  // 4 times wider than narrow
	
	fDpmSel     = new RhoMassParticleSelector("DpmSel",   fDpmSelMean, fDpmSelWin*2.);
	fDpmPreSel  = new RhoMassParticleSelector("DpmPreSel",fDpmSelMean, fDpmSelWin*8.);  // 4 times wider than narrow
	
	fDsSel     = new RhoMassParticleSelector("DsSel",   fDsSelMean, fDsSelWin*2.);
	fDsPreSel  = new RhoMassParticleSelector("DsPreSel",fDsSelMean, fDsSelWin*8.);  // 4 times wider than narrow
	
	fEtacSel     = new RhoMassParticleSelector("EtacSel",   fEtacSelMean, fEtacSelWin*2.);
	fEtacPreSel  = new RhoMassParticleSelector("EtacPreSel",fEtacSelMean, fEtacSelWin*8.);  // 4 times wider than narrow

	fChic0Sel     = new RhoMassParticleSelector("Chic0Sel",   fChic0SelMean, fChic0SelWin*2.);
	fChic0PreSel  = new RhoMassParticleSelector("Chic0PreSel",fChic0SelMean, fChic0SelWin*8.);  // 4 times wider than narrow
	
	fLamcSel     = new RhoMassParticleSelector("LamcSel",   fLamcSelMean, fLamcSelWin*2.);
	fLamcPreSel  = new RhoMassParticleSelector("LamcPreSel",fLamcSelMean, fLamcSelWin*8.);  // 4 times wider than narrow

	
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
	int tag_jpsi  = Tag_Jpsi_ll(njpsi);
	
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
	int tag_chic0  = tag_chic01 + tag_chic02;
	
	int tag_glob   = tag_phi + tag_lam + tag_jpsi + tag_d0 + tag_dpm
				   + tag_ds + tag_lamc + tag_etac + tag_chic0;
	
	
				   
	
	// *** write common information	
	ntp->Column("ev",		(Float_t) 	fEvtCount,	0.0f);
	ntp->Column("mode",		(Float_t)	fMode,		0.0f);
	ntp->Column("ecm",		(Float_t)	fEcm,		0.0f);

	qaP4("ini", fIniP4, ntp);
	
	ntp->Column("tagphi",	(Float_t)	tag_phi,	0.0f);
	ntp->Column("taglam",	(Float_t)	tag_lam,	0.0f);
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
	ntp->Column("tagchic0",	(Float_t)	tag_chic0,	0.0f);

	ntp->Column("tag",		(Float_t)	tag_glob,	0.0f);
	
	qaEventShape(ntp);
	
	ntp->DumpData();
	
}


void PndSoftTriggerTask::Finish()
{	
	ntp->GetInternalTree()->Write();			// overall info
	nks0->GetInternalTree()->Write();			// Ks0 QA
	npi0->GetInternalTree()->Write();			// pi0 QA
	neta->GetInternalTree()->Write();			// eta QA
	
	nphi->GetInternalTree()->Write();			// phi -> K+ K-					(49.8 %)
	nlam->GetInternalTree()->Write();			// Lambda -> p pi-				(63.9 %)
	njpsi->GetInternalTree()->Write();			// J/psi -> e+e- / mu+ mu-		(11.9 %)
	
	nd01->GetInternalTree()->Write();			// D0 -> K- pi+					( 3.9 %)
	nd02->GetInternalTree()->Write();			// D0 -> K- pi+ pi0				(13.9 %)
	nd03->GetInternalTree()->Write();			// D0 -> K- pi+ pi+ pi-			( 8.1 %)
	ndpm1->GetInternalTree()->Write();			// D+- -> K- pi+ pi+			( 9.4 %)
	ndpm2->GetInternalTree()->Write();			// D+- -> K- pi+ pi+ pi0		( 6.1 %)
	ndpm3->GetInternalTree()->Write();			// D+- -> K_S pi+ pi0			( 6.9 %) 
	ndpm4->GetInternalTree()->Write();			// D+- -> K_S pi+ pi+ pi-		( 3.1 %) 
	nds1->GetInternalTree()->Write();			// Ds -> K+ K- pi+				( 5.5 %)
	nds2->GetInternalTree()->Write();			// Ds -> K+ K- pi+ pi0			( 5.6 %)
	
	nlamc->GetInternalTree()->Write();			// Lambda_c -> p K- pi+			( 5.0 %)
	
	netac1->GetInternalTree()->Write();			// eta_c -> K+ K- pi0 			( below) 
	netac2->GetInternalTree()->Write();			// eta_c -> K+- K_S pi-+        ( 3.2 %) = 7%/3 (1) + 7%/3*35% (2)
	netac3->GetInternalTree()->Write();			// eta_c -> eta (gg) pi+ pi-	( 1.9 %) = 4.9% * 39 %
	netac4->GetInternalTree()->Write();			// eta_c -> gg					( 0.0 %) = 6e-5

	nchic01->GetInternalTree()->Write();		// chi_c0 -> pi+ pi- pi0 pi0	( 3.4 %)
	nchic02->GetInternalTree()->Write();		// chi_c0 -> pi+ pi- pi+ pi-	( 2.3 %)
	
}

// -------------------------------------------------------------------------

int PndSoftTriggerTask::CreateKs0Cands(RhoTuple *n)
{
	// *** basic KS0 Reco
	fKs0Cands.Combine(fPionPlus, fPionMinus);
	// *** pre select for qa
	fKs0Cands.Select(fKs0PreSel);
	fKs0Cands.SetType(310);
	
	for (int i=0; i<fKs0Cands.GetLength();++i)
	{
		Float_t tag = (Float_t) fabs(fKs0Cands[i]->Mass()-fKs0SelMean)<fKs0SelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("mode",	(Float_t) fMode,		0.0f);
		n->Column("ksmean", (Float_t) fKs0SelMean,	0.0f);
		n->Column("kswin", 	(Float_t) fKs0SelWin,	0.0f);
		
		qaKs0("ks",fKs0Cands[i], n);
		
		n->DumpData();
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
		
		SelectPidProb(fProtonPlus, 3, fIniPidCut);
		SelectPidProb(fProtonMinus, 3, fIniPidCut);	
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
	for (i=0; i<fPi0Cands.GetLength();++i)
	{
		Float_t tag = (Float_t) fabs(fPi0Cands[i]->Mass()-fPi0SelMean)<fPi0SelWin;
		npi0->Column("tag", 	(Float_t) tag,			0.0f);
		npi0->Column("ev",  	(Float_t) fEvtCount,	0.0f);
		npi0->Column("num", 	(Float_t) i,			0.0f);
		npi0->Column("mode",	(Float_t) fMode,		0.0f);
		npi0->Column("pi0mean", (Float_t) fPi0SelMean,	0.0f);
		npi0->Column("pi0win", 	(Float_t) fPi0SelWin,	0.0f);
		
		qaPi0("pi0",fPi0Cands[i], npi0);
		npi0->DumpData();
	}
	fPi0Cands.Select(fPi0Sel);
	
	// *** era reco
	fEtaCands.Combine(fGammaCands, fGammaCands);
	fEtaCands.Select(fEtaPreSel);
	fEtaCands.SetType(221);
	for (i=0; i<fEtaCands.GetLength();++i)
	{
		Float_t tag = (Float_t) fabs(fEtaCands[i]->Mass()-fEtaSelMean)<fEtaSelWin;
		npi0->Column("tag", 	(Float_t) tag,			0.0f);
		neta->Column("ev",  	(Float_t) fEvtCount,	0.0f);
		neta->Column("num", 	(Float_t) i,			0.0f);
		neta->Column("mode",	(Float_t) fMode,		0.0f);
		neta->Column("etamean", (Float_t) fEtaSelMean,	0.0f);
		neta->Column("etawin", 	(Float_t) fEtaSelWin,	0.0f);
		
		qaPi0("eta",fEtaCands[i], neta);
		neta->DumpData();
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
	for (int i=0;i<l.GetLength();++i)
	{
		qaComp("phi", l[i], n);

		Float_t tag = (Float_t) fabs(l[i]->Mass()-fPhiSelMean)<fPhiSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("phimean",(Float_t) fPhiSelMean,	0.0f);
		n->Column("phiwin", (Float_t) fPhiSelWin,	0.0f);
		
		n->DumpData();
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("lam", l[i], n);

		Float_t tag = (Float_t) fabs(l[i]->Mass()-fLamSelMean)<fLamSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("lammean",(Float_t) fLamSelMean,	0.0f);
		n->Column("lamwin", (Float_t) fLamSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fLamSel);
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// *** J/psi -> l+ l- decays

int PndSoftTriggerTask::Tag_Jpsi_ll(RhoTuple *n)
{
	if (!fTagJpsill) return 0;
	if ( fEcm < DbMass("J/psi") ) return 0;
	
	RhoCandList l;
	
	// *** combinatorics
	l.Combine(fElectronPlus, fElectronMinus);
	l.CombineAndAppend(fMuonPlus, fMuonMinus);
	l.SetType(443);
	
	// *** pre selection for QA
	l.Select(fJpsiPreSel);
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("jpsi", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fJpsiSelMean)<fJpsiSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("jpsimean",(Float_t) fJpsiSelMean,0.0f);
		n->Column("jpsiwin", (Float_t) fJpsiSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fJpsiSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("d0", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fD0SelMean)<fD0SelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("d0mean",(Float_t) fD0SelMean,0.0f);
		n->Column("d0win", (Float_t) fD0SelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fD0Sel);
		
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("d0", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fD0SelMean)<fD0SelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("d0mean",(Float_t) fD0SelMean,0.0f);
		n->Column("d0win", (Float_t) fD0SelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fD0Sel);
		
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("d0", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fD0SelMean)<fD0SelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("d0mean",(Float_t) fD0SelMean,0.0f);
		n->Column("d0win", (Float_t) fD0SelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fD0Sel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("dpm", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fDpmSelMean)<fDpmSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("dpmmean",(Float_t) fDpmSelMean,0.0f);
		n->Column("dpmwin", (Float_t) fDpmSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fDpmSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("dpm", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fDpmSelMean)<fDpmSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("dpmmean",(Float_t) fDpmSelMean,0.0f);
		n->Column("dpmwin", (Float_t) fDpmSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fDpmSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("dpm", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fDpmSelMean)<fDpmSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("dpmmean",(Float_t) fDpmSelMean,0.0f);
		n->Column("dpmwin", (Float_t) fDpmSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fDpmSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("dpm", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fDpmSelMean)<fDpmSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("dpmmean",(Float_t) fDpmSelMean,0.0f);
		n->Column("dpmwin", (Float_t) fDpmSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fDpmSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("ds", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fDsSelMean)<fDsSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("dsmean",(Float_t) fDsSelMean,0.0f);
		n->Column("dswin", (Float_t) fDsSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fDsSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("ds", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fDsSelMean)<fDsSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("dsmean",(Float_t) fDsSelMean,0.0f);
		n->Column("dswin", (Float_t) fDsSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fDsSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("lamc", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fLamcSelMean)<fLamcSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("lamcmean",(Float_t) fLamcSelMean,0.0f);
		n->Column("lamcwin", (Float_t) fLamcSelWin,	0.0f);
		
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("etac", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fEtacSelMean)<fEtacSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("etacmean",(Float_t) fEtacSelMean,0.0f);
		n->Column("etacwin", (Float_t) fEtacSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fEtacSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("etac", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fEtacSelMean)<fEtacSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("etacmean",(Float_t) fEtacSelMean,0.0f);
		n->Column("etacwin", (Float_t) fEtacSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fEtacSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("etac", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fEtacSelMean)<fEtacSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("etacmean",(Float_t) fEtacSelMean,0.0f);
		n->Column("etacwin", (Float_t) fEtacSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fEtacSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("etac", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fEtacSelMean)<fEtacSelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	(Float_t) i,			0.0f);
		n->Column("etacmean",(Float_t) fEtacSelMean,0.0f);
		n->Column("etacwin", (Float_t) fEtacSelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fEtacSel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("chic0", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fChic0SelMean)<fChic0SelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	  (Float_t) i,				0.0f);
		n->Column("chic0mean",(Float_t) fChic0SelMean,	0.0f);
		n->Column("chic0win", (Float_t) fChic0SelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fChic0Sel);
	
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
	
	for (int i=0;i<l.GetLength();++i)
	{
		// *** store QA info
		qaComp("chic0", l[i], n);
		
		Float_t tag = (Float_t) fabs(l[i]->Mass()-fChic0SelMean)<fChic0SelWin;
		n->Column("tag", 	(Float_t) tag,			0.0f);
		n->Column("num", 	  (Float_t) i,				0.0f);
		n->Column("chic0mean",(Float_t) fChic0SelMean,	0.0f);
		n->Column("chic0win", (Float_t) fChic0SelWin,	0.0f);
		
		n->DumpData();
	}
	
	// *** final tag selection
	l.Select(fChic0Sel);
	
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

void PndSoftTriggerTask::CountPidCutMult(double prob, int &ne, int &nmu, int &npi, int &nk, int &np, double mom)
{
	ne = nmu = npi = nk = np = 0;
	
	int i;
	
	for (i=0;i<fElectronPlus.GetLength(); ++i)  
		if (fElectronPlus[i]->P()>mom && fElectronPlus[i]->GetPidInfo(0)>prob) ne++;
		
	for (i=0;i<fElectronMinus.GetLength(); ++i) 
		if (fElectronMinus[i]->P()>mom && fElectronMinus[i]->GetPidInfo(0)>prob) ne++;

	for (i=0;i<fMuonPlus.GetLength(); ++i)  
		if (fMuonPlus[i]->P()>mom && fMuonPlus[i]->GetPidInfo(1)>prob) nmu++;
		
	for (i=0;i<fMuonMinus.GetLength(); ++i) 
		if (fMuonMinus[i]->P()>mom && fMuonMinus[i]->GetPidInfo(1)>prob) nmu++;

	for (i=0;i<fPionPlus.GetLength(); ++i)  
		if (fPionPlus[i]->P()>mom && fPionPlus[i]->GetPidInfo(2)>prob) npi++;
		
	for (i=0;i<fPionMinus.GetLength(); ++i) 
		if (fPionMinus[i]->P()>mom && fPionMinus[i]->GetPidInfo(2)>prob) npi++;

	for (i=0;i<fKaonPlus.GetLength(); ++i)  
		if (fKaonPlus[i]->P()>mom && fKaonPlus[i]->GetPidInfo(3)>prob) nk++;
		
	for (i=0;i<fKaonMinus.GetLength(); ++i) 
		if (fKaonMinus[i]->P()>mom && fKaonMinus[i]->GetPidInfo(3)>prob) nk++;

	for (i=0;i<fProtonPlus.GetLength(); ++i)  
		if (fProtonPlus[i]->P()>mom && fProtonPlus[i]->GetPidInfo(4)>prob) np++;
		
	for (i=0;i<fProtonMinus.GetLength(); ++i) 
		if (fProtonMinus[i]->P()>mom && fProtonMinus[i]->GetPidInfo(4)>prob) np++;

}


// -------------------------------------------------------------------------
void PndSoftTriggerTask::qaEventShape(RhoTuple *n)
{
	// *** vars for PID multiplicity
	int ne, nmu, npi, nk, np;
	
	n->Column("npart",(Float_t)	  fEventShape->NParticles(),0.0f );
	n->Column("ngam", (Float_t)	  fEventShape->NNeutral(),	0.0f );
	n->Column("ntrk", (Float_t)   fEventShape->NCharged(),	0.0f );
	
	// PID multiplicities with P>0.25 (loose)
	CountPidCutMult(0.25, ne, nmu, npi, nk, np);
	n->Column("npidel", (Float_t) 	  ne,	0.0f );
	n->Column("npidmul",(Float_t) 	  nmu,	0.0f );
	n->Column("npidpil",(Float_t) 	  npi,	0.0f );
	n->Column("npidkl", (Float_t)	  nk,	0.0f );
	n->Column("npidpl", (Float_t) 	  np,	0.0f );
	
	// PID multiplicities with P>0.25 for particles with p>1.0 GeV/c
	CountPidCutMult(0.25, ne, nmu, npi, nk, np, 1.0);
	n->Column("npidel1", (Float_t)   ne,	0.0f );
	n->Column("npidmul1",(Float_t)   nmu,	0.0f );
	n->Column("npidpil1",(Float_t)   npi,	0.0f );
	n->Column("npidkl1", (Float_t)	  nk,	0.0f );
	n->Column("npidpl1", (Float_t)   np,	0.0f );
	
	// PID multiplicities with P>0.5 (tight)
	CountPidCutMult(0.5, ne, nmu, npi, nk, np);
	n->Column("npidet", (Float_t) 	  ne,	0.0f );
	n->Column("npidmut",(Float_t) 	  nmu,	0.0f );
	n->Column("npidpit",(Float_t) 	  npi,	0.0f );
	n->Column("npidkt", (Float_t)	  nk,	0.0f );
	n->Column("npidpt", (Float_t) 	  np,	0.0f );
	
	// PID multiplicities with P>0.5 for particles with p>1.0 GeV/c
	CountPidCutMult(0.5, ne, nmu, npi, nk, np, 1.0);
	n->Column("npidet1", (Float_t)   ne,	0.0f );
	n->Column("npidmut1",(Float_t)   nmu,	0.0f );
	n->Column("npidpit1",(Float_t)   npi,	0.0f );
	n->Column("npidkt1", (Float_t)	  nk,	0.0f );
	n->Column("npidpt1", (Float_t)   np,	0.0f );
	
	// PID multiplities with P>0.9 (very tight)
	CountPidCutMult(0.9, ne, nmu, npi, nk, np);
	n->Column("npidevt", (Float_t)   ne,	0.0f );
	n->Column("npidmuvt",(Float_t)   nmu,	0.0f );
	n->Column("npidpivt",(Float_t)   npi,	0.0f );
	n->Column("npidkvt", (Float_t)	  nk,	0.0f );
	n->Column("npidpvt", (Float_t)   np,	0.0f );
	
	n->Column("npi0", (Float_t)	  fPi0Cands.GetLength(),	0.0f );
	n->Column("nks0",(Float_t) 	  fKs0Cands.GetLength(),	0.0f );
	n->Column("neta",(Float_t) 	  fEtaCands.GetLength(),	0.0f );
	
	n->Column("pmax", (Float_t)	  fEventShape->PmaxCms(),	0.0f );
	n->Column("ptmax",(Float_t)	  fEventShape->Ptmax() ,	0.0f );
	n->Column("pmaxl",(Float_t)	  fEventShape->PmaxLab() ,	0.0f );
	n->Column("ptmaxl",(Float_t)  fEventShape->Ptmax(),		0.0f );
	n->Column("pmin", (Float_t)	  fEventShape->PminCms(),	0.0f );
	n->Column("ptmin",(Float_t)   fEventShape->Ptmin(),		0.0f );
	n->Column("pminl",(Float_t)	  fEventShape->PminLab(),	0.0f );
	
	n->Column("prapmax",(Float_t) fEventShape->PRapmax(),		0.0f );
	n->Column("sph", (Float_t)	  fEventShape->Sphericity(),	0.0f );
	n->Column("apl", (Float_t)	  fEventShape->Aplanarity(),	0.0f );
	n->Column("pla", (Float_t)	  fEventShape->Planarity(),		0.0f );
	n->Column("thr",(Float_t)	  fEventShape->Thrust(),		0.0f );
	n->Column("cir", (Float_t)	  fEventShape->Circularity(),	0.0f );	

	n->Column("fw1", (Float_t)	  fEventShape->FoxWolfMomR(1),0.0f );
	n->Column("fw2", (Float_t)	  fEventShape->FoxWolfMomR(2),0.0f );
	n->Column("fw3", (Float_t)	  fEventShape->FoxWolfMomR(3),0.0f );
	n->Column("fw4", (Float_t)	  fEventShape->FoxWolfMomR(4),0.0f );
	n->Column("fw5", (Float_t)	  fEventShape->FoxWolfMomR(5),0.0f );

	n->Column("np05", (Float_t)	  fEventShape->MultPminCms(0.5),	0.0f );
	n->Column("np10", (Float_t)	  fEventShape->MultPminCms(1.0),	0.0f );
	n->Column("np20", (Float_t)	  fEventShape->MultPminCms(2.0),	0.0f );
	n->Column("np30", (Float_t)	  fEventShape->MultPminCms(3.0),	0.0f );
	n->Column("np40", (Float_t)	  fEventShape->MultPminCms(4.0),	0.0f );
	n->Column("np50", (Float_t)	  fEventShape->MultPminCms(5.0),	0.0f );
	
	n->Column("np05l", (Float_t)  fEventShape->MultPminLab(0.5),	0.0f );
	n->Column("np10l", (Float_t)  fEventShape->MultPminLab(1.0),	0.0f );
	n->Column("np20l", (Float_t)  fEventShape->MultPminLab(2.0),	0.0f );
	n->Column("np30l", (Float_t)  fEventShape->MultPminLab(3.0),	0.0f );
	n->Column("np40l", (Float_t)  fEventShape->MultPminLab(4.0),	0.0f );
	n->Column("np50l", 	(Float_t)  fEventShape->MultPminLab(5.0),	0.0f );
	
	n->Column("npt05", 	(Float_t)  fEventShape->MultPtminCms(0.5),	0.0f );
	n->Column("npt10", 	(Float_t)  fEventShape->MultPtminCms(1.0),	0.0f );
	n->Column("npt15", 	(Float_t)  fEventShape->MultPtminCms(1.5),	0.0f );
	n->Column("npt20", 	(Float_t)  fEventShape->MultPtminCms(2.0),	0.0f );
	n->Column("npt25", 	(Float_t)  fEventShape->MultPtminCms(2.5),	0.0f );
	n->Column("npt30", 	(Float_t)  fEventShape->MultPtminCms(3.0),	0.0f );
	
	n->Column("nne003l",  (Float_t) fEventShape->MultNeutEminLab(0.03),	0.0f );
	n->Column("nne005l",  (Float_t) fEventShape->MultNeutEminLab(0.05),	0.0f );
	n->Column("nne01l",   (Float_t) fEventShape->MultNeutEminLab(0.1),	0.0f );
	n->Column("nne05l",   (Float_t) fEventShape->MultNeutEminLab(0.5),	0.0f );
		
	n->Column("ncp005l", (Float_t)	  fEventShape->MultChrgPminLab(0.05),	0.0f );
	n->Column("ncp01l", (Float_t)	  fEventShape->MultChrgPminLab(0.1),	0.0f );
	n->Column("ncp02l", (Float_t)	  fEventShape->MultChrgPminLab(0.2),	0.0f );
	n->Column("ncp05l", (Float_t)	  fEventShape->MultChrgPminLab(0.5),	0.0f );
	n->Column("ncp10l", (Float_t)	  fEventShape->MultChrgPminLab(1.0),	0.0f );
	
	n->Column("ncp005", (Float_t)	  fEventShape->MultChrgPminCms(0.05),	0.0f );
	n->Column("ncp01", (Float_t)	  fEventShape->MultChrgPminCms(0.1),	0.0f );
	n->Column("ncp02", (Float_t)	  fEventShape->MultChrgPminCms(0.2),	0.0f );
	n->Column("ncp05", (Float_t)	  fEventShape->MultChrgPminCms(0.5),	0.0f );
	n->Column("ncp10", (Float_t)	  fEventShape->MultChrgPminCms(1.0),	0.0f );
		
	n->Column("sumpc", 	(Float_t)  fEventShape->ChrgPSumCms(),		0.0f );
	n->Column("sumpcl", (Float_t)  fEventShape->ChrgPSumLab(),		0.0f );
	n->Column("sumen", 	(Float_t)  fEventShape->NeutESumCms(),		0.0f );
	n->Column("sumenl", (Float_t)  fEventShape->NeutESumLab(),		0.0f );

	n->Column("sumpt", 	(Float_t)  fEventShape->PtSumCms(),			0.0f );
	n->Column("sumptl",  (Float_t) fEventShape->PtSumLab(),			0.0f );
	n->Column("sumptc", (Float_t)  fEventShape->ChrgPtSumCms(),		0.0f );
	n->Column("sumptcl",(Float_t)  fEventShape->ChrgPtSumLab(),		0.0f );
	n->Column("sumetn",  (Float_t) fEventShape->NeutEtSumCms(),		0.0f );
	n->Column("sumetnl", (Float_t) fEventShape->NeutEtSumLab(),		0.0f );

	n->Column("sumpt05", (Float_t) fEventShape->SumPtminCms(0.5),	0.0f );
	n->Column("sumpt10", (Float_t) fEventShape->SumPtminCms(1.0),	0.0f );
	
	n->Column("sumpc05", (Float_t) fEventShape->SumChrgPminCms(0.5) ,0.0f );
	n->Column("sumpc10", (Float_t) fEventShape->SumChrgPminCms(1.0),0.0f );
	
	n->Column("sumpc05l", (Float_t)fEventShape->SumChrgPminLab(0.5) ,0.0f );
	n->Column("sumpc10l",(Float_t) fEventShape->SumChrgPminLab(1.0) ,0.0f );
	
	n->Column("sumen05",  (Float_t)fEventShape->SumNeutEminCms(0.5) ,0.0f );
	n->Column("sumen10", (Float_t) fEventShape->SumNeutEminCms(1.0) ,0.0f );
	
	n->Column("sumen05l",(Float_t) fEventShape->SumNeutEminLab(0.5) ,0.0f );
	n->Column("sumen10l",(Float_t) fEventShape->SumNeutEminLab(1.0) ,0.0f );
	
}
// -------------------------------------------------------------------------
void PndSoftTriggerTask::qaEventShapeShort(RhoTuple *n)
{
	// *** vars for PID multiplicity
	int ne, nmu, npi, nk, np;
	
	n->Column("npart",(Float_t)	  fEventShape->NParticles(),0.0f );
	n->Column("ngam", (Float_t)	  fEventShape->NNeutral(),	0.0f );
	n->Column("ntrk", (Float_t)   fEventShape->NCharged(),	0.0f );
	
	// PID multiplicities with P>0.25 (loose)
	CountPidCutMult(0.25, ne, nmu, npi, nk, np);
	n->Column("npidel", (Float_t) 	  ne,	0.0f );
	n->Column("npidmul",(Float_t) 	  nmu,	0.0f );
	n->Column("npidpil",(Float_t) 	  npi,	0.0f );
	n->Column("npidkl", (Float_t)	  nk,	0.0f );
	n->Column("npidpl", (Float_t) 	  np,	0.0f );
	
	// PID multiplicities with P>0.25 for particles with p>1.0 GeV/c
	CountPidCutMult(0.25, ne, nmu, npi, nk, np, 1.0);
	n->Column("npidel1", (Float_t)   ne,	0.0f );
	n->Column("npidmul1",(Float_t)   nmu,	0.0f );
	n->Column("npidpil1",(Float_t)   npi,	0.0f );
	n->Column("npidkl1", (Float_t)	  nk,	0.0f );
	n->Column("npidpl1", (Float_t)   np,	0.0f );
		
	n->Column("npi0", (Float_t)	  fPi0Cands.GetLength(),	0.0f );
	n->Column("nks0",(Float_t) 	  fKs0Cands.GetLength(),	0.0f );
	n->Column("neta",(Float_t) 	  fEtaCands.GetLength(),	0.0f );
	
	n->Column("pmax", (Float_t)	  fEventShape->PmaxCms(),	0.0f );
	n->Column("pmaxl",(Float_t)	  fEventShape->PmaxLab() ,	0.0f );
	n->Column("ptmax",(Float_t)	  fEventShape->Ptmax() ,	0.0f );
	n->Column("pmin", (Float_t)	  fEventShape->PminCms(),	0.0f );
	
	n->Column("prapmax",(Float_t) fEventShape->PRapmax(),		0.0f );
	n->Column("sph", (Float_t)	  fEventShape->Sphericity(),	0.0f );
	n->Column("apl", (Float_t)	  fEventShape->Aplanarity(),	0.0f );
	n->Column("pla", (Float_t)	  fEventShape->Planarity(),		0.0f );
	n->Column("thr",(Float_t)	  fEventShape->Thrust(),		0.0f );
	n->Column("cir", (Float_t)	  fEventShape->Circularity(),	0.0f );	

	n->Column("fw1", (Float_t)	  fEventShape->FoxWolfMomR(1),0.0f );
	n->Column("fw2", (Float_t)	  fEventShape->FoxWolfMomR(2),0.0f );
	n->Column("fw3", (Float_t)	  fEventShape->FoxWolfMomR(3),0.0f );
	n->Column("fw4", (Float_t)	  fEventShape->FoxWolfMomR(4),0.0f );
	n->Column("fw5", (Float_t)	  fEventShape->FoxWolfMomR(5),0.0f );

	n->Column("np10",  (Float_t)  fEventShape->MultPminCms(1.0),	 0.0f );
	n->Column("npt10", (Float_t)  fEventShape->MultPtminCms(1.0),	 0.0f );
	n->Column("ncp10l", (Float_t) fEventShape->MultChrgPminLab(1.0), 0.0f );
	n->Column("nne10l",(Float_t)  fEventShape->MultNeutEminLab(1.0), 0.0f );
		
	n->Column("sumpc05", (Float_t) fEventShape->SumChrgPminCms(0.5) ,0.0f );
	
	n->Column("sumpc", 	(Float_t)  fEventShape->ChrgPSumCms(),		0.0f );
	n->Column("sumpcl", (Float_t)  fEventShape->ChrgPSumLab(),		0.0f );
	n->Column("sumen", 	(Float_t)  fEventShape->NeutESumCms(),		0.0f );
	n->Column("sumenl", (Float_t)  fEventShape->NeutESumLab(),		0.0f );

	n->Column("sumpt", 	(Float_t)  fEventShape->PtSumCms(),			0.0f );
	n->Column("sumptl",  (Float_t) fEventShape->PtSumLab(),			0.0f );
	n->Column("sumptcl",(Float_t)  fEventShape->ChrgPtSumLab(),		0.0f );
	
}

// -------------------------------------------------------------------------
// *** store QA for composite particles

void PndSoftTriggerTask::qaComp(TString pre, RhoCandidate *c, RhoTuple *n)
{
	// *** simple vtx finder
	TVector3 vtx;
	double qavtx = fVtxPoca->GetPocaVtx(vtx, c);
	
	// *** MC truth match
	fAnalysis->McTruthMatch(c);
	RhoCandidate *truth = c->GetMcTruth();
	
	// *** store QA info
	n->Column("ev",  	(Float_t) fEvtCount, 	0.0f);
	n->Column("mode",	(Float_t) fMode,		0.0f);

	n->Column(pre+"vx",  (Float_t) vtx.X(), 0.0f);
	n->Column(pre+"vy",  (Float_t) vtx.Y(), 0.0f);
	n->Column(pre+"vz",  (Float_t) vtx.Z(), 0.0f);
	n->Column(pre+"vqa", (Float_t) qavtx,   0.0f);
	
	qaCand(pre,	c,	n);
	qaP4Cms(pre, c->P4(), n);
	qaEventShapeShort(n);
	
	int nd = c->NDaughters();
	if (nd==2) 
		qa2Body(pre, c, n);
	
	for (int i=0;i<nd;++i)
	{
		RhoCandidate *dau = c->Daughter(i);
		TString name=TString::Format("%sd%d",pre.Data(),i);
		qaCand(name, dau, n);
		if ( fabs(dau->Charge())>0.1 && dau->NDaughters()==0 )
			qaPid(name, dau, n);
		if ( dau->PdgCode()==111) //pi0
			qaPi0(name, dau, n);
		if ( dau->PdgCode()==221) //eta
			qaPi0(name, dau, n);
		if ( dau->PdgCode()==310) //KS
			qaKs0(name, dau, n);		
	}
		
	if (truth!=0)
	{
		qaCand("t"+pre, truth, n);
		n->Column("mct", 1.0f, 0.0f);
	}
	else
	{
		qaCand("t"+pre, NULL , n, true);
		n->Column("mct", 0.0f, 0.0f);
	}
		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaPi0(TString pre, RhoCandidate *c, RhoTuple *n)
{
	qaCand(pre, c, n);
	
	RhoCandidate *d0 = c->Daughter(0);
	RhoCandidate *d1 = c->Daughter(1);
	double ang = d0->P3().Angle(d1->P3());
	
	fAnalysis->McTruthMatch(c);
	RhoCandidate *truth = c->GetMcTruth();
	
	qaCand(pre, c, n);
	n->Column(pre+"oang",(Float_t) ang,		0.0f);
	
	qaCand(pre+"g1", d0, n); 
	qaEmc(pre+"g1", d0, n); 

	qaCand(pre+"g2", d1, n); 
	qaEmc(pre+"g2", d1, n); 
	
	if (truth!=0)
	{
		qaCand("t"+pre, truth, n);
		n->Column(pre+"mct", 1.0f, 0.0f);
	}
	else
	{
		qaCand("t"+pre, NULL , n, true);
		n->Column(pre+"mct", 0.0f, 0.0f);
	}
}
// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaKs0(TString pre, RhoCandidate *c, RhoTuple *n)
{
	RhoCandidate *d0 = c->Daughter(0);
	RhoCandidate *d1 = c->Daughter(1);
	double ang = d0->P3().Angle(d1->P3());
	
	TVector3 vtx;
	
	fAnalysis->McTruthMatch(c);
	RhoCandidate *truth = c->GetMcTruth();
	
	double qavtx = fVtxPoca->GetPocaVtx(vtx, c);
	
	qaCand(pre, c, n);
	
	n->Column(pre+"vx",  (Float_t) vtx.X(), 0.0f);
	n->Column(pre+"vy",  (Float_t) vtx.Y(), 0.0f);
	n->Column(pre+"vz",  (Float_t) vtx.Z(), 0.0f);
	n->Column(pre+"vqa", (Float_t) qavtx,   0.0f);
	
	n->Column(pre+"oang",(Float_t) ang,		0.0f);
	
	qaCand(pre+"pi1", d0, n); 
	qaPid(pre+"pi1",  d0, n); 

	qaCand(pre+"pi2", d1, n); 
	qaPid(pre+"pi2",  d1, n); 

	if (truth!=0)
	{
		qaCand("t"+pre, truth, n);
		n->Column(pre+"mct", 1.0f, 0.0f);
	}
	else
	{
		qaCand("t"+pre, NULL , n, true);
		n->Column(pre+"mct", 0.0f, 0.0f);
	}
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaP4(TString pre, TLorentzVector &c, RhoTuple *n, bool skip)
{
	if (!skip)
	{
		n->Column(pre+"px",  (Float_t) c.Px(),     0.0f );
		n->Column(pre+"py",  (Float_t) c.Py(),     0.0f );
		n->Column(pre+"pz",  (Float_t) c.Pz(),     0.0f );
		n->Column(pre+"e",   (Float_t) c.E(),      0.0f );
		n->Column(pre+"p",   (Float_t) c.P(),      0.0f );
		n->Column(pre+"tht", (Float_t) c.Theta(),  0.0f );
		n->Column(pre+"phi", (Float_t) c.Phi(),    0.0f );
		n->Column(pre+"pt",  (Float_t) c.Pt(),     0.0f );
		n->Column(pre+"m",   (Float_t) c.M(),      0.0f );
	}
	else
	{
		n->Column(pre+"px",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"py",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"pz",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"e",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"p",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"tht", (Float_t) -999., 			0.0f );
		n->Column(pre+"phi", (Float_t) -999.,  			0.0f );
		n->Column(pre+"pt",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"m",   (Float_t) -999.,  			0.0f );
	}
}
// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaP4Cms(TString pre, TLorentzVector c, RhoTuple *n, bool skip)
{
	if (!skip)
	{
		c.Boost(-fIniP4.BoostVector());
		
		n->Column(pre+"pxcm",  (Float_t) c.Px(),     0.0f );
		n->Column(pre+"pycm",  (Float_t) c.Py(),     0.0f );
		n->Column(pre+"pzcm",  (Float_t) c.Pz(),     0.0f );
		n->Column(pre+"ecm",   (Float_t) c.E(),      0.0f );
		n->Column(pre+"pcm",   (Float_t) c.P(),      0.0f );
		n->Column(pre+"thtcm", (Float_t) c.Theta(),  0.0f );
		n->Column(pre+"phicm", (Float_t) c.Phi(),    0.0f );
	}
	else
	{
		n->Column(pre+"pxcm",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"pycm",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"pzcm",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"ecm",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"pcm",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"thtcm", (Float_t) -999., 			0.0f );
		n->Column(pre+"phicm", (Float_t) -999.,  			0.0f );
	}
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaCand(TString pre, RhoCandidate *cc, RhoTuple *n, bool skip)
{
	if (!skip)
	{
		TLorentzVector c=cc->P4();
		TVector3       p=cc->Pos();
		
		n->Column(pre+"px",  (Float_t) c.Px(),     		0.0f );
		n->Column(pre+"py",  (Float_t) c.Py(),			0.0f );
		n->Column(pre+"pz",  (Float_t) c.Pz(),			0.0f );
		n->Column(pre+"e",   (Float_t) c.E(),			0.0f );
		n->Column(pre+"p",   (Float_t) c.P(),      		0.0f );
		n->Column(pre+"tht", (Float_t) c.Theta(),  		0.0f );
		n->Column(pre+"phi", (Float_t) c.Phi(),    		0.0f );
		n->Column(pre+"pt",  (Float_t) c.Pt(),     		0.0f );
		n->Column(pre+"m",   (Float_t) c.M(),      		0.0f );
		n->Column(pre+"chg", (Float_t) cc->Charge(), 	0.0f );
		n->Column(pre+"x",   (Float_t) p.X(),     		0.0f );
		n->Column(pre+"y",   (Float_t) p.Y(),			0.0f );
		n->Column(pre+"z",   (Float_t) p.Z(),			0.0f );
		n->Column(pre+"l",   (Float_t) p.Mag(),			0.0f );
		n->Column(pre+"pdg", (Float_t) cc->PdgCode(),	0.0f );
	}
	else
	{
		n->Column(pre+"px",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"py",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"pz",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"e",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"p",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"tht", (Float_t) -999., 			0.0f );
		n->Column(pre+"phi", (Float_t) -999.,  			0.0f );
		n->Column(pre+"pt",  (Float_t) -999.,  			0.0f );
		n->Column(pre+"m",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"chg", (Float_t) -999., 			0.0f );
		n->Column(pre+"x",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"y",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"z",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"l",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"pdg", (Float_t) -999.,			0.0f );
	}
}
// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaTrk(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"dof",  	(Float_t) mic->GetDegreesOfFreedom(),	0.0f );
		n->Column(pre+"stat",  	(Float_t) mic->GetFitStatus(),			0.0f ); 
		n->Column(pre+"chi2",  	(Float_t) mic->GetChiSquared(),			0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetTrackIndex(),			0.0f );	
		n->Column(pre+"branch",	(Float_t) mic->GetTrackBranch() ,		0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaPid(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	// pinf[0]...pinf[4]: cache P values, pinf[5] = maximum
	double pinf[6] = {0.,0.,0.,0.,0.,-1000.};
	// index of particle type (e=0 ... p=4) with maximum P 
	int bestidx = -1;

	for (int i=0;i<5;++i)
	{
		pinf[i] = c->GetPidInfo(i);
		
		if (pinf[i]>pinf[5])
		{
			pinf[5] = pinf[i];
			bestidx = i;
		}
	}
	
	n->Column(pre+"pide",  	(Float_t) pinf[0],		0.0f );
	n->Column(pre+"pidmu", 	(Float_t) pinf[1],		0.0f ); 
	n->Column(pre+"pidpi",	(Float_t) pinf[2],		0.0f );
	n->Column(pre+"pidk",  	(Float_t) pinf[3],		0.0f );	
	n->Column(pre+"pidp",  	(Float_t) pinf[4],		0.0f );

	n->Column(pre+"pidmax",	(Float_t) pinf[5],		0.0f );
	n->Column(pre+"pidbest",(Float_t) bestidx,		0.0f );
	
}

// -------------------------------------------------------------------------
void PndSoftTriggerTask::qa2Body(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	if (c->NDaughters()!=2) return;
	
	RhoCandidate *d0 = c->Daughter(0);
	RhoCandidate *d1 = c->Daughter(1);
	
	// opening angle lab
	double oang = d0->P3().Angle(d1->P3());
	
	// decay angle
	TLorentzVector d_cms = d0->P4();
	d_cms.Boost(-c->P3());
	Float_t dec  = d_cms.Vect().Angle(c->P());
	Float_t cdec = cos(dec);
	
	n->Column(pre+"oang", (Float_t) oang,		0.0f );
	n->Column(pre+"dec",  (Float_t) dec,		0.0f );
	n->Column(pre+"cdec", (Float_t) cdec,		0.0f );
}


// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaVtx(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	RhoCandidate *d = c->Daughter(0);
	TVector3 v = c->DecayVtx();
	if (d)
	{
		if (v.X()==0 && v.Y()==0 && v.Z()==0) 
			v = d->Pos();
		TLorentzVector d_cms = d->P4();
		d_cms.Boost(-c->P3());
		
		Float_t ctau = v.Mag()*c->M()/c->P();
		Float_t dec  = d_cms.Vect().Angle(c->P());
		Float_t cdec = cos(dec);
		
		n->Column(pre+"vx",  (Float_t) v.X(),		0.0f );
		n->Column(pre+"vy",  (Float_t) v.Y(),		0.0f );
		n->Column(pre+"vz",  (Float_t) v.Z(),		0.0f );
		n->Column(pre+"l", 	 (Float_t) v.Mag(),		0.0f );
		n->Column(pre+"ct",  (Float_t) ctau,		0.0f );
		n->Column(pre+"dec", (Float_t) dec,			0.0f );
		n->Column(pre+"cdec",(Float_t) cdec,		0.0f );
	}
	else
	{
		n->Column(pre+"vx",  (Float_t) -999.0,		0.0f );
		n->Column(pre+"vy",  (Float_t) -999.0,		0.0f );
		n->Column(pre+"vz",  (Float_t) -999.0,		0.0f );
		n->Column(pre+"l", 	 (Float_t) -999.0,		0.0f );
		n->Column(pre+"ct",  (Float_t) -999.0,		0.0f );
		n->Column(pre+"dec", (Float_t) -999.0,		0.0f );
		n->Column(pre+"cdec",(Float_t) -999.0,		0.0f );
	}
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaEmc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"eraw",  (Float_t) mic->GetEmcRawEnergy(),			0.0f );
		n->Column(pre+"ecal",  (Float_t) mic->GetEmcCalEnergy(),			0.0f );
		n->Column(pre+"qa",    (Float_t) mic->GetEmcQuality(),   			0.0f );
		n->Column(pre+"nx",    (Float_t) mic->GetEmcNumberOfCrystals(),		0.0f );
		n->Column(pre+"nb",    (Float_t) mic->GetEmcNumberOfBumps(),   		0.0f );
		n->Column(pre+"z20",   (Float_t) mic->GetEmcClusterZ20(),   		0.0f );
		n->Column(pre+"z53",   (Float_t) mic->GetEmcClusterZ53(),   		0.0f );
		n->Column(pre+"lat",   (Float_t) mic->GetEmcClusterLat(),   		0.0f );
		n->Column(pre+"e1",    (Float_t) mic->GetEmcClusterE1(),   			0.0f );
		n->Column(pre+"e9",    (Float_t) mic->GetEmcClusterE9(),   			0.0f );
		n->Column(pre+"e25",   (Float_t) mic->GetEmcClusterE25(),   		0.0f );
		n->Column(pre+"mod",   (Float_t) mic->GetEmcModule(),   			0.0f );
		n->Column(pre+"idx",   (Float_t) mic->GetEmcIndex(),   				0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaMvd(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"dedx",  (Float_t) mic->GetMvdDEDX(),			0.0f );
		n->Column(pre+"hits",  (Float_t) mic->GetMvdHits(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaStt(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"dedx",  (Float_t) mic->GetSttMeanDEDX(),		0.0f );
		n->Column(pre+"hits",  (Float_t) mic->GetSttHits(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaDrc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"thtc",  	(Float_t) mic->GetDrcThetaC(),			0.0f );
		n->Column(pre+"dthtc",  (Float_t) mic->GetDrcThetaCErr(),		0.0f );
		n->Column(pre+"qa",  	(Float_t) mic->GetDrcQuality(),			0.0f );
		n->Column(pre+"nphot",  (Float_t) mic->GetDrcNumberOfPhotons(),	0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetDrcIndex(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaDsc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"thtc",  	(Float_t) mic->GetDiscThetaC(),			0.0f );
		n->Column(pre+"dthtc",  (Float_t) mic->GetDiscThetaCErr(),		0.0f );
		n->Column(pre+"qa",  	(Float_t) mic->GetDiscQuality(),		0.0f );
		n->Column(pre+"nphot",  (Float_t) mic->GetDiscNumberOfPhotons(),0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetDiscIndex(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaMuo(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"nlay",  	(Float_t) mic->GetMuoNumberOfLayers(),	0.0f );;
		n->Column(pre+"prob",  	(Float_t) mic->GetMuoProbability(),		0.0f );;
		n->Column(pre+"qa",  	(Float_t) mic->GetMuoQuality() ,		0.0f ); ;
		n->Column(pre+"iron",  	(Float_t) mic->GetMuoIron() ,			0.0f );  ;  
		n->Column(pre+"pin",  	(Float_t) mic->GetMuoMomentumIn(),		0.0f );;
		n->Column(pre+"mod", 	(Float_t) mic->GetMuoModule(),			0.0f );  ;  
		n->Column(pre+"hits",  	(Float_t) mic->GetMuoHits(),			0.0f );  ;  
		n->Column(pre+"idx",  	(Float_t) mic->GetMuoIndex(),			0.0f ); ;    
	}		
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::qaTof(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"time", 	(Float_t) mic->GetTofStopTime(),	0.0f );
		n->Column(pre+"m2",  	(Float_t) mic->GetTofM2(),			0.0f );  
		n->Column(pre+"len",  	(Float_t) mic->GetTofTrackLength(),	0.0f );
		n->Column(pre+"qa",  	(Float_t) mic->GetTofQuality(),		0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetTofIndex(),		0.0f ); 
		n->Column(pre+"beta",  	(Float_t) mic->GetTofBeta(),		0.0f );
	}		
}


// -------------------------------------------------------------------------

ClassImp(PndSoftTriggerTask)
