// ******************************************************
// Psi' -> J/psi (-> e+ e-) pi+ pi- Analysis Example Task
//
// K.Goetzen 7/2012
// *******************************************************

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
//#include "PndTpcLheGFTrack.h"

#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStopwatch.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include <string>
#include <iostream>

// RHO stuff
#include "RhoCandidate.h"
#include "RhoHistogram/RhoTuple.h"
#include "RhoFactory.h"

// Analysis Tools
#include "PndSoftTriggerTask.h"
#include "PndAnalysis.h"

// Fitters
#include "Pnd4CFitter.h"
#include "PndKinVtxFitter.h"
#include "PndKinFitter.h"
#include "PndVtxPoca.h"
		
		
using std::cout;
using std::endl;

// *** constants for particles

const double		 	fDMass     = 1.8693;      // *** center of mass cut window
const double		 	fDMassCut  = 0.25;      // *** center of mass cut window

const double		 	fD0Mass    = 1.8645;      // *** center of mass cut window
const double		 	fD0MassCut = 0.25;      // *** center of mass cut window

const double		 	fJMass     = 3.09687;        // *** center of mass cut window
const double		 	fJMassCut  = 0.7;        // *** center of mass cut window

const double		 	fDsMass    = 1.9686;        // *** center of mass cut window
const double		 	fDsMassCut = 0.2;        // *** center of mass cut window
	
const double		 	fPhiMass    = 1.0195;        // *** center of mass cut window
const double		 	fPhiMassCut = 0.2;        // *** center of mass cut window

