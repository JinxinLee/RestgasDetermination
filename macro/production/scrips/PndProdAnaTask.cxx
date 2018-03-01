// ************************************************************************
//
// Analysis Example Task
//
// for the Simulation Production (macro/prod/scripts) and Analysis, see
//
// https://panda-wiki.gsi.de/foswiki/bin/view/Computing/PandaRootSimulationKronos
// https://panda-wiki.gsi.de/foswiki/bin/view/Computing/PandaRootRhoTutorial
//
// K.Goetzen 6/2016
// ************************************************************************


// The header file
#include "PndProdAnaTask.h"

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

// Analysis headers
#include "PndAnalysis.h"
#include "Rho4CFitter.h"
#include "RhoKinVtxFitter.h"
#include "RhoKinFitter.h"
#include "RhoVtxPoca.h"
#include "PndRhoTupleQA.h"


using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndProdAnaTask::PndProdAnaTask(int mode, TString pidAlgo) :
  FairTask("Panda Tutorial Analysis Task")
{
	fMode = mode;
	fPidAlgo = pidAlgo;
	if (fPidAlgo=="") fPidAlgo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts;PidAlgoSciT;PidAlgoRich";
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndProdAnaTask::~PndProdAnaTask() { }
// -------------------------------------------------------------------------


// ----- Method to select true PID candidates
int PndProdAnaTask::SelectTruePid(PndAnalysis *ana, RhoCandList &l)
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
InitStatus PndProdAnaTask::Init()
{
	// initialize analysis object
	fAnalysis = new PndAnalysis();

	// reset the event counter
	fEvtCount = 0;

	// *** Save current gDirectory
	TDirectory *dir = gDirectory;
	FairRootManager::Instance()->GetOutFile()->cd();

	ntp  = new RhoTuple("ntp",  "Prod Example Analysis: D0->K pi");
	ntp->GetInternalTree()->SetDirectory(gDirectory);

	// *** restore original gDirectory
	dir->cd();

	// *** the lorentz vector of the initial psi(2S)
	fIni.SetXYZT(0, 0, 6.231552, 7.240065);

	return kSUCCESS;
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndProdAnaTask::Exec(Option_t*)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;

	// necessary to read the next event
	fAnalysis->GetEventInTask();

	if (!(++fEvtCount%100)) cout << "[PndProdAnaTask] evt "<<fEvtCount<<endl;

	// *** QA tool for simple dumping of analysis results in RhoRuple
	// *** WIKI: https://panda-wiki.gsi.de/foswiki/bin/view/Computing/PandaRootAnalysisJuly13#PndRhoTupleQA
	PndRhoTupleQA qa(fAnalysis,fIni.P());

	// *** RhoCandLists for the analysis
	RhoCandList kp, km, pip, pim, D0, D0bar;

	// *** Select with no PID info ('All'); type and mass are set
	fAnalysis->FillList(kp,  "KaonAllPlus",   fPidAlgo);
	fAnalysis->FillList(km,  "MuonAllMinus",  fPidAlgo);
	fAnalysis->FillList(pip, "PionAllPlus",   fPidAlgo);
	fAnalysis->FillList(pim, "PionAllMinus",  fPidAlgo);

	D0.Combine(km, pip, 421);
	D0bar.Combine(kp, pim, -421);

	D0.Append(D0bar);

	for (j=0; j<D0.GetLength(); ++j)
	{
		Float_t mmiss = (fIni-(D0[j]->P4())).M();
		Float_t msum  = D0[j]->M() + mmiss;

		// dump out information
		ntp->Column("ev",		(Int_t) fEvtCount);
		ntp->Column("cand",	    (Int_t) j);
		ntp->Column("ncand",    (Int_t) D0.GetLength());
		ntp->Column("mode",     (Int_t) fMode);
		ntp->Column("mmiss",	(Float_t) mmiss);
		ntp->Column("msum",	    (Float_t) msum);

		// store beam info
		qa.qaP4("beam", fIni, ntp);

		// store information about composite candidate tree recursively (see analysis/AnalysisTools/PndRhoTupleQA)
		qa.qaComp("x", D0[j], ntp);

		// store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = D0[j]->GetMcTruth();
		TLorentzVector lv(0,0,0,0);
		if (truth) lv = truth->P4();
		qa.qaP4("trx", lv, ntp);

		// do a vertex fit
		RhoKinVtxFitter vtxfitter(D0[j]);          // *** instantiate the vertex fitter; input is the object to be fitted
		vtxfitter.Fit();                           // *** perform fit

		RhoCandidate *cfit = D0[j]->GetFit();      // *** get the fitted candidate
		qa.qaVtx("fvxx",cfit,ntp);
		qa.qaP4("fvxx", cfit->P4(), ntp);
		double chi2_vtx = vtxfitter.GetChi2();     // *** and the chi^2 of the fit
		ntp->Column("chi2vx", (Float_t) chi2_vtx);

		// really write information to tree
		ntp->DumpData();
	}
}


void PndProdAnaTask::Finish()
{
	ntp->GetInternalTree()->Write();
}

ClassImp(PndProdAnaTask)
