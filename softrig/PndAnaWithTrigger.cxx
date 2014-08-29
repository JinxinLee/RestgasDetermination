// ************************************************************************
//
// Template for an Analysis Task, 
// 
// for the December 2013 CM Tutorial 
//
// For further info see also
//
// http://panda-wiki.gsi.de/cgi-bin/viewauth/Computing/PandaRootRhoTutorial
// http://panda-wiki.gsi.de/cgi-bin/view/Computing/PandaRootAnalysisJuly13
//
// K.Goetzen 11/2013
//
// ************************************************************************


// The header file
#include "PndAnaWithTrigger.h"

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
#include "TDatabasePDG.h"

// RHO headers
#include "RhoCandidate.h"
#include "RhoHistogram/RhoTuple.h"
#include "RhoFactory.h"
#include "RhoMassParticleSelector.h"
#include "RhoTuple.h"

// Analysis headers
#include "PndAnalysis.h"
#include "Pnd4CFitter.h"
#include "PndKinVtxFitter.h"
#include "PndKinFitter.h"
#include "PndVtxPoca.h"
#include "PndRhoTupleQA.h"
#include "PndEventShape.h"

// Soft Trigger Header
#include "PndOnlineFilterInfo.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndAnaWithTrigger::PndAnaWithTrigger(double pbarmom, TString outname) :
  FairTask("Panda Analysis Task using Trigger info") 
{ 
	double mp=0.938272;
	fIni.SetXYZT(0,0,pbarmom, sqrt(pbarmom*pbarmom+mp*mp)+mp);

	fOutName = outname;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndAnaWithTrigger::~PndAnaWithTrigger() 
{ 
	delete fFile;
}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndAnaWithTrigger::Init() 
{		
	// *** initialize PndAnalysis object
	fAnalysis = new PndAnalysis();
	fAnalysis->McMatchAllowPhotos();
	
	// *** reset the event counter
	fEvtCount = 0;

	// *******
	// ******* PREPARE/CREATE THE STUFF YOU NEED
	// *******
	
	fPdg = TDatabasePDG::Instance();

	// *** Mass selector for the jpsi cands
	double m0_jpsi = fPdg->GetParticle("J/psi")->Mass();   // Get nominal PDG mass of the J/psi
	jpsiMassSel    = new RhoMassParticleSelector("jpsi"   ,m0_jpsi,0.5);
	jpsiPreMassSel = new RhoMassParticleSelector("jpsipre",m0_jpsi,2.0);
	
	// ***
	// *** Prepare RhoTuple output  
	// ***
	TDirectory *dir = gDirectory;	
    fFile=new TFile(fOutName,"RECREATE");
	fFile->cd();
	
	// *** create some ntuples
	ntp1 = new RhoTuple("ntp1", "jpsi analysis");
	ntp2 = new RhoTuple("ntp2", "psi(2S) analysis");
	nmc  = new RhoTuple("nmc",  "mctruth info");

	// assign RhoTuples to outfile
	if (ntp1) ntp1->GetInternalTree()->SetDirectory(gDirectory);
	if (ntp2) ntp2->GetInternalTree()->SetDirectory(gDirectory);
	if (nmc)  nmc->GetInternalTree()->SetDirectory(gDirectory);

	// *** restore original gDirectory
	dir->cd();
	
	// *** Connect to the Online Filter Info
	fOnlineFilterInfo = ( TClonesArray* ) FairRootManager::Instance()->GetObject ( "OnlineFilterInfo" );	
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndAnaWithTrigger::Exec(Option_t* opt)
{
	// *** some variables
	int i=0,j=0;
	
	// *** necessary to read the next event
	fAnalysis->GetEventInTask();
	
	// *** print event counter
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	
	// *******
	// ******* PUT ANALYSIS CODE HERE			
	// *******
	
	// *** fetch trigger info
	PndOnlineFilterInfo *stInfo = 0;
	if (fOnlineFilterInfo) stInfo = ( PndOnlineFilterInfo* ) fOnlineFilterInfo->At ( 0 );	
	
	TString pidalg = "PidChargedProbability";
	
	// *** QA tool for simple dumping of analysis results in RhoRuple
	// *** WIKI: https://panda-wiki.gsi.de/foswiki/bin/view/Computing/PandaRootAnalysisJuly13#PndRhoTupleQA
	PndRhoTupleQA qa(fAnalysis,fIni.P());

	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, psi2s, all, mclist;
	
	
	// *** Select with PID info pidalg and ('All'); type and mass are set 		
	fAnalysis->FillList(muplus,  "MuonAllPlus",  pidalg);
	fAnalysis->FillList(muminus, "MuonAllMinus", pidalg);
	fAnalysis->FillList(piplus,  "PionAllPlus",  pidalg);
	fAnalysis->FillList(piminus, "PionAllMinus", pidalg);

	
	// *** Setup event shape object
	fAnalysis->FillList(all,   "All", pidalg);
	PndEventShape evsh(all, fIni, 0.05, 0.1);	
	
	
	// *** store MC info in ntuple
	fAnalysis->FillList(mclist, "McTruth");
	
	nmc->Column("ev", (Int_t) fEvtCount);
	qa.qaMcList("",   mclist, nmc);
	nmc->DumpData();
	
	
	// *** combinatorics for J/psi -> mu+ mu-
	jpsi.Combine(muplus, muminus);
	jpsi.Select(jpsiPreMassSel);
	jpsi.SetType(443);
	int njmct = fAnalysis->McTruthMatch(jpsi); // match the whole list to count #matches (should be only 1)
			
	// *** write ntuple for the jpsi reconstruction
	for (j=0;j<jpsi.GetLength();++j) 
	{
		// some general info about event (actually written for each candidate!)
		ntp1->Column("ev",		(Float_t) i);
		ntp1->Column("cand",	(Float_t) j);
		ntp1->Column("ncand",   (Float_t) jpsi.GetLength());
		ntp1->Column("nmct",    (Float_t) njmct);
		
		// *** store info from trigger
		if (stInfo)
		{
			ntp1->Column("sttrig",   (Int_t) stInfo->Tagged());       // event triggered
			ntp1->Column("stntot",   (Int_t) stInfo->GetNTagTotal()); // total number of triggered candidates from all active lines
			ntp1->Column("stn200",   (Int_t) stInfo->GetNTag(200));   // number of triggered candidates from J/psi->e+ e- line
			ntp1->Column("stn201",   (Int_t) stInfo->GetNTag(201));   // number of triggered candidates from J/psi->mu+ mu- line
		}
	
		// store info about initial 4-vector
		qa.qaP4("beam", fIni, ntp1);
			
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		qa.qaComp("j", jpsi[j], ntp1);
		
		// dump info about event shapes
		qa.qaEventShapeShort("es",&evsh, ntp1);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = jpsi[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		qa.qaP4("trj", lv, ntp1);
		
		ntp1->DumpData();
	}
	
	
	// *** some rough mass selection on J/psi
	jpsi.Select(jpsiMassSel);
	
	// *** combinatorics for psi(2S) -> J/psi pi+ pi-
	psi2s.Combine(jpsi, piplus, piminus);
	psi2s.SetType(88880);
	int npsimct = fAnalysis->McTruthMatch(psi2s); // match the whole list to count #matches (should be only 1)

	// *** write ntuple for the psi(2S) reconstruction
	for (j=0;j<psi2s.GetLength();++j) 
	{
		// some general info about event (actually written for each candidate!)
		ntp2->Column("ev",		(Float_t) i);
		ntp2->Column("cand",	(Float_t) j);
		ntp2->Column("ncand",   (Float_t) psi2s.GetLength());
		ntp2->Column("nmct",    (Float_t) npsimct);

		// *** store info from trigger
		if (stInfo)
		{
			ntp2->Column("sttrig",   (Int_t) stInfo->Tagged());       // event triggered
			ntp2->Column("stntot",   (Int_t) stInfo->GetNTagTotal()); // total number of triggered candidates from all active lines
			ntp2->Column("stn200",   (Int_t) stInfo->GetNTag(200));   // number of triggered candidates from J/psi->e+ e- line
			ntp2->Column("stn201",   (Int_t) stInfo->GetNTag(201));   // number of triggered candidates from J/psi->mu+ mu- line
		}
		
		PndKinFitter kinfit(psi2s[j]);
		kinfit.Add4MomConstraint(fIni);
		kinfit.Fit();
		
		RhoCandidate *psifit=psi2s[j]->GetFit();
		// store info about initial 4-vector
		qa.qaP4("beam", fIni, ntp2);
		
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		qa.qaComp("psi", psi2s[j], ntp2);
		qa.qaComp("fpsi",psifit, ntp2);
		ntp2->Column("fchi2", (Float_t) kinfit.GetChi2());

		// dump info about event shapes
		qa.qaEventShapeShort("es",&evsh, ntp2);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = psi2s[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		qa.qaP4("trpsi", lv, ntp2);
		
		ntp2->DumpData();
	}			
	
}


void PndAnaWithTrigger::Finish()
{
	
	// *******
	// ******* STORE YOUR HISTOS AND TUPLES
	// *******
	
	fFile->Write();
	fFile->Close();	

}

ClassImp(PndAnaWithTrigger)