const double		 	fLamcMass    = 2.2849;        // *** center of mass cut window
const double		 	fLamcMassCut = 0.2;        // *** center of mass cut window


		
// -----   Default constructor   -------------------------------------------
PndSoftTriggerTask::PndSoftTriggerTask() :
  FairTask("Panda Analysis Task") { 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSoftTriggerTask::~PndSoftTriggerTask() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndSoftTriggerTask::Init() 
{	
	//cout << " Inside the Init function****" << endl;
		
	
	// initialize analysis object
	theAnalysis=new PndAnalysis();
		
	// Create and register output array
/*	hjpsimass = new TH1F("hjpsimass","jpsi cands",100,0.0,4.5);
	hpsimass = new TH1F("hpsimass","psi cands",100,0.,4.5);*/
	
	double max = 5.5;
	
	h_mom= new TH1F("h_mom","Momenta",200,0,5);
	h_momc= new TH1F("h_momc","Momenta charged",200,0,5);
	h_momn= new TH1F("h_momn","Momenta neutral",200,0,5);
	h_mult= new TH1F("h_mult","Multiplicity",50,-0.5,49.5);
	h_multc= new TH1F("h_multc","Multiplicity charged",30,-0.5,29.5);
	h_multn= new TH1F("h_multn","Multiplicity neutral",50,-0.5,49.5);
	h_multrem= new TH1F("h_multrem","Removed",50,-0.5,49.5);
	
	// J/psi histos
	h_jpsi=new TH1F("h_jpsi","J/psi",200,0,max);
	h_jpsit=new TH1F("h_jpsit","J/psi true",200,0,max);
	h_jpsisel=new TH1F("h_jpsisel","J/psi selected",200,0,max);
	ConfigureHistos(h_jpsi,h_jpsit,h_jpsisel);
	
	// D0 histos
	h_d0=new TH1F("h_d0","D0",200,0,max);
	h_d0t=new TH1F("h_d0t","D0 true",200,0,max);
	h_d0sel=new TH1F("h_d0sel","D0 selected",200,0,max);
	ConfigureHistos(h_d0 ,h_d0t ,h_d0sel );
	
	h_d0_pocx=new TH1F("h_d0_pocx","D0 poca X",100,-1,1);
	h_d0_pocy=new TH1F("h_d0_pocy","D0 poca Y",100,-1,1);
	h_d0_pocz=new TH1F("h_d0_pocz","D0 poca Z",100,-1,1);
	
	h_d0_pocxm=new TH1F("h_d0_pocxm","D0 poca X match",100,-1,1);
	h_d0_pocym=new TH1F("h_d0_pocym","D0 poca Y match",100,-1,1);
	h_d0_poczm=new TH1F("h_d0_poczm","D0 poca Z match",100,-1,1);
	
	h_d0_l = new TH1F("h_d0_l","flight length",100,0,1);
	h_d0_lm = new TH1F("h_d0_lm","flight length match",100,0,1);
	
	h_d0_pocxm->SetLineColor(2);
	h_d0_pocym->SetLineColor(2);
	h_d0_poczm->SetLineColor(2);
	h_d0_lm->SetLineColor(2);	
	
	// D+ histos
	h_dpm=new TH1F("h_dpm","D#pm",200,0,max);
	h_dpmt=new TH1F("h_dpmt","D#pm true",200,0,max);
	h_dpmsel=new TH1F("h_dpmsel","D#pm selected",200,0,max);
	ConfigureHistos(h_dpm ,h_dpmt ,h_dpmsel );
	
	// Ds histos
	h_ds=new TH1F("h_ds","Ds",200,0,max);
	h_dst=new TH1F("h_dst","Ds true",200,0,max);
	h_dssel=new TH1F("h_dssel","Ds selected",200,0,max);
	ConfigureHistos( h_ds, h_dst, h_dssel);
	
	// Phi histos
	h_phi=new TH1F("h_phi","Phi",200,0,max);
	h_phit=new TH1F("h_phit","Phi true",200,0,max);
	h_phisel=new TH1F("h_phisel","Phi selected",200,0,max);
	ConfigureHistos( h_phi,h_phit , h_phisel);
	
	// Lam_c histos
	h_lamc=new TH1F("h_lamc","Lam_c",200,0,max);
	h_lamct=new TH1F("h_lamct","Lam_c true",200,0,max);
	h_lamcsel=new TH1F("h_lamcsel","Lam_c selected",200,0,max);
	ConfigureHistos(h_lamc , h_lamct, h_lamcsel);
	
	
	
	ntp=new RhoTuple("ntp","the J/psi ntuple");
	ntp2=new RhoTuple("ntp2","the psi(2S) ntuple");
	
	// **** mass selector and McTruthMatcher
	//
  
	evcount=0;
 	
	chmax=0;
	neutmax=0;
	allmax=0;
	
	cntsel=0;
	
	cntjpsi=0;
	cntd0=0;
	cntdpm=0;
	cntds=0;
	cntphi=0;
	cntlamc=0;
	
	cout << "-I- PndSoftTriggerTask: Intialization successfull" << endl;
	
  	timer=new TStopwatch();
  	timer->Start();

	return kSUCCESS;
}

void PndSoftTriggerTask::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSoftTriggerTask::Exec(Option_t*) 
{
	Int_t j=0;

	theAnalysis->GetEvent();
	
	if (!(++evcount%100)) 
	{
		cout <<"evt "<<evcount;
		timer->Stop();
		cout <<"  t="<< timer->CpuTime();		
		timer->Start();
        cout <<"  Cand Watermark = "<<RhoFactory::Instance()->GetCandidateWatermark();
		cout <<" allmax:"<<chmax;
		cout <<" chmax:"<<chmax;
		cout <<" neutmax:"<<neutmax;
		cout <<" mcmax:"<<mcmax;
		cout <<endl;
	}
	
	
	// **** create all the particle lists we'll need for rebuilding the decay tree
	//
	//RhoCandList eplus, eminus, piplus, piminus, jpsi, psi, mctrk;   
	 
	RhoCandList all, chrg, neut;	
    RhoCandList ep, em, mup, mum, pip, pim, kp, km, pp, pm;
	RhoCandList Jpsi, D0, D0b, Dpm, Dm, Ds, Dsb, Lamc, Lamcb, Phi;
		
	// *** Select with no PID info ('All'); type and mass are set 		
	theAnalysis->FillList(all ,   "All");
	theAnalysis->FillList(chrg,   "Charged");
	theAnalysis->FillList(neut,  "Neutral");
	
	int nch = chrg.GetLength();
	int nneut = neut.GetLength();
	int nall = all.GetLength();
	
	if (nch>chmax) chmax=nch;
	if (nneut>neutmax) neutmax=nneut;
	if (nall>allmax) allmax=nall;
	
	
	// ************ preliminary FIX to remove double tracks ...	
	RemoveDoubles(chrg);
	// ************ preliminary FIX to remove double tracks ...
	
	double pidcut=0.1;
	
	SelectPid(0,  -11,1,chrg,ep,pidcut);		
	SelectPid(0,   11,-1,chrg,em,pidcut);		
	
	SelectPid(1,  -13,1,chrg,mup,pidcut);		
	SelectPid(1,   13,-1,chrg,mum,pidcut);		
	
	SelectPid(2,  211,1,chrg,pip,pidcut);		
	SelectPid(2, -211,-1,chrg,pim,pidcut);		
	
	SelectPid(3,  321,1,chrg,kp,pidcut);		
	SelectPid(3, -321,-1,chrg,km,pidcut);		
	
	SelectPid(4, 2212,1,chrg,pp,pidcut);		
	SelectPid(4,-2212,-1,chrg,pm,pidcut);		

	// fill multiplicities
	h_mult->Fill(all.GetLength());
	h_multc->Fill(chrg.GetLength());
	h_multn->Fill(neut.GetLength());
	
	// fill momentum histos
	for (j=0;j<all.GetLength();++j) h_mom->Fill(all[j]->P());
	for (j=0;j<chrg.GetLength();++j) h_momc->Fill(chrg[j]->P());
	for (j=0;j<neut.GetLength();++j) h_momn->Fill(neut[j]->P());
	
	// *** J/psi->l+ l- cominatorics 
	Jpsi.Combine(ep, em);
	Jpsi.CombineAndAppend(mup, mum);
	Jpsi.SetType("J/psi");
	
	// *** D0 -> K- pi+ cominatorics 
	D0.Combine(km, pip); D0.SetType("D0");
	D0b.Combine(kp,pim); D0b.SetType("anti-D0");
	D0.Append(D0b);
				
	// *** D+ -> K- pi+ pi+ cominatorics 
	Dpm.Combine(km, pip, pip); Dpm.SetType("D+");
	Dm.Combine(kp,pim,pim); Dm.SetType("D-");
	Dpm.Append(Dm);
	
	// *** Ds+ -> K+ K- pi+ cominatorics 
	Ds.Combine(kp, km, pip); Ds.SetType("D_s+");
	Dsb.Combine(kp, km, pim); Dsb.SetType("D_s-");
	Ds.Append(Dsb);
	
	// *** phi -> K+ K- cominatorics 
	Phi.Combine(km, kp);
	
	// *** Lambda_c -> p K- pi+ cominatorics 
	Lamc.Combine(pp, km, pip); Lamc.SetType("Lambda_c+");
	Lamcb.Combine(pm, kp, pim); Lamcb.SetType("anti-Lambda_c-");
	Lamc.Append(Lamcb);
	
	// how many combinations are selected?
	int comb	   =0;
	int foundjpsi  =0;
	int foundd0    =0;
	int founddpm   =0;
	int foundds    =0;
	int foundphi   =0;
	int foundlamc  =0;
	
	
	// ----------------------------------------------------------------------
	// ********* J/psi
	for (j=0;j<Jpsi.GetLength();++j) 
	{
		double m=Jpsi[j]->M();
		
		h_jpsi->Fill(m);
		
		if (theAnalysis->McTruthMatch(Jpsi.Get(j))) h_jpsit->Fill(m);
		
		if (fabs(m-fJMass)<fJMassCut) 
		{
			h_jpsisel->Fill(m);
			foundjpsi++;	
			comb++;
		}
	}
	
	
	// ----------------------------------------------------------------------
	// ********* D0
	for (j=0;j<D0.GetLength();++j) 
	{
		double m=D0[j]->M();
		
		h_d0->Fill(m);
		
		if (theAnalysis->McTruthMatch(D0.Get(j))) h_d0t->Fill(m);
		
		if (fabs(m-fD0Mass)<fD0MassCut ) 
		{
			h_d0sel->Fill(m);
			foundd0++;
			comb++;
		}
	}
	
	// ----------------------------------------------------------------------
	// ********* D+
	for (j=0;j<Dpm.GetLength();++j) 
	{
		double m=Dpm[j]->M();
		
		h_dpm->Fill(m);
		
		double beta = Dpm[j]->P4().Beta();
		double gamma = Dpm[j]->P4().Gamma();
		
		// determine pseudo vertex
		PndVtxPoca poca;
		TVector3 vtx;
		double pocaquality = poca.GetPocaVtx(vtx,Dpm.Get(j));
		
		// decay length
		h_d0_l->Fill(vtx.Mag()/(beta*gamma));
		
		// vertex
		h_d0_pocx->Fill(vtx.X());
		h_d0_pocy->Fill(vtx.Y());
		h_d0_pocz->Fill(vtx.Z());
		
		// store mc truth matched cands
		if (theAnalysis->McTruthMatch(Dpm.Get(j))) 
		{
			h_dpmt->Fill(m);
			h_d0_pocxm->Fill(vtx.X());
			h_d0_pocym->Fill(vtx.Y());
			h_d0_poczm->Fill(vtx.Z());
			h_d0_lm->Fill(vtx.Mag()/(beta*gamma));
		}
		
		// store selected cands
		if (fabs(m-fDMass)<fDMassCut ) 
		{
			h_dpmsel->Fill(m);
			founddpm++;
			comb++;
		}
	}
	
	// ----------------------------------------------------------------------
	// ********* Ds
	for (j=0;j<Ds.GetLength();++j) 
	{
		double m=Ds[j]->M();
		
		h_ds->Fill(m);
		
		if (theAnalysis->McTruthMatch(Ds.Get(j)))  h_dst->Fill(m);
		
		if ( fabs(m-fDsMass)<fDsMassCut ) 
		{
			h_dssel->Fill(m);
			foundds++;
			comb++;
		}
	}
	
	// ----------------------------------------------------------------------
	// ********* Phi
	for (j=0;j<Phi.GetLength();++j) 
	{
		double m=Phi[j]->M();
		
		h_phi->Fill(m);
		
		if (theAnalysis->McTruthMatch(Phi.Get(j))) h_phit->Fill(m);
		
		if (fabs(m-fPhiMass)<fPhiMassCut ) 
		{
			h_phisel->Fill(m);
			foundphi++;
			comb++;
		}
	}
	
	// ----------------------------------------------------------------------
	// ********* Lam c
	for (j=0;j<Lamc.GetLength();++j) 
	{
		double m=Lamc[j]->M();
		
		h_lamc->Fill(m);
		
		if (theAnalysis->McTruthMatch(Lamc.Get(j))) h_lamct->Fill(m);
			
		if ( fabs(m-fLamcMass)<fLamcMassCut ) 
		{
			h_lamcsel->Fill(m);
			foundlamc++;
			comb++;
		}
	}
	
	if (comb>0) cntsel++;
	
	if (foundjpsi>0) cntjpsi++;
	if (foundd0>0) cntd0++;
	if (founddpm>0) cntdpm++;
	if (foundds>0) cntds++;
	if (foundphi>0) cntphi++;
	if (foundlamc>0) cntlamc++;

	
}
// -------------------------------------------------------------------------
	
void PndSoftTriggerTask::ConfigureHistos(TH1F *hall, TH1F *htrue, TH1F *hsel)
{
	htrue->SetLineColor(2);
	hsel->SetFillStyle(3001);
	hsel->SetFillColor(602);
	hsel->SetLineStyle(3);
	hsel->SetLineColor(602);
	
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::FillMassHisto(TH1F* h, RhoCandList &l)
{
	Int_t i=0;
	for (i=0;i<l.GetLength();++i)
	{
		h->Fill(l[i]->M());
	}
}
// -------------------------------------------------------------------------

int PndSoftTriggerTask::RemoveDoubles(RhoCandList &l, double limit)
{
	int i,j;
	int rem=0;

	//cout <<"********* before*****"<<endl<<l<<endl;
		
	for (i=0;i<l.GetLength();++i)
	{
		bool sim=false;
		for (j=i+1;j<l.GetLength();++j)
		{
		  if(!sim && 
		     fabs(l[i]->Px()-l[j]->Px())<limit &&
		     fabs(l[i]->Py()-l[j]->Py())<limit &&
		     fabs(l[i]->Pz()-l[j]->Pz())<limit &&
		     fabs(l[i]->E()-l[j]->E())<limit)
		    {
		      sim=true;
		    }
		      //if (similarity(l[i].P4(), l[j]->P4())<limit){ sim=true; j=10000;}
		}
		if (sim) {l.Remove(l[i--]); rem++;}
	}
	//cout<<"1:" <<rem<<endl;
	//cout <<"********* after *****"<<endl<<l<<endl;
	
	return rem;
}


// -------------------------------------------------------------------------
void PndSoftTriggerTask::PrintList(RhoCandList &l, int max)
{
	int N=l.GetLength();
	if (N>max) N=max;
	for (int j=0;j<N;++j)
	{
		cout <<*(l[j])<<" PDG:"<<l[j]->PdgCode()<<" MC pointer:"<<l[j]->GetMcTruth()<<endl;
	}
	cout <<endl;
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::SelectPid(int type, int pdg, int chrg, RhoCandList &l, RhoCandList &lpid, double cut)
{
	lpid.Cleanup();
			
	for (int j=0;j<l.GetLength();++j)
	{
		if (fabs(l[j]->Charge()-double(chrg))<0.001 && l[j]->GetPidInfo(type)>=cut)
		{
          //RhoCandidate c(l[j]);
			//c.SetMass(pdgmass[type]);
			lpid.Put(l[j]);
		}		
	}
	for (int j=0;j<lpid.GetLength();++j) lpid[j]->SetType(pdg);
}

// -------------------------------------------------------------------------

int PndSoftTriggerTask::SelectPdgCode(RhoCandList &mct, RhoCandList &l)
{
	int removed = 0;
	int pdgcode=0;
	int nmct = mct.GetLength();
	
	//PndMcTruthMatch mcm;
	
	if (l.GetLength()>0) pdgcode = l[0]->PdgCode();
	
	for (int ii=l.GetLength();ii>=0;--ii)
	{
		RhoCandidate* mccand = l[ii]->GetMcTruth();
        
		if (mccand)
          if(mccand->PdgCode()==pdgcode)
//      if (mcm.MctMatch(l[ii],mct))
            continue; //don't remove if good mctruth
		l.Remove(l[ii]);
		removed++;
	}
	return removed;
}

// -------------------------------------------------------------------------

void PndSoftTriggerTask::Finish()
{
	cout <<"Efficiency (ev):"      << (double)cntsel / (double)evcount <<endl;
	cout <<"Efficiency J/psi (ev):"<< (double)cntjpsi/ (double)evcount <<endl;
	cout <<"Efficiency D0    (ev):"<< (double)cntd0  / (double)evcount <<endl;
	cout <<"Efficiency D+-   (ev):"<< (double)cntdpm / (double)evcount <<endl;
	cout <<"Efficiency Ds    (ev):"<< (double)cntds  / (double)evcount <<endl;
	cout <<"Efficiency Phi   (ev):"<< (double)cntphi / (double)evcount <<endl;
	cout <<"Efficiency Lamc  (ev):"<< (double)cntlamc/ (double)evcount <<endl;
	
	h_mom->Write();
	h_momc->Write();
	h_momn->Write();
	h_mult->Write();
	h_multc->Write();
	h_multn->Write();
	h_multrem->Write();
		
	h_jpsi->Write();
	h_jpsit->Write();
	h_jpsisel->Write();
		
	h_d0->Write();
	h_d0t->Write();
	h_d0sel->Write();
		
	h_d0_pocx->Write();
	h_d0_pocy->Write();
	h_d0_pocz->Write();
		
	h_d0_pocxm->Write();
	h_d0_pocym->Write();
	h_d0_poczm->Write();
		
	h_d0_l->Write();
	h_d0_lm->Write();
		
	h_dpm->Write();
	h_dpmt->Write();
	h_dpmsel->Write();
		
	h_ds->Write();
	h_dst->Write();
	h_dssel->Write();
		
	h_phi->Write();
	h_phit->Write();
	h_phisel->Write();
		
	h_lamc->Write();
	h_lamct->Write();
	h_lamcsel->Write();
	
	ntp->GetInternalTree()->Write();
	ntp2->GetInternalTree()->Write();

}

ClassImp(PndSoftTriggerTask)
