// ************************************************************************
//
//  Analysis Task using PndSimpleCombiner 
// 
// ************************************************************************
//
// Parameters: 
// - anadecay     : Decay specification, e.g. "phi -> K+ K-; D_s+ -> phi pi+" (automatic charged conjugation; particle used have to be defined beforehand)
//                  Keyword 'nocc' at end of decay definition suppresses automatic charged conjugation
//                  This string is handed over to PndSimpleCombiner
//
// - params       : configuration parameters, e.g. "fit4c:qamc". The string contains also parameters handled by PndSimpleCombiner; those handled by this task are:
//   - fit4c[<x]  : perform 4C fit on last resonance; optional argument [<x] cuts on chi2 < x
//   - fitvtx[<x] : perform vertex fit on all resonances when possible (at least two daughters); optional argument [<x] cuts on chi2 < x
//   - qamc       : stored MC information
//   - qaevtshape : store event shape information
//   - !ntpX      : skips dump of TTree for X-th resonance; e.g. in "phi -> K+ K-; D_s+ -> phi pi+", '!ntp0' would skip dump of TTree for phi->KK
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
PndSimpleCombinerTask::PndSimpleCombinerTask(TString anadecay, TString anaparms, double p, int run, int mode) :
  FairTask("PndSimpleCombinerTask"), fVerbose(0), fEvtCount(0), fRun(run), fMode(mode), fRunMult(10000),
  fAnaDecay(anadecay), fAnaParms(anaparms), fNntp(0), 
  fPidAlgo("PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts;PidAlgoSciT;PidAlgoRich;PidAlgoFtof"),
  fQaMC(false), fQaEventShape(false),  fQaRecoInfo(0), fQaEvShapeNtp(false), fFit4C(false), 
  fBest4C(false), fFitVtx(false), fFit4CChiCut(1e15), fFitVtxChiCut(1e8), 
  fAnalysis(0), fSimpleCombiner(0), fNodump(0), nmc(0), nevt(0)
{ 
	fIni.SetXYZT(0,0,0,0);
	double mp = 0.938272;
	if (p>0.0001) fIni.SetXYZT(0,0,p, sqrt(p*p+mp*mp)+mp);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSimpleCombinerTask::~PndSimpleCombinerTask() 
{ 
	if (fSimpleCombiner) delete fSimpleCombiner;
	delete fAnalysis;
}
// -------------------------------------------------------------------------
void PndSimpleCombinerTask::InitParms()
{
	fAnaParms.ReplaceAll(" ","");
	
	StringList pars;
	SplitString(fAnaParms,":",pars);
	
	// loop over all parameters
	for (unsigned int i=0;i<pars.size();++i)
	{
		// is this a parameter handled by task or by combiner? If yes, it will be deleted later from the list
		bool taskparm = false;
		
		// simple parameter flag
		if (pars[i]=="qamc")                { fQaMC 		= true; taskparm = true; } // write mc information
		if (pars[i]=="qaevtshape")          { fQaEventShape = true; taskparm = true; } // write event shape info
		if (pars[i]=="qaevs")               { fQaEventShape = true; taskparm = true; } // write event shape info
		if (pars[i]=="qarec")               { fQaRecoInfo   = 1   ; taskparm = true; } // write reco info summary (EMC, STT, ...)
		if (pars[i]=="qarecfull")           { fQaRecoInfo   = 2   ; taskparm = true; } // write full reco info

		if (pars[i]=="nevt")                { fQaEvShapeNtp = true; taskparm = true; } // write event shape info to extra ntuple
		if (pars[i].Contains("fit4c"))      { fFit4C		= true; taskparm = true; } // perform 4c fit for last particle (usually pbarpSystemX)
		if (pars[i].Contains("fit4cbest"))  { fBest4C		= true; taskparm = true; } // only store best 4C fitted candidate
		if (pars[i].Contains("fitvtx"))     { fFitVtx		= true; taskparm = true; } // perform vertex fit if possible (not cascaded)
		
		if (pars[i].BeginsWith("!ntp"))                                                // !ntpX avoids dump of ntuple of X-th resonance to save disc space (for e.g. subresonances of decay tree)
		{
			int ntpnum = ((TString)(pars[i](4,100))).Atoi();
			if (ntpnum>=0 && ntpnum<32) fNodump |= (1<<ntpnum);                        // this is a bit marker; if i-th bit set, dump of ntpi is skipped
			taskparm = true;
		}
			
		// parameter pair for chi2 cut from fit?
		if (pars[i].Contains("<"))
		{
			// extract the value
			double cut = TString((pars[i])(pars[i].Index("<")+1,1000)).Atof();
			if (cut<=0) { cout <<"[PndSimpleCombinerTask] **** ERROR : Invalid parameter setting '"<<pars[i]<<"'"<<endl;continue; }
			
			// set the cut for the corresponding fitter
			if (pars[i].Contains("fit4c"))  fFit4CChiCut=cut;
			if (pars[i].Contains("fitvtx")) fFitVtxChiCut=cut;	
		}
		
		// delete parameter from list for PndSimpleCombiner
		if (taskparm) fAnaParms.ReplaceAll(pars[i],"");
	}	
}

// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndSimpleCombinerTask::Init() 
{		
	InitParms();
	
	// *** initialize PndAnalysis object and SimpleCombiner
	fAnalysis         = new PndAnalysis();
	
	if (fAnaDecay!="")
	{
		fSimpleCombiner   = new PndSimpleCombiner(fAnalysis, fAnaDecay, fAnaParms, fIni.M());
	
		fSimpleCombiner->SetVerbose(fVerbose);
		fSimpleCombiner->Print();
	}
	
	// *******
	// ******* PREPARE/CREATE THE STUFF YOU NEED
	// *******
	
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
		RhoTuple *n = 0;
		if (!(fNodump & (1<<i))) // do we write this ntuple?
		{				  
			n = new RhoTuple(TString::Format("ntp%d",i), toks[i]);
			n->GetInternalTree()->SetDirectory(gDirectory);
		}
		vntp.push_back(n);
		
		TString pname = toks[i](0,toks[i].Index("->"));
		pname.ReplaceAll(" ","");
		if (TDatabasePDG::Instance()->GetParticle(pname)) {vmpdg.push_back(TDatabasePDG::Instance()->GetParticle(pname)->PdgCode());}
	}
	
	// *** create MC ntuple
	if (fQaMC) nmc  = new RhoTuple("nmc",  "mctruth info");
	if (nmc)   nmc->GetInternalTree()->SetDirectory(gDirectory);
	
	// *** create EventShape ntuple
	if (fQaEvShapeNtp)  nevt  = new RhoTuple("nevt", "event shape info");
	if (nevt)           nevt->GetInternalTree()->SetDirectory(gDirectory);

	// *** restore original gDirectory
	dir->cd();
	
	// *** Connect to the Online Filter Info
	fOnlineFilterInfo = ( TClonesArray* ) FairRootManager::Instance()->GetObject ( "OnlineFilterInfo" );	
	
	
		// ****** Print out some info from PndSimpleCombinerTask
	cout <<"\n------------------------------------------"<<endl<<"[PndSimpleCombinerTask] **** Configuration"<<endl<<"------------------------------------------"<<endl;
	cout <<"Fitting       : ";
	if (fFit4C)  cout <<"4-C ( chi^2 < "<<fFit4CChiCut<<")";
	if (fFitVtx) cout <<"  Vertex ( chi^2 < "<<fFitVtxChiCut<<")";
	cout <<endl;
	
	cout <<"Ntuple output : ";
	if (fQaMC)         cout <<"nmc  ";
	if (fQaEvShapeNtp) cout <<"nevt  ";
	for (int i=0;i<fNntp;++i) if (!(fNodump & (1<<i))) cout <<"ntp"<<i<<"("<<TDatabasePDG::Instance()->GetParticle(vmpdg[i])->GetName()<<")  ";
	cout <<"\n------------------------------------------\n\n"<<endl;

	
	return kSUCCESS;
}

