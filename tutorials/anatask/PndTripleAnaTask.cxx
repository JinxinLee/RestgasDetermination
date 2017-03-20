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
#include "PndTripleAnaTask.h"

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
#include "RhoEnergyParticleSelector.h"
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
PndTripleAnaTask::PndTripleAnaTask(double pbarmom, TString outname, int mode, TString pidalg) : 
  FairTask("Panda Tripple Analysis Task"),
  fEvtCount(0), fMode(mode), fVerbose(0), fOutName(outname), fPidAlg(pidalg)
{ 
	double mp=0.938272;
	fIni.SetXYZT(0,0,pbarmom, sqrt(pbarmom*pbarmom+mp*mp)+mp);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndTripleAnaTask::~PndTripleAnaTask() 
{ 
	delete fFile;
}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndTripleAnaTask::Init() 
{		
	// *** initialize PndAnalysis object
	fAnalysis = new PndAnalysis();
	
	// *******
	// ******* PREPARE/CREATE THE STUFF YOU NEED
	// *******

	fPdg = TDatabasePDG::Instance();
	
	// *** RhoTuple QA helper
	// *** QA tool for simple dumping of analysis results in RhoRuple
	// *** WIKI: https://panda-wiki.gsi.de/foswiki/bin/view/Computing/PandaRootAnalysisJuly13#PndRhoTupleQA
	fQA = new PndRhoTupleQA(fAnalysis,fIni.Pz());
	
	// *** Connect to the Online Filter Info
	fOnlineFilterInfo = ( TClonesArray* ) FairRootManager::Instance()->GetObject ( "OnlineFilterInfo" );	

	// ***
	// *** Prepare RhoTuple output  
	// ***
	
	TDirectory *dir = gDirectory;	
    fFile=new TFile(fOutName,"RECREATE");
	fFile->cd();

	// tuple to store mc truth
	nmc  = new RhoTuple("nmc",  "mctruth info");

	double m0_jpsi = fPdg->GetParticle("J/psi")->Mass();   // Get nominal PDG mass of the J/psi
	double m0_ds = fPdg->GetParticle("D_s+")->Mass();   // Get nominal PDG mass of the Ds
	double m0_pi0 = fPdg->GetParticle("pi0")->Mass();   // Get nominal PDG mass of the pi0

	gamSel = new RhoEnergyParticleSelector("gamsel"   ,10.+0.1,20.);
	
	switch (fMode) 
	{
	// ***
	// *** ppb -> J/psi pi+ pi-
	// ***
	case 0:
		// *** Mass selector for the jpsi cands
		mSel1 = new RhoMassParticleSelector("jpsi"   ,m0_jpsi,0.3);
		mSel2 = new RhoMassParticleSelector("jpsipre",m0_jpsi,1.0);
		
		// *** create some ntuples
		ntp1 = new RhoTuple("ntp1", "jpsi analysis");
		ntp2 = new RhoTuple("ntp2", "psi(2S) analysis");
		
		break;
	
	// ***
	// *** ppb -> Ds+ Ds(2317)- [Ds pi0]
	// ***
	case 1:
		// *** Mass selector for the Ds cands
		mSel1 = new RhoMassParticleSelector("ds"   ,m0_ds,0.2);
		mSel2 = new RhoMassParticleSelector("dspre",m0_ds,1.0);
		mSel4 = new RhoMassParticleSelector("ds0",  2.3178,0.3);
		
		mSel3 = new RhoMassParticleSelector("pi0",m0_pi0,0.05);
		
		// *** create some ntuples
		ntp1 = new RhoTuple("ntp1", "ds analysis");
		ntp2 = new RhoTuple("ntp2", "ds2317 analysis");
		ntp3 = new RhoTuple("ntp3", "ppb analysis");
		ntp4 = new RhoTuple("ntp4", "pi0 analysis");
	
		break;
		
	// ***
	// *** ppb -> pi0 pi0 pi0 (with some resonances)
	// ***
	case 2:
		// *** Mass selector for the pi0 cands
		mSel1 = new RhoMassParticleSelector("pi0",m0_pi0,0.05);

		// *** create some ntuples
		ntp1 = new RhoTuple("ntp1", "pi0 analysis");
		ntp2 = new RhoTuple("ntp2", "2part analysis");
		break;
	}
	
	// assign RhoTuples to outfile
	if (ntp1) ntp1->GetInternalTree()->SetDirectory(gDirectory);
	if (ntp2) ntp2->GetInternalTree()->SetDirectory(gDirectory);
	if (ntp3) ntp3->GetInternalTree()->SetDirectory(gDirectory);
	if (ntp4) ntp4->GetInternalTree()->SetDirectory(gDirectory);
	if (nmc)  nmc->GetInternalTree()->SetDirectory(gDirectory);

	// *** restore original gDirectory
	dir->cd();
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndTripleAnaTask::SetParContainers() 
{
  // *** Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndTripleAnaTask::Exec(Option_t*)
{
	// *** necessary to read the next event
	fAnalysis->GetEventInTask();
	
	// *** print event counter
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	
	// *******
	// ******* PUT ANALYSIS CODE HERE			
	// *******
	
	PndOnlineFilterInfo *stInfo = 0;
	if (fOnlineFilterInfo) stInfo = ( PndOnlineFilterInfo* ) fOnlineFilterInfo->At ( 0 );	
	
	// prepare event shape object
	RhoCandList all;
	fAnalysis->FillList(all, "All");
	PndEventShape evtShape(all,fIni,0.01,0.05);
	fEventShape = &evtShape;
	
	// write MC tuple
	nmc->Column("ev", (Int_t) fEvtCount);
	fQA->qaMcList(nmc);
	nmc->DumpData();
	
	// execute analysis 
	switch (fMode)
	{
	case 0: 
		JpsiAnalysis(); 
		break;
	case 1: 
		DsDs2317Analysis(); 
		break;
	case 2: 
		ThreePiAnalysis(); 
		break;
	}
}

void PndTripleAnaTask::JpsiAnalysis()
{
	// *** some variables
	int i=0,j=0;
	
	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, ppb;
	
	// *** Select with PID info pidalg and ('All'); type and mass are set 		
	fAnalysis->FillList(muplus,  "MuonAllPlus",  fPidAlg);
	fAnalysis->FillList(muminus, "MuonAllMinus", fPidAlg);
	fAnalysis->FillList(piplus,  "PionAllPlus",  fPidAlg);
	fAnalysis->FillList(piminus, "PionAllMinus", fPidAlg);
	
	// *** combinatorics for J/psi -> mu+ mu-
	jpsi.Combine(muplus, muminus, 443);
	jpsi.Select(mSel2);
	int njmct = fAnalysis->McTruthMatch(jpsi); // match the whole list to count #matches (should be only 1)
	
	// *** write ntuple for the jpsi reconstruction
	for (j=0;j<jpsi.GetLength();++j) 
	{
		PndKinVtxFitter vtxfit(jpsi[j]);
		//vtxfit.AddMassConstraint(fPdg->GetParticle("J/psi")->Mass());
		vtxfit.Fit();
		
		double chi2 = vtxfit.GetChi2();
		
		RhoCandidate *jfit=jpsi[j]->GetFit();
		
		// some general info about event (actually written for each candidate!)
		ntp1->Column("ev",		(Float_t) fEvtCount);
		ntp1->Column("cand",	(Float_t) j);
		ntp1->Column("ncand",   (Float_t) jpsi.GetLength());
		ntp1->Column("nmct",    (Float_t) njmct);
		
		// store info about initial 4-vector
		fQA->qaP4("beam", fIni, ntp1);
			
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		fQA->qaComp("j", jpsi[j], ntp1);
		fQA->qaCand("fj",jfit,    ntp1);
		ntp1->Column("fchi2", (Float_t) chi2);
		
		// dump info about event shapes
		fQA->qaEventShapeShort("es",fEventShape, ntp1);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = jpsi[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		fQA->qaP4("trj", lv, ntp1);

		ntp1->DumpData(); 
	}
		
	
	// *** some rough mass selection on J/psi
	jpsi.Select(mSel1);
	
	// *** combinatorics for psi(2S) -> J/psi pi+ pi-
	ppb.Combine(jpsi, piplus, piminus, 88880);
	int nppbmct = fAnalysis->McTruthMatch(ppb); // match the whole list to count #matches (should be only 1)
	
	// *** write ntuple for the psi(2S) reconstruction
	for (j=0;j<ppb.GetLength();++j) 
	{
		PndKinFitter kinfit(ppb[j]);
		kinfit.Add4MomConstraint(fIni);
		kinfit.Fit();
		
		double chi2 = kinfit.GetChi2();
				
		RhoCandidate *ppbfit=ppb[j]->GetFit();
		
		// some general info about event (actually written for each candidate!)
		ntp2->Column("ev",		(Float_t) fEvtCount);
		ntp2->Column("cand",	(Float_t) j);
		ntp2->Column("ncand",   (Float_t) ppb.GetLength());
		ntp2->Column("nmct",    (Float_t) nppbmct);

		// store info about initial 4-vector
		fQA->qaP4("beam", fIni, ntp2);
		
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		fQA->qaComp("ppb", ppb[j], ntp2);
		fQA->qaComp("fppb",ppbfit, ntp2);
		ntp2->Column("fchi2", (Float_t) chi2);

		// dump info about event shapes
		fQA->qaEventShapeShort("es",fEventShape, ntp2);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = ppb[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		fQA->qaP4("trpsi", lv, ntp2);
		
		ntp2->DumpData(); 
	}			
}

void PndTripleAnaTask::DsDs2317Analysis()
{
	// *** some variables
	int i=0,j=0;
	
	// *** RhoCandLists for the analysis
	RhoCandList kp, km, pip, pim, gam, dsp, dsm, ds, pi0, ds0p, ds0m, ds0, ppb;
	
	// *** Select with PID info pidalg and ('All'); type and mass are set 		
	fAnalysis->FillList(kp,  "KaonAllPlus",  fPidAlg);
	fAnalysis->FillList(km,  "KaonAllMinus", fPidAlg);
	fAnalysis->FillList(pip, "PionAllPlus",  fPidAlg);
	fAnalysis->FillList(pim, "PionAllMinus", fPidAlg);
	fAnalysis->FillList(gam, "Neutral", fPidAlg);
	
	gam.Select(gamSel);
	
	if (fVerbose) cout <<"kp:"<<kp.GetLength()<<" km:"<<km.GetLength()<<" pip:"<<pip.GetLength()<<" pim:"<<pim.GetLength()<<" g:"<<gam.GetLength()<<endl;
	
	dsp.Combine(kp, km, pip, 431);
	dsm.Combine(km, kp, pim, -431);
	
	dsp.Select(mSel1);
	dsm.Select(mSel1);
	
	ds.Cleanup();
	ds.Append(dsp);
	ds.Append(dsm);
	
	if (fVerbose) cout <<"Ds+:"<<dsp.GetLength()<<" Ds-:"<<dsm.GetLength()<<" Ds:"<<ds.GetLength()<<endl;
	
	int ndsmct = fAnalysis->McTruthMatch(ds); // match the whole list to count #matches (should be only 1)
		
	// *** write ntuple for the ds reconstruction
	for (j=0;j<ds.GetLength();++j) 
	{
		PndKinVtxFitter vtxfit(ds[j]);
		//vtxfit.AddMassConstraint(fPdg->GetParticle("D_s+")->Mass());
		vtxfit.Fit();
		
		double chi2 = vtxfit.GetChi2();
		
		RhoCandidate *dsfit=ds[j]->GetFit();
		
		// some general info about event (actually written for each candidate!)
		ntp1->Column("ev",		(Float_t) fEvtCount);
		ntp1->Column("cand",	(Float_t) j);
		ntp1->Column("ncand",   (Float_t) ds.GetLength());
		ntp1->Column("nmct",    (Float_t) ndsmct);
		
		// store info about initial 4-vector
		fQA->qaP4("beam", fIni, ntp1);
			
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		fQA->qaComp("ds", ds[j], ntp1);
		fQA->qaCand("fds",dsfit,    ntp1);
		ntp1->Column("fchi2", (Float_t) chi2);
		
		// dump info about event shapes
		fQA->qaEventShapeShort("es",fEventShape, ntp1);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = ds[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		fQA->qaP4("trds", lv, ntp1);
		
		ntp1->DumpData();	
	}
	
	// select ds mass
	dsp.Select(mSel2);
	dsm.Select(mSel2);

	pi0.Combine(gam, gam, 111);
	pi0.Select(mSel3);
	int npi0mct = fAnalysis->McTruthMatch(pi0);
	
	for (j=0;j<pi0.GetLength();++j)
	{
		ntp4->Column("ev",		(Float_t) fEvtCount);
		ntp4->Column("cand",	(Float_t) j);
		ntp4->Column("ncand",   (Float_t) pi0.GetLength());
		ntp4->Column("nmct",    (Float_t) npi0mct);
		fQA->qaPi0("pi0",pi0[j],ntp4);
		ntp4->DumpData();
	}
	
	ds0p.Combine(dsp, pi0, 10431);
	ds0m.Combine(dsm, pi0, -10431);
	
	ds0m.Select(mSel4);
	ds0p.Select(mSel4);
	
	ds0.Cleanup();
	ds0.Append(ds0p);
	ds0.Append(ds0m);
	
	if (fVerbose) cout <<"Pi0:"<<pi0.GetLength()<<" Ds0+:"<<ds0p.GetLength()<<" Ds0-:"<<ds0m.GetLength()<<" Ds0:"<<ds0.GetLength()<<endl;
	
	int nds0mct = fAnalysis->McTruthMatch(ds0); // match the whole list to count #matches (should be only 1)
	
	// *** write ntuple for the ds reconstruction
	for (j=0;j<ds0.GetLength();++j) 
	{
		PndKinFitter kinfit(ds0[j]);
		kinfit.AddMassConstraint(2.3178);
		kinfit.Fit();
		
		double chi2 = kinfit.GetChi2();
		
		RhoCandidate *ds0fit=ds0[j]->GetFit();
		
		// some general info about event (actually written for each candidate!)
		ntp2->Column("ev",		(Float_t) fEvtCount);
		ntp2->Column("cand",	(Float_t) j);
		ntp2->Column("ncand",   (Float_t) ds0.GetLength());
		ntp2->Column("nmct",    (Float_t) nds0mct);
		
		// store info about initial 4-vector
		fQA->qaP4("beam", fIni, ntp2);
			
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		fQA->qaComp("ds0", ds0[j], ntp2);
		fQA->qaCand("fds0",ds0fit,    ntp2);
		ntp2->Column("fchi2", (Float_t) chi2);
		
		// dump info about event shapes
		fQA->qaEventShapeShort("es",fEventShape, ntp2);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = ds0[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		fQA->qaP4("trds0", lv, ntp2);

		ntp2->DumpData();
	}
	
	ppb.Combine(dsp, ds0m, 88880);
	ppb.CombineAndAppend(dsm, ds0p, 88880);
	
	if (fVerbose) cout <<"ppb:"<<ppb.GetLength()<<endl<<endl;
	
	int nppbmct = fAnalysis->McTruthMatch(ppb); // match the whole list to count #matches (should be only 1)

	// *** write ntuple for the psi(2S) reconstruction
	for (j=0;j<ppb.GetLength();++j) 
	{
		PndKinFitter kinfit(ppb[j]);
		kinfit.Add4MomConstraint(fIni);
		kinfit.Fit();
		
		double chi2 = kinfit.GetChi2();
		
		RhoCandidate *ppbfit=ppb[j]->GetFit();
		
		// some general info about event (actually written for each candidate!)
		ntp3->Column("ev",		(Float_t) fEvtCount);
		ntp3->Column("cand",	(Float_t) j);
		ntp3->Column("ncand",   (Float_t) ppb.GetLength());
		ntp3->Column("nmct",    (Float_t) nppbmct);

		// store info about initial 4-vector
		fQA->qaP4("beam", fIni, ntp3);
		
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		fQA->qaComp("ppb", ppb[j], ntp3);
		fQA->qaComp("fppb",ppbfit, ntp3);
		ntp3->Column("fchi2", (Float_t) chi2);

		// dump info about event shapes
		fQA->qaEventShapeShort("es",fEventShape, ntp3);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = ppb[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		fQA->qaP4("trpsi", lv, ntp3);
		
		ntp3->DumpData(); 
	}			
}

void PndTripleAnaTask::ThreePiAnalysis()
{
	// *** some variables
	int i=0,j=0;
	
	// *** RhoCandLists for the analysis
	RhoCandList gam, pi0, pip, pim, comb1, ppb;
	
	fAnalysis->FillList(pip, "PionAllPlus",  fPidAlg);
	fAnalysis->FillList(pim, "PionAllMinus", fPidAlg);
	fAnalysis->FillList(gam, "Neutral", fPidAlg);
	
	gam.Select(gamSel);
	
	pi0.Combine(gam, gam, 111);
	pi0.Select(mSel1);
	
	int npi0mct = fAnalysis->McTruthMatch(pi0);
	
	for (j=0;j<pi0.GetLength();++j)
	{
		ntp1->Column("ev",		(Float_t) fEvtCount);
		ntp1->Column("cand",	(Float_t) j);
		ntp1->Column("ncand",   (Float_t) pi0.GetLength());
		ntp1->Column("nmct",    (Float_t) npi0mct);
		fQA->qaPi0("pi0",pi0[j],ntp1);
		ntp1->DumpData();
	}
	
	ppb.Combine(pip, pim, pi0, 88880);
	
	int nppbmct = fAnalysis->McTruthMatch(ppb); // match the whole list to count #matches (should be only 1)
	
	// *** write ntuple for the psi(2S) reconstruction
	for (j=0;j<ppb.GetLength();++j) 
	{
		PndKinFitter kinfit(ppb[j]);
		kinfit.Add4MomConstraint(fIni);
		kinfit.Fit();
		
		double chi2 = kinfit.GetChi2();
				
		RhoCandidate *ppbfit=ppb[j]->GetFit();
		
		// some general info about event (actually written for each candidate!)
		ntp2->Column("ev",		(Float_t) fEvtCount);
		ntp2->Column("cand",	(Float_t) j);
		ntp2->Column("ncand",   (Float_t) ppb.GetLength());
		ntp2->Column("nmct",    (Float_t) nppbmct);

		// store info about initial 4-vector
		fQA->qaP4("beam", fIni, ntp2);
		
		// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
		fQA->qaComp("ppb", ppb[j], ntp2);
		fQA->qaComp("fppb",ppbfit, ntp2);
		ntp2->Column("fchi2", (Float_t) chi2);

		// dump info about event shapes
		fQA->qaEventShapeShort("es",fEventShape, ntp2);
		
		// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
		RhoCandidate *truth = ppb[j]->GetMcTruth();
		TLorentzVector lv;
		if (truth) lv = truth->P4();
		fQA->qaP4("trpsi", lv, ntp2);
		
		ntp2->DumpData(); 
	}			
}


void PndTripleAnaTask::Finish()
{
	
	// *******
	// ******* STORE YOUR HISTOS AND TUPLES
	// *******
	
	fFile->Write();
	fFile->Close();	

}

ClassImp(PndTripleAnaTask)
