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
#include "PndTutAnaTaskD0.h"

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
#include "Rho4CFitter.h"
#include "RhoKinVtxFitter.h"
#include "PndChiVtxFitter.h"
#include "RhoKalmanVtxFitter.h"
#include "RhoKinFitter.h"
#include "RhoVtxPoca.h"
#include "PndPidCandidate.h"
#include "PndEventShape.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndTutAnaTaskD0::PndTutAnaTaskD0(double pbarmom) :
  FairTask("Panda Tutorial Analysis Task") 
{ 
	double mp=0.938272;
	fIni.SetXYZT(0,0,pbarmom, sqrt(pbarmom*pbarmom+mp*mp)+mp);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndTutAnaTaskD0::~PndTutAnaTaskD0() { }
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndTutAnaTaskD0::Init() 
{		
	// *** initialize PndAnalysis object
	fAnalysis = new PndAnalysis();
	
	// *** reset the event counter
	fEvtCount = 0;

	// *******
	// ******* PREPARE/CREATE THE STUFF YOU NEED
	// *******
	fD0Sel 	 = new RhoMassParticleSelector("D0Sel",   1.864, 1.0);
	nd0 	 = new RhoTuple("nd0","my D0 tuple");
	fVtxPoca = new RhoVtxPoca();
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndTutAnaTaskD0::SetParContainers() 
{
  // *** Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndTutAnaTaskD0::Exec(Option_t*)
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
	RhoCandList kp, km, pip, pim, d0, d0b;
	TString pidsel = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts";
	
	fAnalysis->FillList( kp,  "KaonAllPlus",  pidsel);
	fAnalysis->FillList( km,  "KaonAllMinus", pidsel);
	fAnalysis->FillList( pip, "PionAllPlus",  pidsel);
	fAnalysis->FillList( pim, "PionAllMinus", pidsel);

	d0.Combine(km, pip);
	d0.SetType(421);
	d0b.Combine(kp, pim);
	d0b.SetType(-421);
	d0.Append(d0b);
	
	d0.Select(fD0Sel);
	
	for (j=0;j<d0.GetLength();++j)
	{
		RhoCandidate *dka = d0[j]->Daughter(0);
		RhoCandidate *dpi = d0[j]->Daughter(1);
		
		PndPidCandidate *dkar = (PndPidCandidate*)dka->GetRecoCandidate();
		PndPidCandidate *dpir = (PndPidCandidate*)dpi->GetRecoCandidate();

		TLorentzVector d0cm = d0[j]->P4();
		TLorentzVector kacm = dka->P4();
		TLorentzVector picm = dpi->P4();
		d0cm.Boost(-fIni.BoostVector());
		kacm.Boost(-fIni.BoostVector());
		picm.Boost(-fIni.BoostVector());
				
		bool mct = fAnalysis->McTruthMatch(d0[j]);
		
		TVector3 vtx;
		double qavtx = fVtxPoca->GetPocaVtx(vtx, d0[j]);
	
		nd0->Column("ev", 		(Float_t) i);
		nd0->Column("cand",		(Float_t) j);
		nd0->Column("pdg",		(Float_t) d0[j]->PdgCode());
		nd0->Column("mct",		(Float_t) mct);
		
		nd0->Column("d0m",		(Float_t) d0[j]->M());
		nd0->Column("d0p",		(Float_t) d0[j]->P());
		nd0->Column("d0tht",	(Float_t) d0[j]->P3().Theta());
		nd0->Column("d0phi",	(Float_t) d0[j]->P3().Phi());
		nd0->Column("d0pt",		(Float_t) d0[j]->P3().Pt());
		nd0->Column("d0pcm",	(Float_t) d0cm.P());
		nd0->Column("d0thtcm",	(Float_t) d0cm.Theta());
		nd0->Column("d0miss",	(Float_t) (fIni-(d0[j]->P4())).M());
		
		nd0->Column("d0vx",		(Float_t) vtx.X());
		nd0->Column("d0vy",		(Float_t) vtx.Y());
		nd0->Column("d0vz",		(Float_t) vtx.Z());
		nd0->Column("d0vqa",	(Float_t) qavtx);
		
		nd0->Column("kch",		(Float_t) dka->Charge());
		nd0->Column("kp",		(Float_t) dka->P());
		nd0->Column("ktht",		(Float_t) dka->P3().Theta());
		nd0->Column("kphi",		(Float_t) dka->P3().Phi());
		nd0->Column("kpt",		(Float_t) dka->P3().Pt());
		nd0->Column("kpid",		(Float_t) dka->GetPidInfo(3));
		nd0->Column("kapcm",	(Float_t) kacm.P());
		nd0->Column("kathtcm",	(Float_t) kacm.Theta());
		
		nd0->Column("pich",		(Float_t) dpi->Charge());
		nd0->Column("pip",		(Float_t) dpi->P());
		nd0->Column("pitht",	(Float_t) dpi->P3().Theta());
		nd0->Column("piphi",	(Float_t) dpi->P3().Phi());
		nd0->Column("pit",		(Float_t) dpi->P3().Pt());
		nd0->Column("pipid",	(Float_t) dpi->GetPidInfo(2));
		nd0->Column("pipcm",	(Float_t) picm.P());
		nd0->Column("pithtcm",	(Float_t) picm.Theta());
		
		nd0->DumpData();
	}
		
}


void PndTutAnaTaskD0::Finish()
{
	
	// *******
	// ******* STORE YOUR HISTOS AND TUPLES
	// *******
	nd0->GetInternalTree()->Write();
}

ClassImp(PndTutAnaTaskD0)
