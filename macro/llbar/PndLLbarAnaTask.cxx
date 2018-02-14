// The header file
#include "PndLLbarAnaTask.h"

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
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

// RHO headers
#include "RhoCandidate.h"
#include "PndPidCandidate.h"
#include "RhoHistogram/RhoTuple.h"
#include "RhoFactory.h"
#include "RhoMassParticleSelector.h"

// Analysis headers
#include "PndAnalysis.h"
#include "Rho4CFitter.h"
#include "RhoKinVtxFitter.h"
#include "RhoKinFitter.h"
#include "RhoVtxPoca.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndLLbarAnaTask::PndLLbarAnaTask() :
  FairTask("Panda LLbar Analysis Task") { 
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLLbarAnaTask::~PndLLbarAnaTask() { }
// -------------------------------------------------------------------------


// ----- Method to select true PID candidates
int PndLLbarAnaTask::SelectTruePid(PndAnalysis *ana, RhoCandList &l)
{
	int removed = 0;
	
	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if ( !(ana->McTruthMatch(l[ii])) )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	
	return removed;
}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndLLbarAnaTask::Init() 
{		
	// initialize analysis object
	theAnalysis = new PndAnalysis();

	// reset the event counter
	nevts = 0;

	// *** ------------------------- ***
	// *** Create some 1D histograms ***
	// *** ------------------------- ***

	//4C chi2 and prob histo
	hprob_4c = new TH1F("hprob_4c","#Lambda_{0}#Lambda_{0}bar Prob 4C fit",100,0,1);
	hchi2_4c = new TH1F("hchi2_4c","#Lambda_{0}#Lambda_{0}bar #chi^{2} 4C fit",100,0,250);

	//Lambda
	hlam0_M_all = new TH1F("hlam0_M_all","#Lambda_{0} mass",100,0,5);
	hlam0_M_4c = new TH1F("hlam0_M_4c","#Lambda_{0} mass, with 4c fit",100,0,5);

	//Lambdabar
	hlam0bar_M_all = new TH1F("hlam0bar_M_all","#Lambda_{0} bar mass",100,0,5);
	hlam0bar_M_4c = new TH1F("hlam0bar_M_4c","#Lambda_{0} bar mass, with 4c fit",100,0,5);

	//Piplus truthmatch histos
	hpiplus_tm_P = new TH1F("hpiplus_tm_P","tm #pi^{+} P",150,0.,3.);
	hpiplus_tm_Pt = new TH1F("hpiplus_tm_Pt","tm #pi^{+} Pt",150,0.,3.);
	hpiplus_tm_Theta = new TH1F("hpiplus_tm_Theta","tm #pi^{+} #theta",90,0.,180.);
	hpiplus_tm_cosTheta = new TH1F("hpiplus_tm_cosTheta","tm #pi^{+} cos#theta",100,-1.,1.);
	hpiplus_tm_cm_cosTheta = new TH1F("hpiplus_tm_cm_cosTheta","tm #pi^{+} cos#theta in CM frame",100,-1.,1.);

	//Piminus truthmatch histos
	hpiminus_tm_P = new TH1F("hpiminus_tm_P","tm #pi^{-} P",150,0.,3.);
	hpiminus_tm_Pt = new TH1F("hpiminus_tm_Pt","tm #pi^{-} Pt",150,0.,3.);
	hpiminus_tm_Theta = new TH1F("hpiminus_tm_Theta","tm #pi^{-} #theta",90,0.,180.);
	hpiminus_tm_cosTheta = new TH1F("hpiminus_tm_cosTheta","tm #pi^{-} cos#theta",100,-1.,1.);
	hpiminus_tm_cm_cosTheta = new TH1F("hpiminus_tm_cm_cosTheta","tm #pi^{-} cos#theta in CM frame",100,-1.,1.);

	//Proton truthmatch histos
	hproton_tm_P = new TH1F("hproton_tm_P","tm p P",150,0.,3.);
	hproton_tm_Pt = new TH1F("hproton_tm_Pt","tm p Pt",150,0.,3.);
	hproton_tm_Theta = new TH1F("hproton_tm_Theta","tm p #theta",90,0.,180.);
	hproton_tm_cosTheta = new TH1F("hproton_tm_cosTheta","tm p cos#theta",100,-1.,1.);
	hproton_tm_cm_cosTheta = new TH1F("hproton_tm_cm_cosTheta","tm p cos#theta in CM frame",100,-1.,1.);

	//AntiProton truthmatch histos
	hantiproton_tm_P = new TH1F("hantiproton_tm_P","tm p Pbar",250,0.,5.);
	hantiproton_tm_Pt = new TH1F("hantiproton_tm_Pt","tm pbar Pt",150,0.,3.);
	hantiproton_tm_Theta = new TH1F("hantiproton_tm_Theta","tm pbar #theta",90,0.,180.);
	hantiproton_tm_cosTheta = new TH1F("hantiproton_tm_cosTheta","tm pbar cos#theta",100,-1.,1.);
	hantiproton_tm_cm_cosTheta = new TH1F("hantiproton_tm_cm_cosTheta","tm pbar cos#theta in CM frame",100,-1.,1.);

	//Lambda truthmatch histos
	hlam0_tm_M_all = new TH1F("hlam0_tm_M_all","tm #Lambda_{0} mass",100,0,5);
	hlam0_tm_cosTheta_all = new TH1F("hlam0_tm_cosTheta","tm #Lambda_{0} cos#theta",100,-1.,1.);
	hlam0_tm_chi2_vtx  = new TH1F("hlam0_chi2_vtx", "#tm Lambda_{0} #chi^{2} vertex fit",100,0,10);
	hlam0_tm_prob_vtx  = new TH1F("hlam0_prob_vtx", "#tm Lambda_{0} Prob vertex fit",100,0,1);
	hlam0_tm_M_vtx = new TH1F("hlam0_M_vtx","#tm Lambda_{0} mass, with vertex fit",100,0,5);
	hlam0_tm_R_vtx = new TH1F("hlam0_R_vtx","#tm Lambda_{0} R, with vertex fit",75,0,15);
	hlam0_tm_Z_vtx = new TH1F("hlam0_Z_vtx","#tm Lambda_{0} Z, with vertex fit",90,-5,25);
	hlam0_tm_cm_cosTheta_all = new TH1F("hlam0_tm_cm_cosTheta","tm #Lambda_{0} cos#theta in CM frame",100,-1.,1.);

	//Lambdabar truthmatch histos
	hlam0bar_tm_M_all = new TH1F("hlam0bar_tm_M_all","tm #Lambda_{0} bar mass",100,0,5);
	hlam0bar_tm_cosTheta_all = new TH1F("hlam0bar_tm_cosTheta","tm #Lambda_{0} bar cos#theta",100,-1.,1.);
	hlam0bar_tm_chi2_vtx  = new TH1F("hlam0bar_chi2_vtx", "#tm Lambda_{0} bar #chi^{2} vertex fit",100,0,10);
	hlam0bar_tm_prob_vtx  = new TH1F("hlam0bar_prob_vtx", "#tm Lambda_{0} bar Prob vertex fit",100,0,1);
	hlam0bar_tm_M_vtx = new TH1F("hlam0bar_M_vtx","#tm Lambda_{0} bar mass, with vertex fit",100,0,5);
	hlam0bar_tm_R_vtx = new TH1F("hlam0bar_R_vtx","#tm Lambda_{0} bar R, with vertex fit",75,0,15);
	hlam0bar_tm_Z_vtx = new TH1F("hlam0bar_Z_vtx","#tm Lambda_{0} bar Z, with vertex fit",180,-5,55);
	hlam0bar_tm_cm_cosTheta_all = new TH1F("hlam0bar_tm_cm_cosTheta","tm #Lambda_{0} bar cos#theta in CM frame",100,-1.,1.);
	
	// *** --------------------- ***
	// *** True MC 1D histograms ***
	// *** --------------------- ***

	//Lambda
	htruelam0_cm_cosTheta = new TH1F("htruelam0_cm_cosTheta","McTruth #Lambda_{0} cos#theta in CM frame",100,-1.,1.);

	//Lambdabar
	htruelam0bar_cm_cosTheta = new TH1F("htruelam0bar_cm_cosTheta","McTruth #Lambda_{0}bar cos#theta in CM frame",100,-1.,1.);

	//Piplus
	htruepiplus_cosTheta = new TH1F("htruepiplus_cosTheta","McTruth #pi^{+} cos#theta",100,-1.,1.);

	//Piminus
	htruepiminus_cosTheta = new TH1F("htruepiminus_cosTheta","McTruth #pi^{-} cos#theta",100,-1.,1.);

	//Proton
	htrueproton_cosTheta = new TH1F("htrueproton_cosTheta","McTruth p cos#theta",100,-1.,1.);

	//Antiproton
	htrueantiproton_cosTheta = new TH1F("htrueantiproton_cosTheta","McTruth pbar cos#theta",100,-1.,1.);

	// *** ------------------------- ***
	// *** Create some 2D histograms ***
	// *** ------------------------- ***

	//Piplus truthmatch histos
	hpiplus_tm_PvsTheta = new TH2F("hpiplus_tm_PvsTheta","tm #pi^{+} P vs #theta",150,0.,3.,180,0.,180.);
	hpiplus_tm_PvsDP = new TH2F("hpiplus_tm_PvsDP","tm #pi^{+} P vs #Delta P", 150, 0.,3.,100,-1.,1.);
	hpiplus_tm_ThetavsDTheta = new TH2F("hpiplus_tm_ThetavsDTheta","tm #pi^{+} #theta vs #Delta#theta",180,0.,180.,100,-1.,1.);

	//Piminus truthmatch histos
	hpiminus_tm_PvsTheta = new TH2F("hpiminus_tm_PvsTheta","tm #pi^{-} P vs #theta",150,0.,3.,180,0.,180.);
	hpiminus_tm_PvsDP = new TH2F("hpiminus_tm_PvsDP","tm #pi^{-} P vs #Delta P", 150, 0.,3.,100,-1.,1.);
	hpiminus_tm_ThetavsDTheta = new TH2F("hpiminus_tm_ThetavsDTheta","tm #pi^{-} #theta vs #Delta#theta",180,0.,180.,100,-1.,1.);

	//Proton truthmatch histos
	hproton_tm_PvsTheta = new TH2F("hproton_tm_PvsTheta","tm p P vs #theta",150,0.,3.,180,0.,180.);
	hproton_tm_PvsDP = new TH2F("hproton_tm_PvsDP","tm p P vs #Delta P", 150, 0.,3.,100,-1.,1.);
	hproton_tm_ThetavsDTheta = new TH2F("hproton_tm_ThetavsDTheta","tm p #theta vs #Delta#theta",180,0.,180.,100,-1.,1.);

	//AntiProton truthmatch histos
	hantiproton_tm_PvsTheta = new TH2F("hantiproton_tm_PvsTheta","tm pbar P vs #theta",250,0.,5.,180,0.,180.);
	hantiproton_tm_PvsDP = new TH2F("hantiproton_tm_PvsDP","tm pbar P vs #Delta P", 250, 0.,5.,100,-1.,1.);
	hantiproton_tm_ThetavsDTheta = new TH2F("hantiproton_tm_ThetavsDTheta","tm pbar #theta vs #Delta#theta",180,0.,180.,100,-1.,1.);

	return kSUCCESS;
}//End PndLLbarAnaTask::Init()

// -------------------------------------------------------------------------
	
void PndLLbarAnaTask::SetParContainers() 
{
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndLLbarAnaTask::Exec(Option_t*)
{
	// some variables
	int i=0,j=0, k=0, l=0;
	
	// necessary to read the next event
	theAnalysis->GetEvent();
	
	if (!(++nevts%100)) cout << "evt "<<nevts<<endl;
	
	// *** ---------------------------- ***
	// *** Now the analysis stuff comes ***
	// *** ---------------------------- ***
		
	// *** RhoCandLists for the analysis
	RhoCandList p, pbar, piplus, piminus, lam0, lam0bar, truepiplus, truepiminus, truep, truepbar, lam0_tm, lam0bar_tm, ppbarsystem, mclist;

	Bool_t pfromlam, pbarfromlam, piplusfromlam, piminusfromlam;
	double pbarmom=1.64;
	double m0_p  = 0.938272;
	pfromlam = kFALSE;
	pbarfromlam = kFALSE;
	piplusfromlam = kFALSE;
	piminusfromlam = kFALSE;

	TLorentzVector targ(0,0,0,m0_p);
	TLorentzVector beam(0,0,pbarmom,sqrt(m0_p*m0_p + pbarmom*pbarmom));
	TLorentzVector init=targ+beam;
	TVector3 cmboost = -init.BoostVector();

	// Select with no PID info ('All'); type and mass are set
	theAnalysis->FillList(p,"ProtonAllPlus");
	theAnalysis->FillList(pbar,"ProtonAllMinus");
	theAnalysis->FillList(piplus,"PionAllPlus");
	theAnalysis->FillList(piminus,"PionAllMinus");
	theAnalysis->FillList(mclist,"McTruth");

	theAnalysis->FillList(truepiplus,"PionAllPlus");
	theAnalysis->FillList(truepiminus,"PionAllMinus");
	theAnalysis->FillList(truep,"ProtonAllPlus");
	theAnalysis->FillList(truepbar,"ProtonAllMinus");

	// *** Select RhoCandidates with MC truth match
	SelectTruePid(theAnalysis, truepiplus);
	SelectTruePid(theAnalysis, truepiminus);
	SelectTruePid(theAnalysis, truep);
	SelectTruePid(theAnalysis, truepbar);

	// *** combinatorics for lam0 -> proton pi-
	lam0.Combine(p,piminus);
	lam0bar.Combine(pbar,piplus);
	lam0.SetType(3122);
	lam0bar.SetType(-3122);
	ppbarsystem.Combine(lam0,lam0bar);

	RhoCandidate *mctrlam0, *mctrlam0bar, *mctrpiplus, *mctrpiminus, *mctrproton, *mctrantiproton;

	//Save true MC tracks
	mctrlam0 = mclist[2];
	mctrlam0bar = mclist[1];
	mctrproton = mclist[2]->Daughter(0);
	mctrantiproton = mclist[1]->Daughter(0);
	mctrpiplus = mclist[1]->Daughter(1);
	mctrpiminus = mclist[2]->Daughter(1);

	//Create true MC track 4-momentum
	TLorentzVector mctrlam04 = mctrlam0->P4();
	TLorentzVector mctrlam0bar4 = mctrlam0bar->P4();
	TLorentzVector mctrpiplus4 = mctrpiplus->P4();
	TLorentzVector mctrpiminus4 = mctrpiminus->P4();
	TLorentzVector mctrproton4 = mctrproton->P4();
	TLorentzVector mctrantiproton4 = mctrantiproton->P4();

	//Boost true mc tracks to cm frame
	mctrlam04.Boost(cmboost);
	mctrlam0bar4.Boost(cmboost);
	//mctrpiplus4.Boost(cmboost);
	//mctrpiminus4.Boost(cmboost);
	//mctrproton4.Boost(cmboost);
	//mctrantiproton4.Boost(cmboost);

	//Cm frame true mc histos
	htruelam0_cm_cosTheta->Fill(mctrlam04.CosTheta());
	htruelam0bar_cm_cosTheta->Fill(mctrlam0bar4.CosTheta());

	//lab frame true mc histos
	htruepiplus_cosTheta->Fill(mctrpiplus4.CosTheta());
	htruepiminus_cosTheta->Fill(mctrpiminus4.CosTheta());
	htrueproton_cosTheta->Fill(mctrproton4.CosTheta());
	htrueantiproton_cosTheta->Fill(mctrantiproton4.CosTheta());

	//Loop over all possible Lambda0
	for (j=0;j<lam0.GetLength();++j)
	{
		hlam0_M_all->Fill(lam0[j]->M());
	}

	//Loop over all possible Lambda0 bar
	for (j=0;j<lam0bar.GetLength();++j)
	{
		hlam0bar_M_all->Fill(lam0bar[j]->M());
	}

	//Loop over all possible ppbar (lam0+lam0bar) systems
	for (j=0;j<ppbarsystem.GetLength();++j)
	{
		Rho4CFitter fitter(ppbarsystem[j],init);	// instantiate the 4C fitter
		bool check = fitter.FitConserveMasses();	// perform the fit

		double chi2_4c = fitter.GetChi2();	// access the chi2 of the fit
		double prob_4c = fitter.GetProb();	// access probability of fit
		hchi2_4c->Fill(chi2_4c);
		hprob_4c->Fill(prob_4c);

		RhoCandidate *lam04cfit = ppbarsystem[j]->Daughter(0)->GetFit();	// access the fitted candidate
		RhoCandidate *lam0bar4cfit = ppbarsystem[j]->Daughter(1)->GetFit();	// access the fitted candidate

		if (chi2_4c<20 && check)	// when good enough, fill some histos
		{
			hlam0_M_4c->Fill(lam04cfit->M());
			hlam0bar_M_4c->Fill(lam0bar4cfit->M());
		}
	}

	// Combinatorics for lam0 -> proton pi- *** tm events only ***
	lam0_tm.Combine(p, piminus);
	lam0bar_tm.Combine(pbar, piplus);
	lam0_tm.SetType(3122);
	lam0bar_tm.SetType(-3122);
	SelectTruePid(theAnalysis, lam0_tm);
	SelectTruePid(theAnalysis, lam0bar_tm);

	//Lambda0 with truthmatch loop
	for (j=0;j<lam0_tm.GetLength();++j)
	{
		TLorentzVector lam04 = lam0_tm[j]->P4();
		TLorentzVector lam04_cm = lam04;
		lam04_cm.Boost(cmboost);
		hlam0_tm_cm_cosTheta_all->Fill(lam04_cm.CosTheta());
		hlam0_tm_M_all->Fill(lam0_tm[j]->M());
		hlam0_tm_cosTheta_all->Fill(lam04.CosTheta());

		RhoKinVtxFitter vtxfitter(lam0_tm[j]);	//Instantiate a vertex fitter
		vtxfitter.Fit();
		RhoCandidate *jfit = lam0_tm[j]->GetFit();
		double chi2_vtx = vtxfitter.GetChi2();	// access chi2 of fit
		double prob_vtx = vtxfitter.GetProb();	// access probability of fit
		hlam0_tm_chi2_vtx->Fill(chi2_vtx);
		hlam0_tm_prob_vtx->Fill(prob_vtx);
		TVector3 lam0_tm_XYZ = jfit->Pos();

		if ( prob_vtx > 0.01 )	// when good enough, fill some histos
		{
			hlam0_tm_M_vtx->Fill(jfit->M());
			hlam0_tm_R_vtx->Fill(lam0_tm_XYZ.Perp());
			hlam0_tm_Z_vtx->Fill(lam0_tm_XYZ.Z());
		}
	}

	//Lambda0 bar with truthmatch loop
	for (j=0;j<lam0bar_tm.GetLength();++j)
	{
		TLorentzVector lam0bar4 = lam0bar_tm[j]->P4();
		TLorentzVector lam0bar4_cm = lam0bar4;
		lam0bar4_cm.Boost(cmboost);
		hlam0bar_tm_cm_cosTheta_all->Fill(lam0bar4_cm.CosTheta());
		hlam0bar_tm_M_all->Fill(lam0bar_tm[j]->M());
		hlam0bar_tm_cosTheta_all->Fill(lam0bar4.CosTheta());

		RhoKinVtxFitter vtxfitter(lam0bar_tm[j]);        //Instantiate a vertex fitter
		vtxfitter.Fit();
		RhoCandidate *jfit = lam0bar_tm[j]->GetFit();
		double chi2_vtx = vtxfitter.GetChi2();  // access chi2 of fit
		double prob_vtx = vtxfitter.GetProb();  // access probability of fit
		hlam0bar_tm_chi2_vtx->Fill(chi2_vtx);
		hlam0bar_tm_prob_vtx->Fill(prob_vtx);
		TVector3 lam0bar_tm_XYZ = jfit->Pos();

		if ( prob_vtx > 0.01 )	// when good enough, fill some histos
		{
			hlam0bar_tm_M_vtx->Fill(jfit->M());
			hlam0bar_tm_R_vtx->Fill(lam0bar_tm_XYZ.Perp());
			hlam0bar_tm_Z_vtx->Fill(lam0bar_tm_XYZ.Z());
		}
	}

	//Pi+ with truthmatch loop
	for (j=0;j<truepiplus.GetLength();++j)
	{
		if((truepiplus[j]->GetMcTruth()->TheMother()->PdgCode())!=-3122) continue;	//Only Pi+ from Lambdabar
		piplusfromlam = kTRUE;
		PndRecoCandidate* recopiplus;	//Get Reco- and PID info
		PndPidCandidate* pidpiplus;
		TLorentzVector piplus4=truepiplus[j]->P4();	//Construct ROOT TLorentzVectors
		TLorentzVector mctruthpiplus4=truepiplus[j]->GetMcTruth()->P4();
		TLorentzVector piplus4cm = piplus4;
		piplus4cm.Boost(cmboost);	//Boost to initial p pbar CM system
		recopiplus=truepiplus[j]->GetRecoCandidate();
		pidpiplus=(PndPidCandidate*)truepiplus[j]->GetRecoCandidate();

		hpiplus_tm_P->Fill(piplus4.P());
		hpiplus_tm_Pt->Fill(piplus4.Pt());
		hpiplus_tm_Theta->Fill((piplus4.Theta())*180./TMath::Pi());
		hpiplus_tm_cosTheta->Fill(piplus4.CosTheta());
		hpiplus_tm_cm_cosTheta->Fill(piplus4cm.CosTheta());

		//2D histos
		hpiplus_tm_PvsTheta->Fill(piplus4.P(),piplus4.Theta()*180./TMath::Pi());
		hpiplus_tm_PvsDP->Fill(piplus4.P(),(piplus4.P()-mctruthpiplus4.P())/mctruthpiplus4.P());
		hpiplus_tm_ThetavsDTheta->Fill(piplus4.Theta()*180./TMath::Pi(),(piplus4.Theta()-mctruthpiplus4.Theta())/mctruthpiplus4.Theta());
	}

	//Pi- with truthmatch loop
	for (j=0;j<truepiminus.GetLength();++j)
	{
		if((truepiminus[j]->GetMcTruth()->TheMother()->PdgCode())!=3122) continue;	//Only Pi- from Lambda
		piminusfromlam = kTRUE;
		PndRecoCandidate* recopiminus;
		PndPidCandidate* pidpiminus;
		TLorentzVector piminus4=truepiminus[j]->P4();
		TLorentzVector mctruthpiminus4=truepiminus[j]->GetMcTruth()->P4();
		TLorentzVector piminus4cm = piminus4;
		piminus4cm.Boost(cmboost);
		recopiminus=truepiminus[j]->GetRecoCandidate();
		pidpiminus=(PndPidCandidate*)truepiminus[j]->GetRecoCandidate();

		//1D histos
		hpiminus_tm_P->Fill(piminus4.P());
		hpiminus_tm_Pt->Fill(piminus4.Pt());
		hpiminus_tm_Theta->Fill((piminus4.Theta())*180./TMath::Pi());
		hpiminus_tm_cosTheta->Fill(piminus4.CosTheta());
		hpiminus_tm_cm_cosTheta->Fill(piminus4cm.CosTheta());

		//2D histos
		hpiminus_tm_PvsTheta->Fill(piminus4.P(),piminus4.Theta()*180./TMath::Pi());
		hpiminus_tm_PvsDP->Fill(piminus4.P(),(piminus4.P()-mctruthpiminus4.P())/mctruthpiminus4.P());
		hpiminus_tm_ThetavsDTheta->Fill(piminus4.Theta()*180./TMath::Pi(),(piminus4.Theta()-mctruthpiminus4.Theta())/mctruthpiminus4.Theta());
	}

	//Proton with truthmatch loop
	for (j=0;j<truep.GetLength();++j)
	{
		if((truep[j]->GetMcTruth()->TheMother()->PdgCode())!=3122) continue;	//Only select P from Lambda
		pfromlam = kTRUE;
		PndRecoCandidate* recop;
		PndPidCandidate* pidp;
		TLorentzVector p4=truep[j]->P4();
		TLorentzVector mctruthp4=truep[j]->GetMcTruth()->P4();
		TLorentzVector p4cm = p4;
		p4cm.Boost(cmboost);
		recop=truep[j]->GetRecoCandidate();
		pidp=(PndPidCandidate*)truep[j]->GetRecoCandidate();

		//1D histos
		hproton_tm_P->Fill(p4.P());
		hproton_tm_Pt->Fill(p4.Pt());
		hproton_tm_Theta->Fill((p4.Theta())*180./TMath::Pi());
		hproton_tm_cosTheta->Fill(p4.CosTheta());
		hproton_tm_cm_cosTheta->Fill(p4cm.CosTheta());

		//2D histos
		hproton_tm_PvsTheta->Fill(p4.P(),p4.Theta()*180./TMath::Pi());
		hproton_tm_PvsDP->Fill(p4.P(),(p4.P()-mctruthp4.P())/mctruthp4.P());
		hproton_tm_ThetavsDTheta->Fill(p4.Theta()*180./TMath::Pi(),(p4.Theta()-mctruthp4.Theta())/mctruthp4.Theta());
	}

	//AntiProton with tm loop
	for (j=0;j<truepbar.GetLength();++j)
	{
		if((truepbar[j]->GetMcTruth()->TheMother()->PdgCode())!=-3122) continue;	//Only Pbar from Lambdabar
		pbarfromlam = kTRUE;
		PndRecoCandidate* recopbar;
		PndPidCandidate* pidpbar;
		TLorentzVector pbar4=truepbar[j]->P4();
		TLorentzVector mctruthpbar4=truepbar[j]->GetMcTruth()->P4();
		TLorentzVector pbar4cm = pbar4;
		pbar4cm.Boost(cmboost);
		recopbar=truepbar[j]->GetRecoCandidate();
		pidpbar=(PndPidCandidate*)truepbar[j]->GetRecoCandidate();

		//1D histos
		hantiproton_tm_P->Fill(pbar4.P());
		hantiproton_tm_Pt->Fill(pbar4.Pt());
		hantiproton_tm_Theta->Fill((pbar4.Theta())*180./TMath::Pi());
		hantiproton_tm_cosTheta->Fill(pbar4.CosTheta());
		hantiproton_tm_cm_cosTheta->Fill(pbar4cm.CosTheta());

		//2D histos
		hantiproton_tm_PvsTheta->Fill(pbar4.P(),pbar4.Theta()*180./TMath::Pi());
		hantiproton_tm_PvsDP->Fill(pbar4.P(),(pbar4.P()-mctruthpbar4.P())/mctruthpbar4.P());
		hantiproton_tm_ThetavsDTheta->Fill(pbar4.Theta()*180./TMath::Pi(),(pbar4.Theta()-mctruthpbar4.Theta())/mctruthpbar4.Theta());
	}

}	//End PndLLbarAnaTask::Exec()














void PndLLbarAnaTask::Finish()
{	

	hprob_4c->Write();
	hchi2_4c->Write();

	hlam0_M_all->Write();
	hlam0_M_4c->Write();

	hlam0bar_M_all->Write();
	hlam0bar_M_4c->Write();

	hpiplus_tm_P->Write();
	hpiplus_tm_Pt->Write();
	hpiplus_tm_Theta->Write();
	hpiplus_tm_cosTheta->Write();
	hpiplus_tm_cm_cosTheta->Write();

	//2D histos
	hpiplus_tm_PvsTheta->Write();
	hpiplus_tm_PvsDP->Write();
	hpiplus_tm_ThetavsDTheta->Write();

	hpiminus_tm_P->Write();
	hpiminus_tm_Pt->Write();
	hpiminus_tm_Theta->Write();
	hpiminus_tm_cosTheta->Write();
	hpiminus_tm_cm_cosTheta->Write();
	//2D histos
	hpiminus_tm_PvsTheta->Write();
	hpiminus_tm_PvsDP->Write();
	hpiminus_tm_ThetavsDTheta->Write();

	hproton_tm_P->Write();
	hproton_tm_Pt->Write();
	hproton_tm_Theta->Write();
	hproton_tm_cosTheta->Write();
	hproton_tm_cm_cosTheta->Write();
	//2D histos
	hproton_tm_PvsTheta->Write();
	hproton_tm_PvsDP->Write();
	hproton_tm_ThetavsDTheta->Write();

	hantiproton_tm_P->Write();
	hantiproton_tm_Pt->Write();
	hantiproton_tm_Theta->Write();
	hantiproton_tm_cosTheta->Write();
	hantiproton_tm_cm_cosTheta->Write();

	//2D histos
	hantiproton_tm_PvsTheta->Write();
	hantiproton_tm_PvsDP->Write();
	hantiproton_tm_ThetavsDTheta->Write();
	
	hlam0_tm_M_all->Write();
	hlam0_tm_cosTheta_all->Write();
	hlam0_tm_chi2_vtx->Write();
	hlam0_tm_prob_vtx->Write();
	hlam0_tm_M_vtx->Write();
	hlam0_tm_R_vtx->Write();
	hlam0_tm_Z_vtx->Write();
	hlam0_tm_cm_cosTheta_all->Write();
	//2D histos

	hlam0bar_tm_M_all->Write();
	hlam0bar_tm_cosTheta_all->Write();
	hlam0bar_tm_chi2_vtx->Write();
	hlam0bar_tm_prob_vtx->Write();
	hlam0bar_tm_M_vtx->Write();
	hlam0bar_tm_R_vtx->Write();
	hlam0bar_tm_Z_vtx->Write();
	hlam0bar_tm_cm_cosTheta_all->Write();
	//2D histos

	//True MC histos
	
	//Lambda
	htruelam0_cm_cosTheta->Write();
	//Lambdabar
	htruelam0bar_cm_cosTheta->Write();
	//Piminus
	htruepiminus_cosTheta->Write();
	//Piplus
	htruepiplus_cosTheta->Write();
	//Proton
	htrueproton_cosTheta->Write();
	//Antiproton
	htrueantiproton_cosTheta->Write();
		
}	//End PndLLbarAnaTask::Finish()

ClassImp(PndLLbarAnaTask)
