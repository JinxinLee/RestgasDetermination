
#include "PndRhoTupleQA.h"

#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "RhoTuple.h"
#include "PndEventShape.h"
#include "PndAnalysis.h"
#include "PndPidCandidate.h"
#include "PndVtxPoca.h"
#include "PndVtxPRG.h"

PndRhoTupleQA::PndRhoTupleQA(PndAnalysis *ana, double pbarmom)
{
	fAnalysis = ana;
	fVtxPoca = new PndVtxPoca();
	double mp=0.938272;
	fIniP4.SetXYZT(0,0,pbarmom, sqrt(pbarmom*pbarmom+mp*mp)+mp);
}
// -------------------------------------------------------------------------

PndRhoTupleQA::~PndRhoTupleQA()
{
	delete fVtxPoca;
}
// -------------------------------------------------------------------------

void PndRhoTupleQA::qaESPidMult(TString pre, PndEventShape *evsh, double prob, double pmin, RhoTuple *n)
{
	n->Column(pre+"npide", (Float_t) evsh->MultElectronPminLab(prob,pmin),		0.0f );
	n->Column(pre+"npidmu",(Float_t) evsh->MultMuonPminLab(prob,pmin),			0.0f );
	n->Column(pre+"npidpi",(Float_t) evsh->MultPionPminLab(prob,pmin),			0.0f );
	n->Column(pre+"npidk", (Float_t) evsh->MultKaonPminLab(prob,pmin),			0.0f );
	n->Column(pre+"npidp", (Float_t) evsh->MultProtonPminLab(prob,pmin),		0.0f );
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaESMult(TString pre, PndEventShape *evsh, RhoTuple *n)
{
	n->Column(pre+"npart", (Float_t)  	evsh->NParticles(),	0.0f );
	n->Column(pre+"nneut", (Float_t)	evsh->NNeutral(),	0.0f );
	n->Column(pre+"nchrg", (Float_t)  	evsh->NCharged(),	0.0f );
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaESSum(TString pre, PndEventShape *evsh, RhoTuple *n)
{
	n->Column(pre+"sumpc", 	(Float_t)  evsh->ChrgPSumCms(),		0.0f );
	n->Column(pre+"sumpcl", (Float_t)  evsh->ChrgPSumLab(),		0.0f );
	n->Column(pre+"sumen", 	(Float_t)  evsh->NeutESumCms(),		0.0f );
	n->Column(pre+"sumenl", (Float_t)  evsh->NeutESumLab(),		0.0f );

	n->Column(pre+"sumpt", 	(Float_t)  evsh->PtSumCms(),			0.0f );
	n->Column(pre+"sumptl",  (Float_t) evsh->PtSumLab(),			0.0f );
	n->Column(pre+"sumptc", (Float_t)  evsh->ChrgPtSumCms(),		0.0f );
	n->Column(pre+"sumptcl",(Float_t)  evsh->ChrgPtSumLab(),		0.0f );
	n->Column(pre+"sumetn",  (Float_t) evsh->NeutEtSumCms(),		0.0f );
	n->Column(pre+"sumetnl", (Float_t) evsh->NeutEtSumLab(),		0.0f );
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaESMinMax(TString pre, PndEventShape *evsh, RhoTuple *n)
{
	n->Column(pre+"pmax", (Float_t)	  evsh->PmaxCms(),	0.0f );
	n->Column(pre+"ptmax",(Float_t)	  evsh->Ptmax() ,	0.0f );
	n->Column(pre+"pmaxl",(Float_t)	  evsh->PmaxLab() ,	0.0f );
	n->Column(pre+"pmin", (Float_t)	  evsh->PminCms(),	0.0f );
	n->Column(pre+"ptmin",(Float_t)   evsh->Ptmin(),	0.0f );
	n->Column(pre+"pminl",(Float_t)	  evsh->PminLab(),	0.0f );	
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaESEventVars(TString pre, PndEventShape *evsh, RhoTuple *n)
{
	n->Column(pre+"prapmax",(Float_t) evsh->PRapmax(),		0.0f );
	n->Column(pre+"sph", (Float_t)	  evsh->Sphericity(),	0.0f );
	n->Column(pre+"apl", (Float_t)	  evsh->Aplanarity(),	0.0f );
	n->Column(pre+"pla", (Float_t)	  evsh->Planarity(),	0.0f );
	n->Column(pre+"thr",(Float_t)	  evsh->Thrust(),		0.0f );
	n->Column(pre+"cir", (Float_t)	  evsh->Circularity(),	0.0f );	

	n->Column(pre+"fw1", (Float_t)	  evsh->FoxWolfMomR(1),	0.0f );
	n->Column(pre+"fw2", (Float_t)	  evsh->FoxWolfMomR(2),	0.0f );
	n->Column(pre+"fw3", (Float_t)	  evsh->FoxWolfMomR(3),	0.0f );
	n->Column(pre+"fw4", (Float_t)	  evsh->FoxWolfMomR(4),	0.0f );
	n->Column(pre+"fw5", (Float_t)	  evsh->FoxWolfMomR(5),	0.0f );
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaEventShape(TString pre, PndEventShape *evsh, RhoTuple *n)
{
	if (n==0) return;
	
	// basic multiplicities
	qaESMult(pre,  evsh, n);
		
	// PID multiplicities 
	qaESPidMult(pre+"l",  evsh, 0.25, 0.0, n);
	qaESPidMult(pre+"l1", evsh, 0.25, 1.0, n);
	qaESPidMult(pre+"t",  evsh, 0.5,  0.0, n);
	qaESPidMult(pre+"t1", evsh, 0.5,  1.0, n);
	qaESPidMult(pre+"vt", evsh, 0.9,  0.0, n);
	
	// event vars like thrust, sphericity etc
	qaESEventVars(pre, evsh, n);
	
	// standard sums over pt, p, E for all, neutral, charged in lab, cms
	qaESSum(pre, evsh, n);
	
	// standard min, max values of p, pt in lab, cms
	qaESMinMax(pre, evsh, n);
	
	// Multiplicities with min momemtum cut (cms)
	n->Column(pre+"np05", (Float_t)	  evsh->MultPminCms(0.5),	0.0f );
	n->Column(pre+"np10", (Float_t)	  evsh->MultPminCms(1.0),	0.0f );
	n->Column(pre+"np20", (Float_t)	  evsh->MultPminCms(2.0),	0.0f );
	n->Column(pre+"np30", (Float_t)	  evsh->MultPminCms(3.0),	0.0f );
	n->Column(pre+"np40", (Float_t)	  evsh->MultPminCms(4.0),	0.0f );
	n->Column(pre+"np50", (Float_t)	  evsh->MultPminCms(5.0),	0.0f );
	
	// Multiplicities with min momemtum cut (lab)
	n->Column(pre+"np05l", (Float_t)  evsh->MultPminLab(0.5),	0.0f );
	n->Column(pre+"np10l", (Float_t)  evsh->MultPminLab(1.0),	0.0f );
	n->Column(pre+"np20l", (Float_t)  evsh->MultPminLab(2.0),	0.0f );
	n->Column(pre+"np30l", (Float_t)  evsh->MultPminLab(3.0),	0.0f );
	n->Column(pre+"np40l", (Float_t)  evsh->MultPminLab(4.0),	0.0f );
	n->Column(pre+"np50l", 	(Float_t)  evsh->MultPminLab(5.0),	0.0f );
	
	// Multiplicities with min p_t cut (cms)
	n->Column(pre+"npt05", 	(Float_t)  evsh->MultPtminCms(0.5),	0.0f );
	n->Column(pre+"npt10", 	(Float_t)  evsh->MultPtminCms(1.0),	0.0f );
	n->Column(pre+"npt15", 	(Float_t)  evsh->MultPtminCms(1.5),	0.0f );
	n->Column(pre+"npt20", 	(Float_t)  evsh->MultPtminCms(2.0),	0.0f );
	n->Column(pre+"npt25", 	(Float_t)  evsh->MultPtminCms(2.5),	0.0f );
	n->Column(pre+"npt30", 	(Float_t)  evsh->MultPtminCms(3.0),	0.0f );
	
	// Neutral multiplicities with min energy cut (lab)
	n->Column(pre+"nne003l",  (Float_t) evsh->MultNeutEminLab(0.03),	0.0f );
	n->Column(pre+"nne005l",  (Float_t) evsh->MultNeutEminLab(0.05),	0.0f );
	n->Column(pre+"nne01l",   (Float_t) evsh->MultNeutEminLab(0.1),	0.0f );
	n->Column(pre+"nne05l",   (Float_t) evsh->MultNeutEminLab(0.5),	0.0f );
		
	// Charged multiplicities with min momentum cut (lab)
	n->Column(pre+"ncp005l", (Float_t)	  evsh->MultChrgPminLab(0.05),	0.0f );
	n->Column(pre+"ncp01l", (Float_t)	  evsh->MultChrgPminLab(0.1),	0.0f );
	n->Column(pre+"ncp02l", (Float_t)	  evsh->MultChrgPminLab(0.2),	0.0f );
	n->Column(pre+"ncp05l", (Float_t)	  evsh->MultChrgPminLab(0.5),	0.0f );
	n->Column(pre+"ncp10l", (Float_t)	  evsh->MultChrgPminLab(1.0),	0.0f );
	
	// Charged multiplicities with min momentum cut (cms)
	n->Column(pre+"ncp005", (Float_t)	  evsh->MultChrgPminCms(0.05),	0.0f );
	n->Column(pre+"ncp01", (Float_t)	  evsh->MultChrgPminCms(0.1),	0.0f );
	n->Column(pre+"ncp02", (Float_t)	  evsh->MultChrgPminCms(0.2),	0.0f );
	n->Column(pre+"ncp05", (Float_t)	  evsh->MultChrgPminCms(0.5),	0.0f );
	n->Column(pre+"ncp10", (Float_t)	  evsh->MultChrgPminCms(1.0),	0.0f );
		
	// Sum of p_t  with min momentum cut (cms)
	n->Column(pre+"sumpt05", (Float_t) evsh->SumPtminCms(0.5),	0.0f );
	n->Column(pre+"sumpt10", (Float_t) evsh->SumPtminCms(1.0),	0.0f );
	
	// Sum of charged momenta with min momentum cut (cms)
	n->Column(pre+"sumpc05", (Float_t) evsh->SumChrgPminCms(0.5) ,0.0f );
	n->Column(pre+"sumpc10", (Float_t) evsh->SumChrgPminCms(1.0),0.0f );
	
	// Sum of charged momenta with min momentum cut (lab)
	n->Column(pre+"sumpc05l", (Float_t)evsh->SumChrgPminLab(0.5) ,0.0f );
	n->Column(pre+"sumpc10l",(Float_t) evsh->SumChrgPminLab(1.0) ,0.0f );
	
	// Sum of neutral energy with min energy cut (cms)
	n->Column(pre+"sumen05",  (Float_t)evsh->SumNeutEminCms(0.5) ,0.0f );
	n->Column(pre+"sumen10", (Float_t) evsh->SumNeutEminCms(1.0) ,0.0f );
	
	// Sum of neutral energy with min energy cut (lab)
	n->Column(pre+"sumen05l",(Float_t) evsh->SumNeutEminLab(0.5) ,0.0f );
	n->Column(pre+"sumen10l",(Float_t) evsh->SumNeutEminLab(1.0) ,0.0f );
	
}
// -------------------------------------------------------------------------
void PndRhoTupleQA::qaEventShapeShort(TString pre, PndEventShape *evsh, RhoTuple *n)
{
	if (n==0) return;
	// *** vars for PID multiplicity
	int ne, nmu, npi, nk, np;
	
	// basic multiplicities
	qaESMult(pre,  evsh, n);
		
	// PID multiplicities 
	qaESPidMult(pre+"l",  evsh, 0.25, 0.0, n);
	qaESPidMult(pre+"l1", evsh, 0.25, 1.0, n);
	
	// event vars like thrust, sphericity etc
	qaESEventVars(pre, evsh, n);
	
	// standard sums over pt, p, E for all, neutral, charged in lab, cms
	qaESSum(pre, evsh, n);
	
	// standard min, max values of p, pt in lab, cms
	qaESMinMax(pre, evsh, n);
	
	// some multiplicities with min momentum cut (lab, cms)
	n->Column(pre+"np10",  (Float_t)  evsh->MultPminCms(1.0),	 0.0f );
	n->Column(pre+"npt10", (Float_t)  evsh->MultPtminCms(1.0),	 0.0f );
	n->Column(pre+"ncp10l", (Float_t) evsh->MultChrgPminLab(1.0), 0.0f );
	n->Column(pre+"nne10l",(Float_t)  evsh->MultNeutEminLab(1.0), 0.0f );
		
	// sum of charged momenta with min momentum cut
	n->Column(pre+"sumpc05", (Float_t) evsh->SumChrgPminCms(0.5) ,0.0f );
}
// -------------------------------------------------------------------------
// *** store QA for PocaVtx
void PndRhoTupleQA::qaPoca(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	// *** simple vtx finder
	TVector3 vtx;
	double qavtx = fVtxPoca->GetPocaVtx(vtx, c);
	
	// *** store QA info
	n->Column(pre+"pocvx",  (Float_t) vtx.X(), 0.0f);
	n->Column(pre+"pocvy",  (Float_t) vtx.Y(), 0.0f);
	n->Column(pre+"pocvz",  (Float_t) vtx.Z(), 0.0f);
	n->Column(pre+"pocqa", (Float_t) qavtx,   0.0f);
	
}

// -------------------------------------------------------------------------
// *** store QA for PRG Vtx
void PndRhoTupleQA::qaPRG(TString pre, RhoCandidate *c, RhoTuple *n)
{
    if (n==0) return;

	PndVtxPRG vtxPRG(c);
	vtxPRG.SetSilent();     
    
    // *** PRG vtx finder
    TVector3 vtx(0,0,0);
    TMatrixD vmatrix(3,3);
	double qavtx = vtxPRG.FitVertexFast(vtx, vmatrix, true, 2);//iteration 2=default
    
	// *** store QA info
    n->Column(pre+"prgvx",  (Float_t) vtx.X(), 0.0f);
    n->Column(pre+"prgvy",  (Float_t) vtx.Y(), 0.0f);
    n->Column(pre+"prgvz",  (Float_t) vtx.Z(), 0.0f);
    n->Column(pre+"prgqa",  (Float_t) qavtx,   0.0f);
}

// -------------------------------------------------------------------------
// *** store QA for composite particles

void PndRhoTupleQA::qaComp(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	// what kind of particle?
	int pdg = c->PdgCode();

	// special composite particle?
	// pi0 or eta?
	if (pdg == 111 || pdg == 221)
	{
		qaPi0(pre, c, n);
		return;
	}
	// K_S?
	if (pdg == 310)
	{
		qaKs0(pre, c, n);
		return;
	}
	
	// how many daughters?
	int nd = c->NDaughters();
	
	// truth match already done?
	RhoCandidate *truth = c->GetMcTruth();
	
	// if not, try one
	if (truth == 0 && fAnalysis != 0) 
	{
		fAnalysis->McTruthMatch(c);
		truth = c->GetMcTruth();
	}
	
	bool mct = false;
	if (truth!=0) 
	{
		if (nd>0) mct = true;
		else mct = (truth->PdgCode()==c->PdgCode());
	}
	// store cand info in lab and cms
	qaCand(pre,	c,	n);
	qaP4Cms(pre, c->P4(), n);
	n->Column(pre+"mct", (Float_t) mct, 0.0f);
	
	// cand is final state particle
	if (nd==0)
	{
		// charged particle -> store PID info
		if ( fabs(c->Charge())>0.1 ) qaPid(pre,	c,	n);
	}
	// cand is composite
	else 
	{
		// if 2 daughters -> store decay angle etc
		if (nd==2) qa2Body(pre, c, n);
		// if 3 daughters -> dalitz plot vars
		if (nd==3) qaDalitz(pre, c, n);
		
		// counter for charged final states
		int nchrgfs = 0;
		
		for (int i=0; i<nd; ++i) 
		{
			RhoCandidate *dau = c->Daughter(i);
			TString name=TString::Format("%sd%d",pre.Data(),i);
			
			// count charged final states for possible vertexing later
			if (dau->NDaughters()==0 && fabs(dau->Charge())>0.01) nchrgfs++;
			
			// recursive call of qaComp
			qaComp(name, dau, n);
		}
		// only charged final state daughters -> Vtx info with PndVtxPoca 
		if (nchrgfs == nd)
			qaPoca(pre, c, n);
	}
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaPi0(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	qaCand(pre, c, n);
	
	RhoCandidate *d0 = c->Daughter(0);
	RhoCandidate *d1 = c->Daughter(1);
	double ang = d0->P3().Angle(d1->P3());
	
	if (fAnalysis) fAnalysis->McTruthMatch(c);
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

void PndRhoTupleQA::qaKs0(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;

	RhoCandidate *d0 = c->Daughter(0);
	RhoCandidate *d1 = c->Daughter(1);
	double ang = d0->P3().Angle(d1->P3());
	
	TVector3 vtx;
	
	if (fAnalysis!=0) fAnalysis->McTruthMatch(c);
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

void PndRhoTupleQA::qaP4(TString pre, TLorentzVector &c, RhoTuple *n, bool skip)
{
	if (n==0) return;
	
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

void PndRhoTupleQA::qaP4Cms(TString pre, TLorentzVector c, RhoTuple *n, bool skip)
{
	if (n==0) return;
	
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

void PndRhoTupleQA::qaCand(TString pre, RhoCandidate *cc, RhoTuple *n, bool skip)
{
	if (n==0) return;
	
	if (!skip)
	{
		TLorentzVector c=cc->P4();
		TVector3       p=cc->Pos();
		
		qaP4(pre, c, n);
		n->Column(pre+"chg", (Float_t) cc->Charge(), 	0.0f );
		n->Column(pre+"x",   (Float_t) p.X(),     		0.0f );
		n->Column(pre+"y",   (Float_t) p.Y(),			0.0f );
		n->Column(pre+"z",   (Float_t) p.Z(),			0.0f );
		n->Column(pre+"l",   (Float_t) p.Mag(),			0.0f );
		n->Column(pre+"pdg", (Float_t) cc->PdgCode(),	0.0f );
	}
	else
	{
		TLorentzVector dummy;
		qaP4(pre, dummy, n, true);
		n->Column(pre+"chg", (Float_t) -999., 			0.0f );
		n->Column(pre+"x",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"y",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"z",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"l",   (Float_t) -999.,  			0.0f );
		n->Column(pre+"pdg", (Float_t) -999.,			0.0f );
	}
}
// -------------------------------------------------------------------------

void PndRhoTupleQA::qaTrk(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"trkdof",  	(Float_t) mic->GetDegreesOfFreedom(),	0.0f );
		n->Column(pre+"trkstat",  	(Float_t) mic->GetFitStatus(),			0.0f ); 
		n->Column(pre+"trkchi2",  	(Float_t) mic->GetChiSquared(),			0.0f );
		n->Column(pre+"trkidx",  	(Float_t) mic->GetTrackIndex(),			0.0f );	
		n->Column(pre+"trkbranch",	(Float_t) mic->GetTrackBranch() ,		0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaPid(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	if (n==0) return;
	
	// pinf[0]...pinf[4]: cache P values, pinf[5] = maximum
	double pinf[6] = {-1000.,-1000.,-1000.,-1000.,-1000.,-1000.};
	// index of particle type (e=0 ... p=4) with maximum P 
	int bestidx = -1;
	
	if (fabs(c->Charge())>0.1)
	{
		for (int i=0;i<5;++i)
		{
			pinf[i] = c->GetPidInfo(i);
			
			if (pinf[i]>pinf[5])
			{
				pinf[5] = pinf[i];
				bestidx = i;
			}
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
void PndRhoTupleQA::qa2Body(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	if (n==0) return;
	
	if (c->NDaughters()!=2) return;
	
	RhoCandidate *d0 = c->Daughter(0);
	RhoCandidate *d1 = c->Daughter(1);
	
	// opening angle lab
	double oang = d0->P3().Angle(d1->P3());
	
	// decay angle
	TLorentzVector d_cms = d0->P4();
	d_cms.Boost(-(c->P4().BoostVector()));
	Float_t dec  = d_cms.Vect().Angle(c->P());
	Float_t cdec = cos(dec);
	
	n->Column(pre+"oang", 	 (Float_t) oang,		0.0f );
	n->Column(pre+"decang",  (Float_t) dec,		0.0f );
	n->Column(pre+"cdecang", (Float_t) cdec,		0.0f );
}


// -------------------------------------------------------------------------
void PndRhoTupleQA::qaDalitz(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	if (n==0) return;
	
	if (c->NDaughters()!=3) return;
	
	TLorentzVector l01 = c->Daughter(0)->P4()+c->Daughter(1)->P4();
	TLorentzVector l12 = c->Daughter(1)->P4()+c->Daughter(2)->P4();	
	TLorentzVector l02 = c->Daughter(2)->P4()+c->Daughter(0)->P4();	
		
	n->Column(pre+"m01", (Float_t) l01.M(),		0.0f );
	n->Column(pre+"m12", (Float_t) l12.M(),		0.0f );
	n->Column(pre+"m02", (Float_t) l02.M(),		0.0f );
	
	n->Column(pre+"dal01", (Float_t) l01.M2(),		0.0f );
	n->Column(pre+"dal12", (Float_t) l12.M2(),		0.0f );
	n->Column(pre+"dal02", (Float_t) l02.M2(),		0.0f );
}


// -------------------------------------------------------------------------

void PndRhoTupleQA::qaVtx(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	if (n==0) return;
	
	RhoCandidate *d = c->Daughter(0);
	TVector3 v = c->DecayVtx();
	if (d)
	{
		if (v.X()==0 && v.Y()==0 && v.Z()==0) 
			v = d->Pos();
		TLorentzVector d_cms = d->P4();
		d_cms.Boost(-(c->P4().BoostVector()));
		
		Float_t ctau = v.Mag()*c->M()/c->P();
		Float_t dec  = d_cms.Vect().Angle(c->P());
		Float_t cdec = cos(dec);
		
		n->Column(pre+"vx",  	(Float_t) v.X(),		0.0f );
		n->Column(pre+"vy",  	(Float_t) v.Y(),		0.0f );
		n->Column(pre+"vz",  	(Float_t) v.Z(),		0.0f );
		n->Column(pre+"len", 	(Float_t) v.Mag(),		0.0f );
		n->Column(pre+"ctau",	(Float_t) ctau,		0.0f );
		n->Column(pre+"decang", (Float_t) dec,			0.0f );
		n->Column(pre+"cdecang",(Float_t) cdec,		0.0f );
	}
	else
	{
		n->Column(pre+"vx",  	(Float_t) -999.0,		0.0f );
		n->Column(pre+"vy",  	(Float_t) -999.0,		0.0f );
		n->Column(pre+"vz",  	(Float_t) -999.0,		0.0f );
		n->Column(pre+"len", 	(Float_t) -999.0,		0.0f );
		n->Column(pre+"ctau",  	(Float_t) -999.0,		0.0f );
		n->Column(pre+"decang", (Float_t) -999.0,		0.0f );
		n->Column(pre+"cdecang",(Float_t) -999.0,		0.0f );
	}
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaEmc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"emceraw",  (Float_t) mic->GetEmcRawEnergy(),			0.0f );
		n->Column(pre+"emcecal",  (Float_t) mic->GetEmcCalEnergy(),			0.0f );
		n->Column(pre+"emcqa",    (Float_t) mic->GetEmcQuality(),   		0.0f );
		n->Column(pre+"emcnx",    (Float_t) mic->GetEmcNumberOfCrystals(),	0.0f );
		n->Column(pre+"emcnb",    (Float_t) mic->GetEmcNumberOfBumps(),   	0.0f );
		n->Column(pre+"emcz20",   (Float_t) mic->GetEmcClusterZ20(),   		0.0f );
		n->Column(pre+"emcz53",   (Float_t) mic->GetEmcClusterZ53(),   		0.0f );
		n->Column(pre+"emclat",   (Float_t) mic->GetEmcClusterLat(),   		0.0f );
		n->Column(pre+"emce1",    (Float_t) mic->GetEmcClusterE1(),   		0.0f );
		n->Column(pre+"emce9",    (Float_t) mic->GetEmcClusterE9(),   		0.0f );
		n->Column(pre+"emce25",   (Float_t) mic->GetEmcClusterE25(),   		0.0f );
		n->Column(pre+"emcmod",   (Float_t) mic->GetEmcModule(),   			0.0f );
		n->Column(pre+"emcidx",   (Float_t) mic->GetEmcIndex(),   			0.0f );
	}		
}
// -------------------------------------------------------------------------

void PndRhoTupleQA::qaGem(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"gemnhits",  (Float_t) mic->GetMvdDEDX(),			0.0f );
	}		
}


// -------------------------------------------------------------------------

void PndRhoTupleQA::qaMvd(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"mvddedx",  (Float_t) mic->GetMvdDEDX(),			0.0f );
		n->Column(pre+"mvdhits",  (Float_t) mic->GetMvdHits(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaStt(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"sttdedx",  (Float_t) mic->GetSttMeanDEDX(),		0.0f );
		n->Column(pre+"stthits",  (Float_t) mic->GetSttHits(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaDrc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"drcthtc",  	(Float_t) mic->GetDrcThetaC(),			0.0f );
		n->Column(pre+"drcdthtc", 	(Float_t) mic->GetDrcThetaCErr(),		0.0f );
		n->Column(pre+"drcqa",  	(Float_t) mic->GetDrcQuality(),			0.0f );
		n->Column(pre+"drcnphot",  	(Float_t) mic->GetDrcNumberOfPhotons(),	0.0f );
		n->Column(pre+"drcidx",  	(Float_t) mic->GetDrcIndex(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaDsc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"dscthtc",  	(Float_t) mic->GetDiscThetaC(),			0.0f );
		n->Column(pre+"dscdthtc",  	(Float_t) mic->GetDiscThetaCErr(),		0.0f );
		n->Column(pre+"dscqa",  	(Float_t) mic->GetDiscQuality(),		0.0f );
		n->Column(pre+"dscnphot",  	(Float_t) mic->GetDiscNumberOfPhotons(),0.0f );
		n->Column(pre+"dscidx",  	(Float_t) mic->GetDiscIndex(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaRich(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"richthtc",  	(Float_t) mic->GetRichThetaC(),			0.0f );
		n->Column(pre+"richdthtc",  	(Float_t) mic->GetRichThetaCErr(),		0.0f );
		n->Column(pre+"richqa",  	(Float_t) mic->GetRichQuality(),		0.0f );
		n->Column(pre+"richnphot",  	(Float_t) mic->GetRichNumberOfPhotons(),0.0f );
		n->Column(pre+"richidx",  	(Float_t) mic->GetRichIndex(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaMuo(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"muonlay",  	(Float_t) mic->GetMuoNumberOfLayers(),	0.0f );;
		n->Column(pre+"muoprob",  	(Float_t) mic->GetMuoProbability(),		0.0f );;
		n->Column(pre+"muoqa",  	(Float_t) mic->GetMuoQuality() ,		0.0f ); ;
		n->Column(pre+"muoiron",  	(Float_t) mic->GetMuoIron() ,			0.0f );  ;  
		n->Column(pre+"muopin",  	(Float_t) mic->GetMuoMomentumIn(),		0.0f );;
		n->Column(pre+"muomod", 	(Float_t) mic->GetMuoModule(),			0.0f );  ;  
		n->Column(pre+"muohits",  	(Float_t) mic->GetMuoHits(),			0.0f );  ;  
		n->Column(pre+"muoidx",  	(Float_t) mic->GetMuoIndex(),			0.0f ); ;    
	}		
}

// -------------------------------------------------------------------------

void PndRhoTupleQA::qaTof(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"toftime", 	(Float_t) mic->GetTofStopTime(),	0.0f );
		n->Column(pre+"tofm2",  	(Float_t) mic->GetTofM2(),			0.0f );  
		n->Column(pre+"toflen",  	(Float_t) mic->GetTofTrackLength(),	0.0f );
		n->Column(pre+"tofqa",  	(Float_t) mic->GetTofQuality(),		0.0f );
		n->Column(pre+"tofidx",  	(Float_t) mic->GetTofIndex(),		0.0f ); 
		n->Column(pre+"tofbeta",  	(Float_t) mic->GetTofBeta(),		0.0f );
	}		
}

// -------------------------------------------------------------------------
void PndRhoTupleQA::qaRecoFull(TString pre, RhoCandidate *c, RhoTuple *n)
{
	if (n==0) return;
	
	qaEmc( pre, c, n );
	qaMvd( pre, c, n );
	qaStt( pre, c, n );
	qaGem( pre, c, n );
	qaDrc( pre, c, n );
	qaDsc( pre, c, n );
	qaRich( pre, c, n );
	qaTof( pre, c, n );
	qaMuo( pre, c, n );
	qaTrk( pre, c, n);
}


