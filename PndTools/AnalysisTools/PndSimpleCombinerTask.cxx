// ************************************************************************
//
// Analysis Task using PndSimpleCombiner 
// 
// K.Goetzen 1/2015
//
// ************************************************************************


// The header file
#include "PndSimpleCombinerTask.h"

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
#include "PndSimpleCombiner.h"

// Soft Trigger Header
#include "PndOnlineFilterInfo.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndSimpleCombinerTask::PndSimpleCombinerTask(TString anadecay, TString anaparms) :
  FairTask("PndSimpleCombinerTask"),
  fAnaDecay(anadecay), fAnaParms(anaparms), fNntp(0), 
  fPidAlgo("PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts"),
  fQaMC(false), fQaEventShape(false), fFit4C(false), fFitVtx(false), nmc(0)
{ 
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSimpleCombinerTask::~PndSimpleCombinerTask() 
{ 
	delete fSimpleCombiner;
	delete fAnalysis;
}
// -------------------------------------------------------------------------
void PndSimpleCombinerTask::InitParms()
{
	fAnaParms.ReplaceAll(" ","");
	if (fAnaParms.Contains("fit4c"))      { fFit4C        = true; fAnaParms.ReplaceAll("fit4c",""); }
	if (fAnaParms.Contains("fitvtx"))     { fFitVtx       = true; fAnaParms.ReplaceAll("fitvtx",""); }
	if (fAnaParms.Contains("qamc"))       { fQaMC         = true; fAnaParms.ReplaceAll("qamc",""); }
	if (fAnaParms.Contains("qaevtshape")) { fQaEventShape = true; fAnaParms.ReplaceAll("qaevtshape",""); }
}

// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndSimpleCombinerTask::Init() 
{		
	fIni.SetXYZT(0,0,0,0);
	InitParms();
	
	// *** reset the event counter
	fEvtCount = 0;
	
	// *** initialize PndAnalysis object and SimpleCombiner
	fAnalysis         = new PndAnalysis();
	fSimpleCombiner   = new PndSimpleCombiner(fAnalysis, fAnaDecay, fAnaParms);
	
	fSimpleCombiner->Print();

	// *******
	// ******* PREPARE/CREATE THE STUFF YOU NEED
	// *******
	
	fPdg = TDatabasePDG::Instance();

	// ***
	// *** Prepare RhoTuple output  
	// ***

	// *** Save current gDirectory
	TDirectory *dir = gDirectory;
	FairRootManager::Instance()->GetOutFile()->cd();
	
	std::vector<TString> toks;
	fNntp = SplitString(fAnaDecay,";",toks);
	
	// *** create some ntuples
	for (int i=0;i<fNntp;++i)
	{
		RhoTuple *n = new RhoTuple(TString::Format("ntp%d",i), toks[i]);
		n->GetInternalTree()->SetDirectory(gDirectory);
		vntp.push_back(n);
		
		TString pname = toks[i](0,toks[i].Index("->"));
		pname.ReplaceAll(" ","");
		if (fPdg->GetParticle(pname)) {vmpdg.push_back(fPdg->GetParticle(pname)->PdgCode());}
	}
	
	// *** create MC ntuples
	if (fQaMC) nmc  = new RhoTuple("nmc",  "mctruth info");
	if (nmc)  nmc->GetInternalTree()->SetDirectory(gDirectory);

	// *** restore original gDirectory
	dir->cd();
	
	// *** Connect to the Online Filter Info
	fOnlineFilterInfo = ( TClonesArray* ) FairRootManager::Instance()->GetObject ( "OnlineFilterInfo" );	
	
	return kSUCCESS;
}


// -------------------------------------------------------------------------

int PndSimpleCombinerTask::SplitString(TString s, TString delim, StringList &toks)
{
	toks.clear();
	
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	
	for (int i=0;i<N;++i) 
	{
		TString st = ((TObjString*)tok->At(i))->String();
		st.ReplaceAll("\t","");
		st = st.Strip(TString::kBoth);
		if (st != "") toks.push_back(st);
	}
	
	return toks.size();
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSimpleCombinerTask::Exec(Option_t* opt)
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
	
	PndOnlineFilterInfo *stInfo = 0;
	if (fOnlineFilterInfo) stInfo = ( PndOnlineFilterInfo* ) fOnlineFilterInfo->At ( 0 );	
	
	// determine pbarmom if not done so far
	if (fIni.T()<1e-6)
	{
		RhoCandList mclist;
		fAnalysis->FillList(mclist, "McTruth");
		if (mclist.GetLength()>0)
		{
			double p  = mclist[0]->P3().Pz();
			double mp = 0.938272;
			fIni.SetXYZT(0,0,p, sqrt(p*p+mp*mp)+mp);
		}
	}
	
	// *** QA tool for simple dumping of analysis results in RhoRuple
	// *** WIKI: https://panda-wiki.gsi.de/foswiki/bin/view/Computing/PandaRootAnalysisJuly13#PndRhoTupleQA
	PndRhoTupleQA qa(fAnalysis,fIni.P());

	// *** RhoCandLists for the analysis
	RhoCandList l1, l2, all;
	
	// *** store MC info in ntuple
	if (nmc)
	{
		nmc->Column("ev", (Int_t) fEvtCount);
		qa.qaMcList(nmc);
		nmc->DumpData();
	}
	
	// *** Setup event shape object
	PndEventShape *evsh = 0;

	if (fQaEventShape) 
	{
		fAnalysis->FillList(all,   "All", fPidAlgo);
		evsh = new PndEventShape(all, fIni, 0.05, 0.1);
	}
	
	// *****
	// *** combinatorics 
	// *****
	
	fSimpleCombiner->Combine();
	
	
	// ntuple dump
	for (i=0;i<fNntp;++i)
	{
		int pdg  = vmpdg[i];
		int apdg = 0;
		if (fPdg->GetParticle(pdg)->AntiParticle()) apdg = fPdg->GetParticle(pdg)->AntiParticle()->PdgCode();
		
		// merge list from particles and anti-particles
		fSimpleCombiner->GetList(l1, pdg);
		if (fSimpleCombiner->GetList(l2, apdg)) l1.Append(l2);

		//RhoMassParticleSelector msel("msel",fPdg->GetParticle(pdg)->Mass(),0.2);
		//l1.Select(&msel);
		
		for (j=0;j<l1.GetLength();++j) 
		{
			Float_t mmiss = (fIni-(l1[j]->P4())).M();

			vntp[i]->Column("ev",		(Int_t) fEvtCount);
			vntp[i]->Column("cand",	    (Int_t) j);
			vntp[i]->Column("ncand",    (Int_t) l1.GetLength());
			vntp[i]->Column("mmiss",	(Float_t) mmiss);
			
			qa.qaP4("beam", fIni, vntp[i]);
			
			// store information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
			qa.qaComp("x", l1[j], vntp[i]);
			
			// store info about event shapes
			if (fQaEventShape) qa.qaEventShapeShort("es",evsh, vntp[i]);
		
			// store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
			RhoCandidate *truth = l1[j]->GetMcTruth();		
			TLorentzVector lv;
			if (truth) lv = truth->P4();
			qa.qaP4("trx", lv, vntp[i]);
			
			// for the last list we perform a 4C fit
			if (fFit4C && i==fNntp-1)
			{
/*				Pnd4CFitter fit4c(l1[j],fIni);
				fit4c.FitConserveMasses();*/
				PndKinFitter fit4c(l1[j]);
				fit4c.Add4MomConstraint(fIni);
				fit4c.Fit();
				
				double chi2_4c = fit4c.GetChi2();   
				RhoCandidate *cfit   = l1[j]->GetFit();
				
				vntp[i]->Column("chi4c", (Float_t) chi2_4c);
				qa.qaP4("fx", cfit->P4(), vntp[i]);
				
				for (int k=0;k<cfit->NDaughters();++k)
				{
					RhoCandidate *d0fit = cfit->Daughter(k);
					qa.qaP4(TString::Format("fxd%d",k),d0fit->P4(),vntp[i]);
				}
			}
			
			vntp[i]->DumpData();
		}
	}
	
	delete evsh;
}


void PndSimpleCombinerTask::Finish()
{
	if (nmc) nmc->GetInternalTree()->Write();
	
	for (int i=0;i<fNntp;++i) vntp[i]->GetInternalTree()->Write();
}

ClassImp(PndSimpleCombinerTask)
