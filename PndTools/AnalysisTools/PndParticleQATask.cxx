// ************************************************************************
//
// QA Task for single particles
// 
// K.Goetzen 7/2013
// last change: K.Goetzen 3/2016
// ************************************************************************


// The header file
#include "PndParticleQATask.h"

// C++ headers

// FAIR headers

// ROOT headers
#include "TString.h"

// RHO headers
#include "RhoCandidate.h"
#include "RhoTuple.h"

// Analysis headers
#include "PndAnalysis.h"
#include "PndRhoTupleQA.h"		
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndParticleQATask::PndParticleQATask(bool fastsim) :
  FairTask("Panda Tutorial Analysis Task") { 
	  fFastSim = fastsim;
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
	nmc	 = new RhoTuple("nmc", "Particle QA MC truth");
	ntp	 = new RhoTuple("ntp", "Particle QA charged");
	ntpn = new RhoTuple("ntpn","Particle QA neutrals");
	
	if (fFastSim)
	{
		// individual detectors/ algos
		//fPid[1] = "ScEmcPidBarrelProbability;ScEmcPidFwCapProbability;ScEmcPidBwCapProbability"; 
		//fPid[5] = "DrcBarrelProbability";	
		//fPid[6] = "DrcDiscProbability";	
		//fPid[4] = "MvdPidProbability";		
		//fPid[2] = "ScMdtPidBarrelProbability;ScMdtPidForwardProbability";							
		//fPid[3] = "SttPidProbability";	
		
		// KG 03/2016: Changed ordering like in FullSim (don't know why it was ordered differently by me beforehand...)
		
		fPid[1] = "ScEmcPidBarrelProbability;ScEmcPidFwCapProbability;ScEmcPidBwCapProbability"; 
		fPid[2] = "DrcBarrelProbability";	
		fPid[3] = "DrcDiscProbability";	
		fPid[4] = "MvdPidProbability";		
		fPid[5] = "ScMdtPidBarrelProbability;ScMdtPidForwardProbability";							
		fPid[6] = "SttPidProbability";	
		fPid[7] = "RichProbability";
		
		fPid[9] = "IdealPidProbability";

		// total PID	
		fPid[0] = "PidChargedProbability";												
		// control: pid8 should be  = pid0
		fPid[8] = fPid[1];
		for (int i=2;i<8;++i) fPid[8] += ";"+fPid[i];
	}
	else
	{
		// individual detectors/ algos
		fPid[1] = "PidAlgoEmcBayes";
		fPid[2] = "PidAlgoDrc";
		fPid[3] = "PidAlgoDisc";
		fPid[4] = "PidAlgoMvd";
		fPid[5] = "PidAlgoMdtHardCuts";
		fPid[6] = "PidAlgoStt";
		fPid[7] = "PidAlgoIdealCharged";  // only 6 algos available in full sim

		fPid[9] = "PidAlgoIdealCharged";

		// total pid
		fPid[0] = fPid[1];
		for (int i=2;i<7;++i) fPid[0] += ";"+fPid[i];
		fPid[8] = fPid[0];
	}
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndParticleQATask::SetParContainers() 
{
  // Get run and runtime database
  //FairRun* run = FairRun::Instance();
  //if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndParticleQATask::Exec(Option_t* opt)
{
	// *** some variables
	//int i=0,j=0, k=0, l=0;
	int j=0;
	TLorentzVector dummy;
	
	// necessary to read the next event
	fAnalysis->GetEventInTask();
	
	if (!(++fEvtCount%100)) cout << "[PndParticleQATask] evt "<<fEvtCount<<endl;
	
	// *** RhoCandLists for the analysis
	RhoCandList chr, chr1emc, chr2drc, chr3dsc, chr4mvd, chr5mdt, chr6stt, chr7rch, chr8chk, chr9idl;
	RhoCandList chr1nemc, chr2ndrc, chr3ndsc, chr4nmvd, chr5nmdt, chr6nstt, chr7nrch;
	//RhoCandList chr16, chr126, chr1256, chr12356, chr1236, chr26, chr123456, chr1234567;
	RhoCandList neut, mclist;
	
	PndRhoTupleQA qa(fAnalysis,15.0);
	
	// *** store MC truth info
	fAnalysis->FillList(mclist,   "McTruth",50);
	nmc->Column("ev", (Int_t) fEvtCount);
	qa.qaMcList("",   mclist, nmc);
	nmc->DumpData();
	
	// *** Select with no PID info ('All'); type and mass are set 		
	fAnalysis->FillList( neut,    "Neutral" );
	
	// *** charged lists with different PID algo combinations
	// *** the individual PID probs
	fAnalysis->FillList( chr,     "Charged" , fPid[0]);		// total pid
	
	fAnalysis->FillList( chr1emc, "Charged" , fPid[1]);		// emc = algo 1
	fAnalysis->FillList( chr2drc, "Charged" , fPid[2]);		// drc = algo 2
	fAnalysis->FillList( chr3dsc, "Charged" , fPid[3]);		// dsc = algo 3
	fAnalysis->FillList( chr4mvd, "Charged" , fPid[4]);		// mvd = algo 4
	fAnalysis->FillList( chr5mdt, "Charged" , fPid[5]); 	// mdt = algo 5
	fAnalysis->FillList( chr6stt, "Charged" , fPid[6]);		// stt = algo 6
	if (fFastSim) fAnalysis->FillList( chr7rch, "Charged" , fPid[7]);		// rich = algo 7
	
	fAnalysis->FillList( chr8chk, "Charged" , fPid[8]);		// check for total PID (only useful for FastSim)
	fAnalysis->FillList( chr9idl, "Charged" , fPid[9]);		// idl = ideal PID

	
	
	// *** combinations without a certain PID algo
	if (fFastSim)
	{
		fAnalysis->FillList( chr1nemc, "Charged" , fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]+";"+fPid[7]);		// w/o emc = no algo 1
		fAnalysis->FillList( chr2ndrc, "Charged" , fPid[1]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]+";"+fPid[7]);		// w/o drc = no algo 2
		fAnalysis->FillList( chr3ndsc, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]+";"+fPid[7]);		// w/o dsc = no algo 3
		fAnalysis->FillList( chr4nmvd, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[5]+";"+fPid[6]+";"+fPid[7]);		// w/o mvd = no algo 4
		fAnalysis->FillList( chr5nmdt, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[6]+";"+fPid[7]); 	// w/o mdt = no algo 5
		fAnalysis->FillList( chr6nstt, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]+";"+fPid[7]);		// w/o stt = no algo 6
		fAnalysis->FillList( chr7nrch, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]);		// w/o rich = no algo 7
	}
	else
	{
		fAnalysis->FillList( chr1nemc, "Charged" , fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]);	// w/o emc = no algo 1
		fAnalysis->FillList( chr2ndrc, "Charged" , fPid[1]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]);	// w/o drc = no algo 2
		fAnalysis->FillList( chr3ndsc, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]);	// w/o dsc = no algo 3
		fAnalysis->FillList( chr4nmvd, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[5]+";"+fPid[6]);	// w/o mvd = no algo 4
		fAnalysis->FillList( chr5nmdt, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[6]); 	// w/o mdt = no algo 5
		fAnalysis->FillList( chr6nstt, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]);	// w/o stt = no algo 6
	}
	
	
	// now the combination of numbers define the algo-combination
	//fAnalysis->FillList( chr16,     "Charged" , fPid[1]+";"+fPid[6] );
	//fAnalysis->FillList( chr126,    "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[6] );
	//fAnalysis->FillList( chr1236,   "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[6] );
	//fAnalysis->FillList( chr1256,   "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[5]+";"+fPid[6]);
	//fAnalysis->FillList( chr12356,  "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[5]+";"+fPid[6]);
	//fAnalysis->FillList( chr123456, "Charged" , fPid[1]+";"+fPid[2]+";"+fPid[3]+";"+fPid[4]+";"+fPid[5]+";"+fPid[6]);
	//fAnalysis->FillList( chr26,     "Charged" , fPid[2]+";"+fPid[6]);
	//fAnalysis->FillList( chrmva, "Charged" ,"TMVABDTMvaProb");
	
	int ntrk = chr.GetLength();
	
	TLorentzVector chrgP4=dummy;
	
	// *** Loop over CHARGED particles
	for (j=0; j<chr.GetLength(); ++j)
	{
		RhoCandidate *truth = chr[j]->GetMcTruth();
		
		ntp->Column("ev", 	(Float_t)  fEvtCount,			0.0f );
		ntp->Column("trk",  (Float_t)  j,     				0.0f );
		ntp->Column("ntrk", (Float_t)  ntrk,     			0.0f );
		ntp->Column("chrg", (Float_t)  chr[j]->Charge(),    0.0f );
		
		qa.qaP4(  "",		chr[j]->P4(),		ntp);
			   
		qa.qaPid( "pid",	chr[j], 		ntp);
		
		qa.qaPid( "pid1",	chr1emc[j], 		ntp); 
		qa.qaPid( "pid2", 	chr2drc[j], 		ntp); 
		qa.qaPid( "pid3", 	chr3dsc[j], 		ntp); 
		qa.qaPid( "pid4", 	chr4mvd[j], 		ntp); 
		qa.qaPid( "pid5", 	chr5mdt[j], 		ntp); 
		qa.qaPid( "pid6", 	chr6stt[j], 		ntp); 
		if (fFastSim) 
		  qa.qaPid( "pid7", 	chr7rch[j], 		ntp); 
		
		qa.qaPid( "pidchk",	chr8chk[j], 		ntp); 
		qa.qaPid( "pidid", 	chr9idl[j], 		ntp); 
		
		qa.qaPid( "pidn1",	chr1nemc[j], 		ntp); 
		qa.qaPid( "pidn2", 	chr2ndrc[j], 		ntp); 
		qa.qaPid( "pidn3", 	chr3ndsc[j], 		ntp); 
		qa.qaPid( "pidn4", 	chr4nmvd[j], 		ntp); 
		qa.qaPid( "pidn5", 	chr5nmdt[j], 		ntp); 
		qa.qaPid( "pidn6", 	chr6nstt[j], 		ntp); 
		if (fFastSim) 
		  qa.qaPid( "pidn7", 	chr7nrch[j], 		ntp); 
		
		//qa.qaPid( "algmva", 	chrmva[j], 		ntp); 
		
		//qa.qaPid( "pid16", 	chr16[j], 		ntp); 
		//qa.qaPid( "pid126", 	chr126[j], 		ntp); 
		//qa.qaPid( "pid1236", 	chr1236[j], 		ntp); 
		//qa.qaPid( "pid1256", 	chr1256[j], 		ntp); 
		//qa.qaPid( "pid12356", 	chr12356[j], 		ntp); 
		//qa.qaPid( "pid123456", chr123456[j], 		ntp); 
		//qa.qaPid( "pid26", 	chr26[j], 		ntp); 
		
		qa.qaEmc( "",		chr[j], 		ntp);
		qa.qaMvd( "",		chr[j], 		ntp);
		qa.qaStt( "",		chr[j], 		ntp);
		qa.qaDrc( "",		chr[j], 		ntp);
		qa.qaDsc( "",		chr[j], 		ntp);
		qa.qaTof( "",		chr[j], 		ntp);
		qa.qaMuo( "",		chr[j], 		ntp);
		qa.qaTrk( "",		chr[j], 		ntp);
		
		float mct = 0.0;
		float prim = 0.0;
		
		if (truth)
		{
			mct = 1.0;
			RhoCandidate *moth = truth->TheMother();
			if (!moth || abs(moth->PdgCode()-88850)<100 ) 
			{
				prim = 1.0;
				chrgP4 = truth->P4();
			}
			
			qa.qaP4( "tr",				chrgP4,		ntp);
			ntp->Column("trpdg", 	(Float_t) truth->PdgCode(),		0.0f );
		}
		else
		{
			qa.qaP4( "tr", 			dummy,			ntp, 		true);
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
		
		qa.qaP4(  "",			neut[j]->P4(),	ntpn);
		qa.qaEmc( "",		neut[j], 		ntpn);
		qa.qaP4(  "primlv",     chrgP4,  ntpn);
		
		float mct = 0.0;
		float prim = 0.0;
		
		if (truth)
		{
			mct = 1.0;
			RhoCandidate *moth = truth->TheMother();
			if (!moth || abs(moth->PdgCode()-88850)<100 ) prim = 1.0;
			if (moth) 
			{
				qa.qaP4( "moth", moth->P4(), ntpn);
				ntpn->Column("mothpdg", (Float_t) moth->PdgCode(), -999.f);
			}
			else
			{
				qa.qaP4( "moth", dummy, ntpn, true);
				ntpn->Column("mothpdg", (Float_t) -1., -999.f);
			}
			
			qa.qaP4( "tr",				truth->P4(),		ntpn);
			ntpn->Column("trpdg", 	(Float_t) truth->PdgCode(),		0.0f );
		}
		else
		{
			qa.qaP4( "moth", 		dummy, 			ntpn, 		true);
			qa.qaP4( "tr", 			dummy,			ntpn, 		true);
			ntpn->Column("trpdg", 	(Float_t)  0.,	0.0f );
			ntpn->Column("mothpdg", (Float_t) -1, -999.f);
		}
		
		ntpn->Column("prim",  	(Float_t)  prim,    0.0f );
		ntpn->Column("mct",  	(Float_t)  mct,    	0.0f );
		
		ntpn->DumpData();
	}
}


void PndParticleQATask::Finish()
{	
	nmc->GetInternalTree()->Write();
	ntp->GetInternalTree()->Write();		
	ntpn->GetInternalTree()->Write();	
}

ClassImp(PndParticleQATask)