// -------------------------------------------------------------------------

int PndSimpleCombinerTask::CountChargedDaughters(RhoCandidate *c)
{
	int nd = 0;
	
	for (int i=0;i<c->NDaughters();++i)
	{
		if (fabs(c->Daughter(i)->Charge())>1e-6) nd++;
	}
	
	return nd;
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
void PndSimpleCombinerTask::Exec(Option_t*)
{
	// *** some variables
	int i=0,j=0;
	
	// *** necessary to read the next event
	fAnalysis->GetEventInTask();
	
	// *** print event counter
	if (!(++fEvtCount%100)) cout << "[PndSimpleCombinerTask] evt "<<fEvtCount<<endl;
	
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

	if (fQaEventShape || fQaEvShapeNtp) 
	{
		fAnalysis->FillList(all,   "All", fPidAlgo);
		evsh = new PndEventShape(all, fIni, 0.05, 0.1);
	}
	
	// *** Store event shape info in ntuple
	if (fQaEvShapeNtp)
	{
		nevt->Column("ev",		(Int_t) fEvtCount);
		nevt->Column("run",     (Int_t) fRun);
		nevt->Column("uid",     (Int_t) fRun*fRunMult+fEvtCount);
		nevt->Column("mode",    (Int_t) fMode);
		
		qa.qaP4("beam", fIni, nevt);
		qa.qaEventShape("es", evsh, nevt);
		
		nevt->DumpData();
	}
	
	// *****
	// *** combinatorics 
	// *****
	
	if (fSimpleCombiner)
	{
	  fSimpleCombiner->Combine();

	  // ntuple dump
	  for (i=0;i<fNntp;++i)
	  {
		  if (fNodump & (1<<i)) continue; // if dump of ntuple 'ntpi' is skipped continue

		  int pdg  = vmpdg[i];
		  int apdg = 0;
		  if (TDatabasePDG::Instance()->GetParticle(pdg)->AntiParticle()) apdg = TDatabasePDG::Instance()->GetParticle(pdg)->AntiParticle()->PdgCode();

		  // check whether there is an own ntuple connected to the anti-particle pdg; if yes, reset apdg
		  for (j=0; j<fNntp; ++j) if (vmpdg[j]==apdg) {apdg=0; j=fNntp+1;}

		  // merge list from particles and anti-particles
		  fSimpleCombiner->GetList(l1, pdg);
		  if (apdg!=0 && fSimpleCombiner->GetList(l2, apdg)) l1.Append(l2);

		  //RhoMassParticleSelector msel("msel",TDatabasePDG::Instance()->GetParticle(pdg)->Mass(),0.2);
		  //l1.Select(&msel);

		  // number of charged daughters for vtx fit
		  int ncdau = -1;

		  // determine the best chi2 from 4C fit in case we only want to store the best candidate
		  double best4cChi2 = 1e10;

		  for (j=0;j<l1.GetLength();++j) 
		  {
			  if (ncdau<0) ncdau = CountChargedDaughters(l1[j]);

			  Float_t mmiss = (fIni-(l1[j]->P4())).M();
			  Float_t msum  = l1[j]->M() + mmiss;

			  // in case we do 4C fit, we need to do determine first, whether the candidate is the best one
			  // flag whether the fit is accepted
			  bool fitaccept = true;

			  // for the last list we perform a 4C fit
			  if (fFit4C && i==fNntp-1)
			  {
				  PndKinFitter fit4c(l1[j]);
				  fit4c.Add4MomConstraint(fIni);
				  fit4c.Fit();

				  double chi2_4c = fit4c.GetChi2();   

				  if (chi2_4c>=fFit4CChiCut) fitaccept = false;
				  if (chi2_4c>best4cChi2 || !fitaccept) continue;

				  best4cChi2 = chi2_4c;

				  RhoCandidate *cfit   = l1[j]->GetFit();

				  vntp[i]->Column("chi24c", (Float_t) chi2_4c);
				  qa.qaP4("f4cx", cfit->P4(), vntp[i]);

				  for (int k=0;k<cfit->NDaughters();++k)
				  {
					  RhoCandidate *d0fit = cfit->Daughter(k);
					  qa.qaP4(TString::Format("f4cxd%d",k),d0fit->P4(),vntp[i]);

					  for (int k2=0;k2<d0fit->NDaughters();++k2)
					  {
						  RhoCandidate *ddfit = d0fit->Daughter(k2);
						  qa.qaP4(TString::Format("f4cxd%dd%d",k,k2),ddfit->P4(),vntp[i]);
					  }
				  }
			  }

			  vntp[i]->Column("ev",		(Int_t) fEvtCount);
			  vntp[i]->Column("cand",	    (Int_t) j);
			  vntp[i]->Column("ncand",    (Int_t) l1.GetLength());
			  vntp[i]->Column("run",      (Int_t) fRun);
			  vntp[i]->Column("uid",      (Int_t) fRun*fRunMult+fEvtCount);
			  vntp[i]->Column("mode",     (Int_t) fMode);

			  vntp[i]->Column("mmiss",	(Float_t) mmiss);
			  vntp[i]->Column("msum",	    (Float_t) msum);

			  qa.qaP4("beam", fIni, vntp[i]);

			  // store information about composite candidate tree recursively (see analysis/AnalysisTools/PndRhoTupleQA)
			  qa.qaComp("x", l1[j], vntp[i]);
			  // also optional for reco info
			  if (fQaRecoInfo==1) qa.qaRecoShortTree("x", l1[j], vntp[i]);
			  else if (fQaRecoInfo==2) qa.qaRecoFullTree("x", l1[j], vntp[i]);

			  // store info about event shapes
			  if (fQaEventShape) qa.qaEventShapeShort("es",evsh, vntp[i]);

			  // *** store info from trigger
			  if (stInfo)
			  {
				  vntp[i]->Column("trig",    (Int_t) stInfo->Tagged() );       // event triggered
				  vntp[i]->Column("ntrig",   (Int_t) stInfo->GetNTagTotal());  // total number of triggered candidates from all active lines
			  }

			  // store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
			  RhoCandidate *truth = l1[j]->GetMcTruth();		
			  TLorentzVector lv;
			  if (truth) lv = truth->P4();
			  qa.qaP4("trx", lv, vntp[i]);


			  // shall we do a vertex fit?
			  if (fFitVtx && ncdau>1)
			  {
				  PndKinVtxFitter vtxfitter(l1[j]);        // *** instantiate the vertex fitter; input is the object to be fitted      
				  vtxfitter.Fit();                           // *** perform fit

				  RhoCandidate *cfit = l1[j]->GetFit();      // *** get the fitted candidate

				  qa.qaVtx("fvxx",cfit,vntp[i]);
				  qa.qaP4("fvxx", cfit->P4(), vntp[i]);
				  double chi2_vtx = vtxfitter.GetChi2();     // *** and the chi^2 of the fit
				  vntp[i]->Column("chi2vx", (Float_t) chi2_vtx);

				  if (chi2_vtx>=fFitVtxChiCut) fitaccept = false;
			  }	

			  if (fitaccept && ((i<fNntp-1) || !fBest4C )) vntp[i]->DumpData();
		  }

		  if (fBest4C && i==fNntp-1 && best4cChi2<1e10) { vntp[i]->DumpData();}
	  }
	}
	
	if (evsh) delete evsh;
}


void PndSimpleCombinerTask::Finish()
{
	if (nmc)  nmc->GetInternalTree()->Write();
	if (nevt) nevt->GetInternalTree()->Write();
	
	for (int i=0;i<fNntp;++i) if (!(fNodump & (1<<i))) vntp[i]->GetInternalTree()->Write();
}

ClassImp(PndSimpleCombinerTask)
