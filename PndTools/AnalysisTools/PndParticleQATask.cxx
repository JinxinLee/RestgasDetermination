// ************************************************************************
//
// psi(2S) -> J/psi (-> mu+ mu-) pi+ pi- Analysis Example Task
// 
// for the Rho Tutorial, see
// http://panda-wiki.gsi.de/cgi-bin/viewauth/Computing/PandaRootRhoTutorial
//
// K.Goetzen 7/2013
// ************************************************************************


// The header file
#include "PndParticleQATask.h"

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
#include "RhoTuple.h"

// Analysis headers
#include "PndAnalysis.h"
#include "Pnd4CFitter.h"
#include "PndKinVtxFitter.h"
#include "PndKinFitter.h"
#include "PndVtxPoca.h"
#include "PndPidCandidate.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndParticleQATask::PndParticleQATask() :
  FairTask("Panda Tutorial Analysis Task") { 
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndParticleQATask::~PndParticleQATask() { }
// -------------------------------------------------------------------------


// ----- Method to select true PID candidates
int PndParticleQATask::SelectTruePid(PndAnalysis *ana, RhoCandList &l)
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
InitStatus PndParticleQATask::Init() 
{		
	// initialize analysis object
	fAnalysis = new PndAnalysis();
	
	// reset the event counter
	fEvtCount = 0;

	// create ntuple
	ntp	 = new RhoTuple("ntp", "Particle QA charged");
	ntpn = new RhoTuple("ntpn","Particle QA neutrals");
		
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndParticleQATask::SetParContainers() 
{
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndParticleQATask::Exec(Option_t* opt)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	
	TLorentzVector dummy;
	
	// necessary to read the next event
	fAnalysis->GetEvent();
	
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	
	// *** RhoCandLists for the analysis
	RhoCandList chr, chr1emc, chr2drc, chr3dsc, chr4mvd, chr5mdt, chr6stt;
	RhoCandList chr16, chr126, chr1256, chr12356, chr1236, chr26, chr123456, chrmva;
	RhoCandList neut;
	
	// *** Select with no PID info ('All'); type and mass are set 		
	fAnalysis->FillList( neut,   "Neutral" );
	
	fAnalysis->FillList( chr,     "Charged" ,"PidAlgoEmcBayes");    // short cut name
	fAnalysis->FillList( chr1emc, "Charged" ,"PidAlgoEmcBayes");    // emc = algo 1
	fAnalysis->FillList( chr2drc, "Charged" ,"PidAlgoDrc");         // drc = algo 2
	fAnalysis->FillList( chr3dsc, "Charged" ,"PidAlgoDisc");        // dsc = algo 3
	fAnalysis->FillList( chr4mvd, "Charged" ,"PidAlgoMvd");         // mvd = algo 4
	fAnalysis->FillList( chr5mdt, "Charged" ,"PidAlgoMdtHardCuts"); // mdt = algo 5
	fAnalysis->FillList( chr6stt, "Charged" ,"PidAlgoStt");         // stt = algo 6
	
	// now the combination of numbers define the algo-combination
	fAnalysis->FillList( chr16,     "Charged" ,"PidAlgoEmcBayes;PidAlgoStt");
	fAnalysis->FillList( chr126,    "Charged" ,"PidAlgoEmcBayes;PidAlgoStt;PidAlgoDrc");
	fAnalysis->FillList( chr1236,   "Charged" ,"PidAlgoEmcBayes;PidAlgoStt;PidAlgoDrc;PidAlgoDisc;");
	fAnalysis->FillList( chr1256,   "Charged" ,"PidAlgoEmcBayes;PidAlgoStt;PidAlgoDrc;PidAlgoMdtHardCuts");
	fAnalysis->FillList( chr12356,  "Charged" ,"PidAlgoEmcBayes;PidAlgoStt;PidAlgoDrc;PidAlgoDisc;PidAlgoMdtHardCuts");
	fAnalysis->FillList( chr123456, "Charged" ,"PidAlgoEmcBayes;PidAlgoStt;PidAlgoDrc;PidAlgoDisc;PidAlgoMdtHardCuts;PidAlgoMvd");
	fAnalysis->FillList( chr26,     "Charged" ,"PidAlgoStt;PidAlgoDrc");
	//fAnalysis->FillList( chrmva, "Charged" ,"TMVABDTMvaProb");
	
	int ntrk = chr.GetLength();
	
	// *** Loop over CHARGED particles
	for (j=0; j<chr.GetLength(); ++j)
	{
		RhoCandidate *truth = chr[j]->GetMcTruth();
		
		ntp->Column("ev", 	(Float_t)  fEvtCount,			0.0f );
		ntp->Column("trk",  (Float_t)  j,     				0.0f );
		ntp->Column("ntrk", (Float_t)  ntrk,     			0.0f );
		ntp->Column("chrg", (Float_t)  chr[j]->Charge(),    0.0f );
		
		qaP4(  "",			chr[j]->P4(),	ntp);
			   
		qaPid( "alg1emc",	chr1emc[j], 		ntp); 
		qaPid( "alg2drc", 	chr2drc[j], 		ntp); 
		qaPid( "alg3dsc", 	chr3dsc[j], 		ntp); 
		qaPid( "alg4mvd", 	chr4mvd[j], 		ntp); 
		qaPid( "alg5mdt", 	chr5mdt[j], 		ntp); 
		qaPid( "alg6stt", 	chr6stt[j], 		ntp); 
		//qaPid( "algmva", 	chrmva[j], 		ntp); 
		
		qaPid( "alg16", 	chr16[j], 		ntp); 
		qaPid( "alg126", 	chr126[j], 		ntp); 
		qaPid( "alg1236", 	chr1236[j], 		ntp); 
		qaPid( "alg1256", 	chr1256[j], 		ntp); 
		qaPid( "alg12356", 	chr12356[j], 		ntp); 
		qaPid( "alg123456", 	chr123456[j], 		ntp); 
		qaPid( "algall", 	chr123456[j], 		ntp); 
		qaPid( "alg26", 	chr26[j], 		ntp); 
		
		qaEmc( "emc",		chr[j], 		ntp);
		qaMvd( "mvd",		chr[j], 		ntp);
		qaStt( "stt",		chr[j], 		ntp);
		qaDrc( "drc",		chr[j], 		ntp);
		qaDsc( "dsc",		chr[j], 		ntp);
		qaTof( "tof",		chr[j], 		ntp);
		qaMuo( "muo",		chr[j], 		ntp);
		qaTrk( "trk",		chr[j], 		ntp);
		qaPid( "pid",		chr[j], 		ntp);
		
		float mct = 0.0;
		float prim = 0.0;
		
		if (truth)
		{
			mct = 1.0;
			RhoCandidate *moth = truth->TheMother();
			if (!moth || abs(moth->PdgCode()-88850)<100 ) prim = 1.0;
			
			qaP4( "tr",				truth->P4(),		ntp);
			ntp->Column("trpdg", 	(Float_t) truth->PdgCode(),		0.0f );
		}
		else
		{
			qaP4( "tr", 			dummy,			ntp, 		true);
			ntp->Column("trpdg", 	(Float_t) 0.,	0.0f );
		}
		
		ntp->Column("prim",  	(Float_t)  prim,    0.0f );
		ntp->Column("mct",  	(Float_t)  mct,    	0.0f );
		
		ntp->DumpData();
	}
	
	ntrk = neut.GetLength();
	// *** Loop over NEUTRAL particles
	for (j=0; j<neut.GetLength(); ++j)
	{
		RhoCandidate *truth = neut[j]->GetMcTruth();
		
		ntpn->Column("ev", 	 (Float_t)  fEvtCount,			0.0f );
		ntpn->Column("ntrk", (Float_t)  ntrk,     			0.0f );
		ntpn->Column("trk",  (Float_t)  j,     				0.0f );
		ntpn->Column("chrg", (Float_t)  neut[j]->Charge(),  0.0f );
		
		qaP4(  "",			neut[j]->P4(),	ntpn);
		qaEmc( "emc",		neut[j], 		ntpn);
		
		float mct = 0.0;
		float prim = 0.0;
		
		if (truth)
		{
			mct = 1.0;
			RhoCandidate *moth = truth->TheMother();
			if (!moth || abs(moth->PdgCode()-88850)<100 ) prim = 1.0;
			
			qaP4( "tr",				truth->P4(),		ntpn);
			ntpn->Column("trpdg", 	(Float_t) truth->PdgCode(),		0.0f );
		}
		else
		{
			qaP4( "tr", 			dummy,			ntpn, 		true);
			ntpn->Column("trpdg", 	(Float_t)  0.,	0.0f );
		}
		
		ntpn->Column("prim",  	(Float_t)  prim,    0.0f );
		ntpn->Column("mct",  	(Float_t)  mct,    	0.0f );
		
		ntpn->DumpData();
	}
}


void PndParticleQATask::Finish()
{	
	ntp->GetInternalTree()->Write();		
	ntpn->GetInternalTree()->Write();	
}
// -------------------------------------------------------------------------

void PndParticleQATask::qaP4(TString pre, TLorentzVector c, RhoTuple *n, bool skip)
{
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
void PndParticleQATask::qaEmc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"eraw",  (Float_t) mic->GetEmcRawEnergy(),			0.0f );
		n->Column(pre+"ecal",  (Float_t) mic->GetEmcCalEnergy(),			0.0f );
		n->Column(pre+"qa",    (Float_t) mic->GetEmcQuality(),   			0.0f );
		n->Column(pre+"nx",    (Float_t) mic->GetEmcNumberOfCrystals(),		0.0f );
		n->Column(pre+"nb",    (Float_t) mic->GetEmcNumberOfBumps(),   		0.0f );
		n->Column(pre+"z20",   (Float_t) mic->GetEmcClusterZ20(),   		0.0f );
		n->Column(pre+"z53",   (Float_t) mic->GetEmcClusterZ53(),   		0.0f );
		n->Column(pre+"lat",   (Float_t) mic->GetEmcClusterLat(),   		0.0f );
		n->Column(pre+"e1",    (Float_t) mic->GetEmcClusterE1(),   			0.0f );
		n->Column(pre+"e9",    (Float_t) mic->GetEmcClusterE9(),   			0.0f );
		n->Column(pre+"e25",   (Float_t) mic->GetEmcClusterE25(),   		0.0f );
		n->Column(pre+"mod",   (Float_t) mic->GetEmcModule(),   			0.0f );
		n->Column(pre+"idx",   (Float_t) mic->GetEmcIndex(),   				0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaMvd(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"dedx",  (Float_t) mic->GetMvdDEDX(),			0.0f );
		n->Column(pre+"hits",  (Float_t) mic->GetMvdHits(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaStt(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"dedx",  (Float_t) mic->GetSttMeanDEDX(),		0.0f );
		n->Column(pre+"hits",  (Float_t) mic->GetSttHits(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaDrc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"thtc",  	(Float_t) mic->GetDrcThetaC(),			0.0f );
		n->Column(pre+"dthtc",  (Float_t) mic->GetDrcThetaCErr(),		0.0f );
		n->Column(pre+"qa",  	(Float_t) mic->GetDrcQuality(),			0.0f );
		n->Column(pre+"nphot",  (Float_t) mic->GetDrcNumberOfPhotons(),	0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetDrcIndex(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaDsc(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"thtc",  	(Float_t) mic->GetDiscThetaC(),			0.0f );
		n->Column(pre+"dthtc",  (Float_t) mic->GetDiscThetaCErr(),		0.0f );
		n->Column(pre+"qa",  	(Float_t) mic->GetDiscQuality(),		0.0f );
		n->Column(pre+"nphot",  (Float_t) mic->GetDiscNumberOfPhotons(),0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetDiscIndex(),			0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaMuo(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"nlay",  	(Float_t) mic->GetMuoNumberOfLayers(),	0.0f );;
		n->Column(pre+"prob",  	(Float_t) mic->GetMuoProbability(),		0.0f );;
		n->Column(pre+"qa",  	(Float_t) mic->GetMuoQuality() ,		0.0f ); ;
		n->Column(pre+"iron",  	(Float_t) mic->GetMuoIron() ,			0.0f );  ;  
		n->Column(pre+"pin",  	(Float_t) mic->GetMuoMomentumIn(),		0.0f );;
		n->Column(pre+"mod", 	(Float_t) mic->GetMuoModule(),			0.0f );  ;  
		n->Column(pre+"hits",  	(Float_t) mic->GetMuoHits(),			0.0f );  ;  
		n->Column(pre+"idx",  	(Float_t) mic->GetMuoIndex(),			0.0f ); ;    
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaTof(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"time", 	(Float_t) mic->GetTofStopTime(),	0.0f );
		n->Column(pre+"m2",  	(Float_t) mic->GetTofM2(),			0.0f );  
		n->Column(pre+"len",  	(Float_t) mic->GetTofTrackLength(),	0.0f );
		n->Column(pre+"qa",  	(Float_t) mic->GetTofQuality(),		0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetTofIndex(),		0.0f ); 
		n->Column(pre+"beta",  	(Float_t) mic->GetTofBeta(),		0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaTrk(TString pre, RhoCandidate *c, RhoTuple *n)
{
	PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	if (mic)
	{
		n->Column(pre+"dof",  	(Float_t) mic->GetDegreesOfFreedom(),	0.0f );
		n->Column(pre+"stat",  	(Float_t) mic->GetFitStatus(),			0.0f ); 
		n->Column(pre+"chi2",  	(Float_t) mic->GetChiSquared(),			0.0f );
		n->Column(pre+"idx",  	(Float_t) mic->GetTrackIndex(),			0.0f );	
		n->Column(pre+"branch",	(Float_t) mic->GetTrackBranch() ,		0.0f );
	}		
}

// -------------------------------------------------------------------------

void PndParticleQATask::qaPid(TString pre, RhoCandidate *c, RhoTuple *n)
{	
	n->Column(pre+"e",  (Float_t) c->GetPidInfo(0),		0.0f );
	n->Column(pre+"mu", (Float_t) c->GetPidInfo(1),		0.0f ); 
	n->Column(pre+"pi", (Float_t) c->GetPidInfo(2),		0.0f );
	n->Column(pre+"k",  (Float_t) c->GetPidInfo(3),		0.0f );	
	n->Column(pre+"p",	(Float_t) c->GetPidInfo(4),		0.0f );
}

// -------------------------------------------------------------------------

ClassImp(PndParticleQATask)
