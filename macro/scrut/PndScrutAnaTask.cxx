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
#include "PndScrutAnaTask.h"

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
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndScrutAnaTask::PndScrutAnaTask(double pbarmom) :
  FairTask("Panda Scrutiny Analysis Task") 
{ 
	double mp=0.938272;
	fIni.SetXYZT(0,0,pbarmom, sqrt(pbarmom*pbarmom+mp*mp)+mp);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndScrutAnaTask::~PndScrutAnaTask() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndScrutAnaTask::Init() 
{		
	// *** initialize PndAnalysis object
	fAnalysis = new PndAnalysis();
	
	// *** reset the event counter
	fEvtCount = 0;

	// *******
	// ******* PREPARE/CREATE THE STUFF YOU NEED
	// *******
	
	fPdg = TDatabasePDG::Instance();
	
	// *** create some ntuples
	ntp1 = new RhoTuple("ntp1", "jpsi analysis");
	ntp2 = new RhoTuple("ntp2", "psi(2S) analysis");
	nmc  = new RhoTuple("nmc",  "mctruth info");
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndScrutAnaTask::SetParContainers() 
{
  // *** Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndScrutAnaTask::Exec(Option_t* opt)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	
	// *** necessary to read the next event
	fAnalysis->GetEvent();
	
	// *** print event counter
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	
	// *******
	// ******* PUT ANALYSIS CODE HERE
	// *******
	
	TString pidalg = "PidChargedProbability";
	
	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, psi2s, all, mclist;
	
	// *** Mass selector for the jpsi cands
	double m0_jpsi = fPdg->GetParticle("J/psi")->Mass();   // Get nominal PDG mass of the J/psi
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",m0_jpsi,1.0);
	
	fAnalysis->FillList(mclist, "McTruth");
	fAnalysis->FillList(all,  "All", pidalg);
	
	// *** Select with PID info pidalg and ('All'); type and mass are set 		
	fAnalysis->FillList(muplus,  "MuonAllPlus", pidalg);
	fAnalysis->FillList(muminus, "MuonAllMinus", pidalg);
	fAnalysis->FillList(piplus,  "PionAllPlus", pidalg);
	fAnalysis->FillList(piminus, "PionAllMinus", pidalg);

	// *** Setup event shape object
	PndEventShape evsh(all, fIni, 0.05, 0.1);	
	PndRhoTupleQA qa(fAnalysis,fIni.P());
	
	// *** get MC list
	for (j=0;j<mclist.GetLength();++j)
	{
		nmc->Column("ev",	 (Float_t) i);
		nmc->Column("part",	 (Float_t) j);
		nmc->Column("npart", (Float_t) mclist.GetLength());
		
		qa.qaCand("mc",mclist[j], nmc);
		nmc->DumpData();
	}
	
							
	
	// *** combinatorics for J/psi -> mu+ mu-
	jpsi.Combine(muplus, muminus);		
	jpsi.SetType(443);
	int njmct = fAnalysis->McTruthMatch(jpsi);
			
	for (j=0;j<jpsi.GetLength();++j) 
	{
		ntp1->Column("ev",		(Float_t) i);
		ntp1->Column("cand",	(Float_t) j);
		ntp1->Column("ncand",   (Float_t) jpsi.GetLength());
		ntp1->Column("nmct",    (Float_t) njmct);
		
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		qa.qaComp("j", jpsi[j], ntp1);
		// dump info about event shapes
		qa.qaEventShapeShort("es",&evsh, ntp1);
		
		RhoCandidate *truth = jpsi[j]->GetMcTruth();
		
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		qa.qaP4("trj", lv, ntp1);
		
		ntp1->DumpData();
	}
	
	// *** some rough mass selection
	jpsi.Select(jpsiMassSel);
	
	
	// *** combinatorics for psi(2S) -> J/psi pi+ pi-
	psi2s.Combine(jpsi, piplus, piminus);
	psi2s.SetType(100443);
	int npsimct = fAnalysis->McTruthMatch(psi2s);

	for (j=0;j<psi2s.GetLength();++j) 
	{
		ntp2->Column("ev",		(Float_t) i);
		ntp2->Column("cand",	(Float_t) j);
		ntp2->Column("ncand",   (Float_t) psi2s.GetLength());
		ntp2->Column("nmct",    (Float_t) npsimct);
		
		qa.qaComp("psi", psi2s[j], ntp2);
		qa.qaEventShapeShort("es",&evsh, ntp2);
		
		RhoCandidate *truth = psi2s[j]->GetMcTruth();
		
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		qa.qaP4("trpsi", lv, ntp1);
		
		ntp2->DumpData();
	}			
	
}


void PndScrutAnaTask::Finish()
{
	
	// *******
	// ******* STORE YOUR HISTOS AND TUPLES
	// *******
	
	ntp1->GetInternalTree()->Write();
	ntp2->GetInternalTree()->Write();
	nmc->GetInternalTree()->Write();

}

ClassImp(PndScrutAnaTask)
