// ************************************************************************
//
// Online SoftTrigger reco and tagging for several channels 
//
// K.Goetzen 10/2013
// ************************************************************************


// The header file
#include "PndSoftTriggerTask.h"

// Package headers
#include "PndOnlineFilterInfo.h"
#include "PndSoftTriggerLine.h"

// C++ headers
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdlib.h>     /* getenv */

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
#include "TString.h"
#include "TRegexp.h"
#include "TMVA/Reader.h"

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
#include "Pnd4CFitter.h"
#include "PndKinVtxFitter.h"
#include "PndVtxPRG.h"
#include "PndKinFitter.h"
#include "PndVtxPoca.h"
#include "PndPidCandidate.h"
#include "PndEventShape.h"
#include "PndRhoTupleQA.h"
		
const int STMAXCUT  = 50;
const int STMAXEVVARS  = 50;

using std::cout;
using std::endl;

// *** Holds the cut set for a certain mode
struct STCutSet
{
	int    ncut;
	int    varid[STMAXCUT];
	int    op[STMAXCUT];
	double cutval[STMAXCUT];
	
	TMVA::Reader *tmvaread;
	int    tmvanvar;
	int    tmvavarid[STMAXCUT];
	float  tmvacut;
	TString tmvameth;
};

// *** this is the array of input variables for the cut selection or TMVA
float fSTInputValues[STMAXCUT];

// *** maps needed for selection parsing
std::map<TString, int> fSTVarmap;
std::map<int, STCutSet> fSTSelmap;
//std::map<int, TString> fSTOps;
TString fSTOps[5]={">=",">","==","<=","<"};
std::map<int, PndSoftTriggerLine*> fSTTriggers;
std::map<int, int> fSTMapListIndex;

typedef std::map<int, PndSoftTriggerLine*>::iterator TrigIt;

//                            e+  e-  mu+  mu- pi+   pi-  K+    K-    p     pb   gam  pi0  KS   eta   aux  anti-aux
int fSTPidIndex[16] =       {-11, 11, -13, 13, 211, -211, 321, -321, 2212, -2212, 22, 111, 310, 221,   0,   0};

// *** Event Shape Vars

// (  0) eslnpide        (  1) eslnpidmu       (  2) eslnpidpi       (  3) eslnpidk        (  4) eslnpidp           
// (  5) esfw1           (  6) esfw2           (  7) esfw3           (  8) esfw4           (  9) esfw5
// ( 10) esapl           ( 11) escir           ( 12) espla           ( 13) esnneut         ( 14) esnpart      
// ( 15) esnchrg         ( 16) espmax          ( 17) espmaxl         ( 18) espmin          ( 19) espminl       
// ( 20) esprapmax       ( 21) esptmax         ( 22) essumen         ( 23) essumenl        ( 24) essumetn       
// ( 25) essumpc         ( 26) essumpc05       ( 27) essumpcl        ( 28) essumpt         ( 29) essumptc    
// ( 30) esthr           ( 31) essph           ( 32) esptmin         ( 33) esncp10l        ( 34) esnne10l   

int   fSTNEvVars = 35;
TString fSTenames[] = {
    "eslnpide"     , "eslnpidmu"    , "eslnpidpi"    , "eslnpidk"     , "eslnpidp"     , 
    "esfw1"        , "esfw2"        , "esfw3"        , "esfw4"        , "esfw5"        , 
    "esapl"        , "escir"        , "espla"        , "esnneut"      , "esnpart"      , 
    "esnchrg"      , "espmax"       , "espmaxl"      , "espmin"       , "espminl"      , 
    "esprapmax"    , "esptmax"      , "essumen"      , "essumenl"     , "essumetn"     , 
    "essumpc"      , "essumpc05"    , "essumpcl"     , "essumpt"      , "essumptc"     , 
    "esthr"        , "essph"        , "esptmin"      , "esncp10l"     , "esnne10l"
};

// coding for variable suffix (to translate variable names to code)
int   fSTNQuant = 17;
TString fSTVarSuff[] = {"pt","tht","pcm","thtcm","pide","pidmu","pidpi","pidk","pidp", "pidmax","pocqa","pocdist","pocctau","oang","cdecang","decang","p" };
int   fSTQuantCode[] = { 11,  12,   13,   14,      20,    21,     22,     23,    24,     25,     30,     31,       32,      40,     42,      41,      10 };


// cache for event shape variables
double fSTVarEvArray[STMAXEVVARS];

// cache map for candidate variables
std::map<int, double> fSTVarCandArray;

// codes for the available energies
std::vector<int> fSTencode;
int fSTModeIndex = 0;

// -----   Default constructor   -------------------------------------------
PndSoftTriggerTask::PndSoftTriggerTask(double pmom, int mode, int runnum, TString trigfilename) :
	FairTask("Panda Softtrigger Task"),
	fVerbose(0), fMode(mode), fEvtCount(0), fRunNum(runnum), fSigCount(0), fNsigTag(8.0), fNsigAux(3.0), fDstMDiffCut(10.),
	fTriggerFileName(trigfilename), fPhotosMax(0), fPhotosThresh(0.05), 
	fIniP4(0,0,0,0), fEcm(0.), fPbarMom(pmom),
	fQAPi0(false),fQAEta(false),fQAKs0(false),fQAEvent(false), fQAMc(false), fQAMctOnly(false),
	fGammaMinE(0.03), fPi0MinE(0.0), fEtaMinE(0.0), fTrackMinP(0.15), fIniPidCut(0.0),
	fEventShape(NULL), 
	fQA(NULL),
	fPi0Sel(NULL), fEtaSel(NULL), fKs0Sel(NULL),
	fMomentumSel(NULL), fEnergySel(NULL),
	ntp(0),	nks0(0), npi0(0), neta(0), nmc(0)	
{
	fPdg = TDatabasePDG::Instance();
	
	// *** add several pbar p/n/dd Systems for MC truth match
	double ppwidth = 0.01;
	fPdg->AddParticle("pbarpSystem", "pbar p", fEcm, false, ppwidth,0,"",88888);
	fPdg->AddParticle("pbarpSystem0","pbar p", fEcm, false, ppwidth,0,"",88880);
	fPdg->AddParticle("pbarpSystem1","pbar p", fEcm, false, ppwidth,0,"",88881);
	fPdg->AddParticle("pbarpSystem2","pbar p", fEcm, false, ppwidth,0,"",88882);
	
	fPdg->AddParticle("pbarnSystem", "pbar n", fEcm, false, ppwidth,0,"",88887);
	fPdg->AddParticle("pbardSystem", "pbar d", fEcm, false, ppwidth,0,"",88889);
	
	double mp = fPdg->GetParticle("proton")->Mass(); //Proton mass for computation of p4_ini
	
	// set 4-vector of pbar-p-system
	fIniP4.SetPz(pmom);
	fIniP4.SetE(sqrt(pmom*pmom+mp*mp)+mp);
	fEcm = fIniP4.M();
	
	// set default algorithms for pid
	fAlgoElectron	= "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts";
	fAlgoMuon		= fAlgoElectron;
	fAlgoPion		= fAlgoElectron; 
	fAlgoKaon		= fAlgoElectron;
	fAlgoProton		= fAlgoElectron; 
	
	fIniPidCut		= 0.0; 
	
	fCfgFileName    = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/selection_10ch_tight.cfg";
	fApplyFullSelection = false;
	
	// *** set default signal parameters (mean, sigma)
	SetSignalParamsDefaults();
	SetQASelectionDefaults();	
	
	// if no trigger definition file is given, use the default one
	if (fTriggerFileName.Length()==0)
	{
		fTriggerFileName = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/triggerlines.cfg";
		cout <<"Reading default trigger lines file "<<fTriggerFileName.Data()<<endl;
	}	
	// map pdg codes to RhoCandList index
	for (int i=0;i<14;++i) fSTMapListIndex[fSTPidIndex[i]] = i;
	
	ReadTriggerLines();
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSoftTriggerTask::~PndSoftTriggerTask() 
{
	delete fPocaVertexer;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSoftTriggerTask::Init() 
{		
	fRootManager = FairRootManager::Instance();
	
	// Register TCA for tagging info
    fTcaOnlineFilterInfo = new TClonesArray ( "PndOnlineFilterInfo" );
	if (fRootManager) 
		fRootManager->Register ( "OnlineFilterInfo","PndOnlineFolder", fTcaOnlineFilterInfo, kTRUE );
	
	// *** initialize analysis object
	fAnalysis = new PndAnalysis();
	if (fPhotosMax) fAnalysis->McMatchAllowPhotos(fPhotosMax,fPhotosThresh);
	
	// *** RhoTuple QA helper
	fQA = new PndRhoTupleQA(fAnalysis,fPbarMom);
	
	// *** Save current gDirectory
	TDirectory *dir = gDirectory;
	fRootManager->GetOutFile()->cd();
	
	// *** create ntuple
	if (fQAEvent) { ntp = new RhoTuple("ntpev","Soft Trigger Common"); ntp->GetInternalTree()->SetDirectory(gDirectory);}
	if (fQAKs0)   { nks0 = new RhoTuple("nks0","K_S -> pi+ pi-"); nks0->GetInternalTree()->SetDirectory(gDirectory);}
	if (fQAPi0)   { npi0 = new RhoTuple("npi0","pi0 -> gamma gamma"); npi0->GetInternalTree()->SetDirectory(gDirectory);}
	if (fQAEta)   { neta = new RhoTuple("neta","eta -> gamma gamma"); neta->GetInternalTree()->SetDirectory(gDirectory);}
	if (fQAMc)    { nmc  = new RhoTuple("nmc", "MC info"); nmc->GetInternalTree()->SetDirectory(gDirectory);}
	
	
	// *** create mass pre selectors for QA (formular takes into account RhoSelector definition mean +- win/2
	fPi0PreSel   = new RhoMassParticleSelector("pi0PreSel",  (fPi0QaMax + fPi0QaMin)/2.0, 	fPi0QaMax - fPi0QaMin );  
	fEtaPreSel   = new RhoMassParticleSelector("etaPreSel",  (fEtaQaMax + fEtaQaMin)/2.0, 	fEtaQaMax - fEtaQaMin ); 
	fKs0PreSel   = new RhoMassParticleSelector("Ks0PreSel",  (fKs0QaMax + fKs0QaMin)/2.0, 	fKs0QaMax - fKs0QaMin);  	
	
	// *** number of sigmas deviation for tag
	//double tagNumSig   = fNsigTag;

	// *** create final selectors for pi0, eta, KS 
	fPi0Sel     = new RhoMassParticleSelector("pi0Sel",   fPi0Mean, fPi0Sigma*2.0*fNsigAux);
	fEtaSel     = new RhoMassParticleSelector("etaSel",   fEtaMean, fEtaSigma*2.0*fNsigAux);
	fKs0Sel     = new RhoMassParticleSelector("Ks0Sel",   fKs0Mean, fKs0Sigma*2.0*fNsigAux);

	// *** basic selectors for preselection
	fMomentumSel = new RhoMomentumParticleSelector("PSel",50.+fTrackMinP,100.);
	fEnergySel   = new RhoEnergyParticleSelector("ESel",50.+fGammaMinE,100.);
	
	// *** the poca vertexer
	fPocaVertexer = new PndVtxPoca();
		
	// *** read selection from configuration file
	fSTencode.clear();
	if (fCfgFileName!="" && fApplyFullSelection) ReadConfiguration();

	// *** set mode index for current beam momentum
	fSTModeIndex = 0;
	double diff = 1000.;
	
	for (int i=0; i<fSTencode.size(); ++i) 
	{
		double en = (double)fSTencode[i]/100.;
		if (fabs(fEcm-en)<diff)
		{
			diff=fabs(fEcm-en);
			fSTModeIndex = i;
		}
	}
	
	if (fApplyFullSelection && diff>1.0) cout <<"[PndSoftTriggerTask::Init] **** WARNING: Energy of best matching selection differs "<<diff<<" GeV from current energy!"<<endl;

	// *** initialize triggers
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
	{
		PndSoftTriggerLine *tl = it->second;
		
		if (tl->GetThreshold()<=fEcm)
		{
			tl->Init();
			if (tl->GetRhoTuple()) tl->GetRhoTuple()->GetInternalTree()->SetDirectory(gDirectory);
			
			if (fVerbose>0)
			{
				cout <<"*** MODE: "<<it->first<<endl;
				tl->Print();
				cout <<endl<<endl;
			}
		}
	}
	
	// *** Resore original gDirectory
	dir->cd();

	if (fVerbose>0)
	{
		// print selection setup
		cout <<"[PndSoftTriggerTask] **** Selection setup:"<<endl;
		for (std::map<int,STCutSet>::iterator it=fSTSelmap.begin(); it!=fSTSelmap.end(); ++it)
		{
			std::cout << it->first << " => ";
			STCutSet cs = it->second;
			for (int i=0;i<cs.ncut;++i) cout <<"v["<<cs.varid[i]<<"]"<<fSTOps[cs.op[i]]<<cs.cutval[i]<<"  ";
			cout <<endl;
			if (cs.tmvaread)
			{
				cout<<"  TMVA cut:"<<cs.tmvacut<<"  vars:";
				for (int i=0;i<cs.tmvanvar;++i) cout <<cs.tmvavarid[i]<<" ";
				cout<< endl;
			}
		}
	}

	return kSUCCESS;
}

// ----Defaul parameters for QA--------------------------------------------------------------
void PndSoftTriggerTask::SetQASelectionDefaults()
{
	double Pi0Mass = DbMass("pi0");
	double EtaMass = DbMass("eta");
	double Ks0Mass = DbMass("K_S0");
		
	// default windows are roughly +-15*sigma, and 15*sigma_max, if different channels are reconstructed
	// should not depend explicitly from set mean and sigma values, therefore fixed values are chosen
	
	fPi0QaMin = Pi0Mass - 0.06;		
	fPi0QaMax = Pi0Mass + 0.06;		
	
	fEtaQaMin = EtaMass - 0.10;		
	fEtaQaMax = EtaMass + 0.10;		
	
	fKs0QaMin = Ks0Mass - 0.10;		
	fKs0QaMax = Ks0Mass + 0.10;		
	
}

// ----Defaul signal parameters --------------------------------------------------------------
void PndSoftTriggerTask::SetSignalParamsDefaults()
{
	// *** fitted peak values from previous studies
		
	fPi0Mean = 0.136;		// mean value for pi0 signal
	fPi0Sigma = 0.0045;		// sigma value for pi0 signal
	
	fEtaMean = 0.552;		// mean value for eta(gg) signal
	fEtaSigma = 0.009;		// sigma value for eta(gg) signal
	
	fKs0Mean = 0.497;		// mean value for Ks signal
	fKs0Sigma = 0.008;		// sigma value for Ks signal
	
}	

// ----Set all PID algos at once --------------------------------------------------------------
void PndSoftTriggerTask::SetPidAlgoAll(TString algo)
{
	SetPidAlgoElectron(algo);
	SetPidAlgoMuon(algo);
	SetPidAlgoPion(algo); 
	SetPidAlgoKaon(algo);
	SetPidAlgoProton(algo);
}


// ----Method to enable/disable QA for single mode --------------------------------------------------
void PndSoftTriggerTask::SetQAMode(int mode, bool qa)
{
	int divi = 1;
	if (mode<10) divi=100;
	else if (mode<100) divi=10;
	
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
		if (it->first/divi == mode) it->second->SetWriteQA(qa);
		
//	if (fSTTriggers.find(mode) == fSTTriggers.end()) return;	
//	fSTTriggers[mode]->SetWriteQA(qa);
}


// ----Method to enable/disable full QA--------------------------------------------------------------
void PndSoftTriggerTask::SetQAAll(bool qa)
{	
	SetQAPi0(qa);
	SetQAEta(qa);
	SetQAKs0(qa);
	SetQAEvent(qa);
	SetQAMc(qa);
	
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it) 
		SetQAMode(it->first, qa);
}

// ----Method to enable/disable tagging for single mode --------------------------------------------------
void PndSoftTriggerTask::SetTagMode(int mode, bool tag)
{
	int divi = 1;
	if (mode<10) divi=100;
	else if (mode<100) divi=10;
	
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
		if (it->first/divi == mode) it->second->SetTagActive(tag);
		
//	if (fSTTriggers.find(mode) == fSTTriggers.end()) return;
//	fSTTriggers[mode]->SetTagActive(tag);
}

// ----Method to enable/disable full Tagging--------------------------------------------------------------
void PndSoftTriggerTask::SetTagAll(bool tag)
{	
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
		it->second->SetTagActive(tag);
}


// ----Method to set mass selection n_sigmas for 'mode' --------------------------------------------------
void PndSoftTriggerTask::SetTagNSigMode(int mode, double nsig)
{
	if (fSTTriggers.find(mode) == fSTTriggers.end()) return;
	
	fSTTriggers[mode]->SetTagNSig(nsig);
}


// ----Method to set mass selection n_sigmas for all modes -----------------------------------------------
void PndSoftTriggerTask::SetTagNSigAll(double nsig)
{
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
		it->second->SetTagNSig(nsig);	
}


// ----- Method to select true PID candidates
int PndSoftTriggerTask::SelectTruePid(RhoCandList &l)
{
	int removed = 0;
	
	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if ( !(fAnalysis->McTruthMatch(l[ii])) )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	
	return removed;
}
// -------------------------------------------------------------------------

// ----- Method to select candidate with certain PID probability
int PndSoftTriggerTask::SelectPidProb(RhoCandList &l, int pididx, double cut)
{
	int removed = 0;
	
	if (pididx>=0 && pididx<5)
		for (int ii=l.GetLength()-1;ii>=0;--ii)
			if (l[ii]->GetPidInfo(pididx)<cut )
			{
				l.Remove(l[ii]);
				removed++;
			}
	
	return removed;
}
// -------------------------------------------------------------------------

// ----- Method to count candidates with certain PID probability
int PndSoftTriggerTask::MultPidProb(RhoCandList &l, int pididx, double prob)
{
	int mult = 0;
	
	if (pididx>=0 && pididx<5)
		for (int ii=0;ii<l.GetLength();++ii)
			if (l[ii]->GetPidInfo(pididx)>=prob) ++mult;
				
	return mult;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
// reads in trigger line definitions
// defaults are in $VMCWORKDIR/softrig/triggerlines.cfg
bool PndSoftTriggerTask::ReadTriggerLines()
{
	ifstream file(fTriggerFileName.Data());
	if (!file.is_open()) 
	{
		cout <<"[PndSoftTriggerTask] **** Unable to open trigger file "<<fTriggerFileName.Data()<<endl;
		fApplyFullSelection=false;
	}
	else cout <<"[PndSoftTriggerTask] **** Reading trigger lines from "<<fTriggerFileName.Data()<<endl;
	
	char line[500];
	TString toks[13];
	
	// loop through file line by line
	while (!file.eof())
	{
		file.getline(line,499);
		TString sline(line);

		// empty lines or comments are skipped (a comment has to have a # as _first_ char)
		if (sline.BeginsWith("#")||sline=="") continue;	 
	
		// split the line into tokens, has to be
		// 0-mode, 1-name, 2-decay pattern, 3-prefix, 4-ntpname, 5-qamin, 6-qamax, 7-mean, 8-sigma, 9-threshold, 10-nsig, 11-fqa, 12-ftag
		int N = SplitString(sline, ":", toks,13);
		if (N!=13)  {cout <<"[PndSoftTriggerTask] **** Invalid trigger line: "<<sline.Data()<<endl; continue;}
		//if (fVerbose>1) for (int i=0;i<N;++i) cout <<toks[i]<<":"; cout <<endl; 
		
		int mode = toks[0].Atoi();
		PndSoftTriggerLine *tl = new PndSoftTriggerLine(mode, toks[1], toks[2], toks[3], toks[4]);

		double mean=0, qamin = toks[5].Atof(), qamax = toks[6].Atof();
		
		// check whether mean = Ecm
		if (toks[7]=="Ecm") 
			mean = fEcm;
		else 
			mean = toks[7].Atof();
		
		// check whether qa mass window values are relative to mean 
		if (toks[5].BeginsWith("-")) qamin = mean + toks[5].Atof(); // sum, since toks[5].Atof()<0 due to '-' sign
		if (toks[6].BeginsWith("+")) qamax = mean + toks[6].Atof();
		
		tl->SetQAMassWindow(qamin, qamax);
		tl->SetMeanSigma(mean, toks[8].Atof());
		tl->SetThreshold(toks[9].Atof());
		tl->SetTagNSig(toks[10].Atof());
		tl->SetWriteQA(toks[11].Atoi());
		tl->SetTagActive(toks[12].Atoi());
		
		fSTTriggers[mode] = tl;
	}
	
	cout << "[PndSoftTriggerTask] **** Found "<<fSTTriggers.size()<<" trigger definitions: ";
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
		cout << it->second->GetName()<<" ";
	cout <<endl;
	
}


// -------------------------------------------------------------------------
// reads in specific trigger selections for certain modes and energies
// example selection (from ST report - full sim) can be found in the 2 files
// $VMCWORKDIR/softrig/selection_10ch_tight.cfg (suppression of 1/1000)
// $VMCWORKDIR/softrig/selection_10ch_loose.cfg (eff of 95%)
bool PndSoftTriggerTask::ReadConfiguration()
{
	for (int i=0;i<fSTNEvVars;++i) fSTVarmap[fSTenames[i]] = i;

	ifstream file(fCfgFileName.Data());
	
	if (!file.is_open()) 
	{
		cout <<"[PndSoftTriggerTask] **** Unable to open selection file "<<fCfgFileName.Data()<<endl;
		fApplyFullSelection=false;
	}
	else cout <<"[PndSoftTriggerTask] **** Reading selection from "<<fCfgFileName.Data()<<endl;
	
	char line[1500];
	
	TString toks[30];
	TString cuts[30];
	TString toks2[30];

	// loop through file line by line
	while (!file.eof())
	{
		file.getline(line,1499);
		TString sline(line);
		
		// remove tabs everywhere
		sline.ReplaceAll("\t","");

		// if lines contains a '#' somewhere, cut string from that position (=comment)
		if (sline.Contains("#")) sline = sline(0,sline.Index("#")-1);
		
		// remove whitespace at begin and end
		sline = sline.Strip(TString::kBoth);
		
		if (sline=="") continue;
		
		// split the line into tokens; token 0 ist the mode code, token 1 is the complete cut string
		// e.g. '38400 : eslnpidp>0&&abs(pcm-2.105)<0.695&&esthr>0.9&&pt>0.8'
		int N1 = SplitString(sline, ":", toks,30);
		
		// if N==2, only simple cut; N==3 includes TMVA selector
		if (N1<2 || N1>3)  {cout <<"invalid line: "<<sline.Data()<<endl; continue;}

		// extract the mode code by converting to integer
		int mcode = toks[0].Atoi();
		int en    = mcode / 1000; // energy prefix (e.g. 550 for sqs=5.5GeV)
		
		// if new energy, store the energy prefix in fSTencode
		if ( std::find(fSTencode.begin(), fSTencode.end(), en) == fSTencode.end() ) fSTencode.push_back(en);
		
		// now split the cut string into single cuts
		int N2 = SplitString(toks[1],"&&",cuts,30);
		
		// replace windows cuts 'abs(<name>-x)<y' by 2 single cuts
		for (int i=0;i<N2;++i) 
		{
			cuts[i].ReplaceAll(" ","");
			if (cuts[i]=="tag") cuts[i]="tag>0";
			
			// is cut a window cut?
			if (cuts[i].BeginsWith("abs"))
			{
				int pos1 = cuts[i].First('-'); // position of '-'
				int pos2 = cuts[i].Last(')');  // position of closing abs bracket 
				int pos3 = cuts[i].Last('<');  // position of operator '<'
				
				TString name = TString(cuts[i](4,pos1-4));  // name is from first char after 'abs(' to '-' sign
				double val = TString(cuts[i](pos1+1,(pos2-pos1)-1)).Atof(); // val to cut on is from '-' to closing bracket ')'
				double win = TString(cuts[i](pos3+1,1000)).Atof();  // window size is from '<' to end
				cuts[i]= TString::Format("%s>%f",name.Data(), val-win);      // construct the 1st and
				cuts[N2++] = TString::Format("%s<%f",name.Data(), val+win);   // 2nd single cut
			}
		}
		
		// *** now encode the string cuts into sets of (variable index, operator, cutvalue)
		// *** and put the cut sets into a map with key being the mode code
		
		STCutSet cs;
		cs.ncut = N2;
		
		cs.tmvaread = 0;
		cs.tmvanvar = 0;
		
		bool ok = true;  // if a variable is not know switch to false
		
		for (int i=0;i<N2;++i)
		{
			int op=-1, j=0;
			while (j<5 && op==-1) 
			{
				if (cuts[i].Contains(fSTOps[j])) 
				{
					SplitString(cuts[i],fSTOps[j],toks2,5); 
					op = j;
				}
				j++;
			}

			if (op<0) // fail
			{
				i=N2+1;
				ok=false;
			}
			
			// event shape variable?
			if (fSTVarmap.find(toks2[0]) != fSTVarmap.end())
			{
				cs.varid[i]  = fSTVarmap[toks2[0]];
				cs.op[i]     = op;
				cs.cutval[i] = toks2[1].Atof();
			}
			// candidate variable?
			else if (CodeVariable(toks2[0])>0)
			{
				cs.varid[i]  = CodeVariable(toks2[0]);
				cs.op[i]     = op;
				cs.cutval[i] = toks2[1].Atof();
				
			}
			else // fail
			{
				cout <<"[PndSoftTriggerTask] **** Unmapped cut variable: "<<toks2[0].Data()<<". Skipping mode "<<mcode<<"."<<endl;
				i=N2+1;
				ok=false;
			}
		}
		
		// *** do we have a TMVA Selector defined?
		if (N1==3)
		{
			// now split the TMVA string into single tokens
			int N3 = SplitString(toks[2]," ",toks2,30);
			
			if (N3<4) // at least four elements expected; fail
			{
				cout <<"[PndSoftTriggerTask] **** Invalid TMVA configuration for mode "<<mcode<<". Skipping."<<endl;
				ok = false;
			}
			
			if (ok)
			{
				// first token is the weightfile name without leading path and ending '.weights.xml'
				TString wfile   = TString(getenv("VMCWORKDIR"))+"/softrig/weights/"+toks2[0]+".weights.xml";
				cs.tmvameth = toks2[0](toks2[0].Length()-3,3); 
				
				// last token is the cut to the TMVA output
				cs.tmvacut  = toks2[N3-1].Atof();
				
				// tokens inbetween are the variables
				cs.tmvanvar = N3 - 2;
				
				// create the reader
				cs.tmvaread = new TMVA::Reader("Silent");
				
				for (int i=1; i<N3-1; ++i)
				{
					if (fSTVarmap.find(toks2[i]) != fSTVarmap.end()) cs.tmvavarid[i-1] = fSTVarmap[toks2[i]];
					else if (CodeVariable(toks2[i])>0) cs.tmvavarid[i-1] = CodeVariable(toks2[i]);
					else // fail 
					{
						cout <<"[PndSoftTriggerTask] **** Unmapped TMVA variable: "<<toks2[i].Data()<<". Skipping mode "<<mcode<<"."<<endl;
						i=N2+1;
						ok=false;
					}
					if (ok) cs.tmvaread->AddVariable(toks2[i], &fSTInputValues[i-1]);
				}
				if (ok) cs.tmvaread->BookMVA(cs.tmvameth,wfile);
			}
		}
		
		if (ok) fSTSelmap[mcode] = cs;
		
		if (fVerbose)
		{
			cout <<"mcode="<<mcode<<endl;
			STCutSet cts = fSTSelmap[mcode];
			cout <<"  Ncut="<<cts.ncut<<" (";
			for (int k=0;k<cts.ncut;++k) cout <<cts.varid[k]<<" ";
			cout <<")"<<endl;
			if (cts.tmvaread!=0)
			{
				cout <<"  TMVAMeth="<<cts.tmvameth<<"; Nvar="<<cts.tmvanvar<<"; Cut="<<cts.tmvacut<<"; TMVAobject="<<cts.tmvaread<<" (";			
				for (int k=0;k<cts.tmvanvar;++k) cout <<cts.tmvavarid[k]<<" ";
				cout <<")"<<endl;
			}
		}
	}
	
}

// -------------------------------------------------------------------------

int PndSoftTriggerTask::SplitString(TString s, TString delim, TString *toks, int maxtoks)
{
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
		if (i<maxtoks) 
		{
			toks[i] = ((TObjString*)tok->At(i))->String();
			toks[i].ReplaceAll("\t","");
			toks[i] = toks[i].Strip(TString::kBoth);
		}
	return N;
}

// -------------------------------------------------------------------------
	
void PndSoftTriggerTask::SetParContainers() 
{
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndSoftTriggerTask::Exec(Option_t* opt)
{
	// *** prepare TCA for writing OnlineFilterInfo
	if ( fTcaOnlineFilterInfo->GetEntriesFast() != 0 ) fTcaOnlineFilterInfo->Delete();
	
	// *** some variables
	int i=0,j=0, k=0;
	
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	
	// *** read the next event
	fAnalysis->GetEventInTask();
	
	// *** fill all lists necessary for combinatorics
	FillGlobalLists();
	
	// *** go through MC truth list and codify the recoil mode (pi+-, pi0, eta, gamma, K+-)
	fRecoilMode  = DetermineRecoilMode(fRecoilCnt);
	
	// *** estimate primary vertex
	fPrimVtxQa = fPocaVertexer->GetPocaVtx(fPrimVtx, fChargedCands);
	
	// *** setup eventshape object
	PndEventShape evtShape(fAllCands,fIniP4,fGammaMinE,fTrackMinP);
	fEventShape = &evtShape;
	
	if (fApplyFullSelection) FillEventShapeVarArray();
	
	int tag_glob = 0, tag_glob_pre = 0, tag_pre = 0;

	PndOnlineFilterInfo* info=new ( (*fTcaOnlineFilterInfo)[0] ) PndOnlineFilterInfo();	
	
	// *** loop through all channels and tag
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
	{
		PndSoftTriggerLine *tl = it->second;	
		
		if (tl->GetThreshold()<=fEcm && tl->GetTagActive() )
		{
			int ntag =  TagMode( tl , tag_pre);
			tl->SetNTagged( ntag );
			info->SetNTag(it->first, ntag);                      
			tag_glob += ntag;
			tag_glob_pre += tag_pre;
		}
	}
	
	Int_t tagged  = (tag_glob>0);
	Int_t taggedm = (tag_glob_pre>0);  // tagged by mass cut only
	
	// *** write common information	
	if (fQAEvent)
	{
		ntp->Column("ev",		(Int_t) 	fEvtCount,	  0);
		ntp->Column("run",  	(Int_t) 	fRunNum,	  0);
		ntp->Column("mode",		(Int_t)		fMode,		  0);
		ntp->Column("recmode",  (Int_t)     fRecoilMode,  0);
		ntp->Column("reccnt",   (Int_t)     fRecoilCnt,   0);

		fQA->qaP4("beam", fIniP4, ntp);
		ntp->Column("primvx",   (Float_t)   fPrimVtx.X(), 0.0f);
		ntp->Column("primvy",   (Float_t)   fPrimVtx.Y(), 0.0f);
		ntp->Column("primvz",   (Float_t)   fPrimVtx.Z(), 0.0f);
		ntp->Column("primvqa",  (Float_t)   fPrimVtxQa  , 0.0f);
		
		// write tags of individual modes
		for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
		{
			PndSoftTriggerLine *tl = it->second;
			if ( tl->GetThreshold()<=fEcm && tl->GetTagActive() ) 
			{
				//ntp->Column("tag_"+tl->GetName(), (Int_t) tl->GetNTagged(), 0);
				ntp->Column(TString::Format("tag%d",tl->GetModeCode()), (Int_t) tl->GetNTagged(), 0);
			}
		}

		ntp->Column("tagall",	(Int_t)	tag_glob,		0);
		ntp->Column("tagpre",	(Int_t)	tag_glob_pre,	0);
		ntp->Column("tag",		(Int_t)	tagged,			0);
		ntp->Column("tagm",		(Int_t)	taggedm,		0);
		
		fQA->qaEventShape("es", fEventShape, ntp);
		
		// replace PID mult values from fEventShape by actual counts with individual algorithms
		ntp->Column("eslnpide", (Int_t)  fPidMult_025[0],		0 );
		ntp->Column("eslnpidmu",(Int_t)  fPidMult_025[1],		0 );
		ntp->Column("eslnpidpi",(Int_t)  fPidMult_025[2],		0 );
		ntp->Column("eslnpidk", (Int_t)  fPidMult_025[3],		0 );
		ntp->Column("eslnpidp", (Int_t)  fPidMult_025[4],		0 );
		
		ntp->DumpData();
	}
	
	// *** write out MC info
	if (fQAMc) {fQA->qaMcList("",fMcTruth,nmc); nmc->DumpData();}
	
}


void PndSoftTriggerTask::Finish()
{	
	if (ntp) ntp->GetInternalTree()->Write();			  // overall info
	if (nks0) nks0->GetInternalTree()->Write();		      // Ks0 QA
	if (npi0) npi0->GetInternalTree()->Write();			  // pi0 QA
	if (neta) neta->GetInternalTree()->Write();			  // eta QA
	if (nmc)  nmc->GetInternalTree()->Write();			  // MC QA
	
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
	{
		PndSoftTriggerLine *tl = it->second;
		if (tl->GetRhoTuple())
			tl->GetRhoTuple()->GetInternalTree()->Write();
	}
}

// -------------------------------------------------------------------------
// determine recoil mode (N_gamma + 10*N_pi0 + 100*N_pi+- + 1000*N_K+- + 10000*N_K0 + 100000*N_eta) 
int PndSoftTriggerTask::DetermineRecoilMode(int &mode)
{
	mode = 0;
	int code=-1;
	
	if ( fMcTruth.GetLength()==0 ) return -1;
	if ( fMcTruth[0]==0 ) return -1;
	
	for (int i=0; i<fMcTruth[0]->NDaughters(); ++i)
	{
		int dpdg = abs(fMcTruth[0]->Daughter(i)->PdgCode());
		
		switch (dpdg)
		{
			case 22:  mode += 1;      break;  // gamma
			case 111: mode += 10;     break;  // pi0 
			case 211: mode += 100;    break;  // pi+
			case 321: mode += 1000;   break;  // K+
			case 130: mode += 10000;  break;  // K0
			case 310: mode += 10000;  break;  // K0
			case 221: mode += 100000; break;  // eta
		}
	}
	
	switch (mode) 
	{
		case       0 : code = 0;  break;  // no recoil
		case       1 : code = 1;  break;  // gamma
		case      10 : code = 2;  break;  // pi0
		case  100000 : code = 3;  break;  // eta
		case      20 : code = 4;  break;  // pi0 pi0
		case     200 : code = 5;  break;  // pi+ pi-
		case    2000 : code = 6;  break;  // K+ K-
		case   20000 : code = 7;  break;  // K0 K0b
		case  200000 : code = 8;  break;  // eta eta
		case     210 : code = 9;  break;  // pi+ pi- pi0
		default      : code =-1;  break;
	}
	
	return code;
}

// -------------------------------------------------------------------------

int PndSoftTriggerTask::CreateKs0Cands(RhoTuple *n)
{
	// *** basic KS0 Reco
	//fKs0Cands.Combine(fPionPlus, fPionMinus);
	fKs0Cands.Combine(fPidList[4], fPidList[5]);
	// *** pre select for qa
	fKs0Cands.Select(fKs0PreSel);
	fKs0Cands.SetType(310);
	
	if (n)
	{
		for (int i=0; i<fKs0Cands.GetLength();++i)
		{			
			Float_t nsig = (Float_t) fabs(fKs0Cands[i]->Mass()-fKs0Mean)/fKs0Sigma;
			Float_t tag  = nsig<fNsigAux;  // instead of fNsigTag, fNsigAux cut is applied for KS
			
			n->Column("tag", 	(Int_t) tag,		0);
			n->Column("nsig", 	(Float_t) nsig,		0.0f);
			n->Column("ev",  	(Int_t) fEvtCount,	0);
			n->Column("run",  	(Int_t) fRunNum,	0);
			n->Column("num", 	(Int_t) i,			0);
			n->Column("mode",	(Int_t) fMode,		0);
			n->Column("xmean", (Float_t) fKs0Mean,		0.0f);
			n->Column("xsig", 	(Float_t) fKs0Sigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, n);
			fQA->qaKs0("x",  fKs0Cands[i], n);
			
			n->DumpData();
		}
	}
	
	// *** final selection
	fKs0Cands.Select(fKs0Sel);
	
	return fKs0Cands.GetLength();
}

// -------------------------------------------------------------------------


void PndSoftTriggerTask::FillGlobalLists()
{
	int i;
	
	// *** fill standard lists
	fAnalysis->FillList( fMcTruth, 			"McTruth" );
	fAnalysis->FillList( fAllCands,			"All"  ,    fAlgoElectron);
	fAnalysis->FillList( fChargedCands,		"Charged" , fAlgoElectron);
	fAnalysis->FillList( fNeutralCands,		"Neutral" );	
	
	// *** fill PID lists

	fAnalysis->FillList( fPidList[0], 	"ElectronAllPlus", 	fAlgoElectron );
	fAnalysis->FillList( fPidList[1],	"ElectronAllMinus", fAlgoElectron );
	
	fAnalysis->FillList( fPidList[2],	"MuonAllPlus", 		fAlgoMuon );
	fAnalysis->FillList( fPidList[3],	"MuonAllMinus", 	fAlgoMuon );
	
	fAnalysis->FillList( fPidList[4],	"PionAllPlus", 		fAlgoPion );
	fAnalysis->FillList( fPidList[5],	"PionAllMinus", 	fAlgoPion );
	
	fAnalysis->FillList( fPidList[6],	"KaonAllPlus", 		fAlgoKaon );
	fAnalysis->FillList( fPidList[7],	"KaonAllMinus", 	fAlgoKaon );
	
	fAnalysis->FillList( fPidList[8],	"ProtonAllPlus", 	fAlgoProton );
	fAnalysis->FillList( fPidList[9],	"ProtonAllMinus", 	fAlgoProton );

	// *** apply minimum PID cut if required
	if (fIniPidCut>0) for (i=0;i<10;++i) SelectPidProb(fPidList[i],i/2, fIniPidCut);
		
	// *** select on lists
	fGammaCands.Select(fNeutralCands, fEnergySel);
	
	// *** apply minimum momentum cut if required
	if (fTrackMinP>0) for (i=0;i<10;++i) fPidList[i].Select(fMomentumSel);
		
	// *** count PID multiplicities
	for (i=0;i<5;++i) fPidMult_025[i] = MultPidProb(fPidList[2*i], i, 0.25) + MultPidProb(fPidList[2*i+1], i, 0.25);

	// *** Ks reco
	CreateKs0Cands(nks0);
	
	// *** pi0 reco
	fPi0Cands.Combine(fGammaCands, fGammaCands);
	fPi0Cands.Select(fPi0PreSel);
	fPi0Cands.SetType(111);
	if (fQAPi0)
	{
		for (i=0; i<fPi0Cands.GetLength();++i)
		{
			Float_t nsig = (Float_t) fabs(fPi0Cands[i]->Mass()-fPi0Mean)/fPi0Sigma;
			Float_t tag  = nsig<fNsigAux;  // instead of fNsigTag, a fNsigAux cut is applied for pi0
			
			npi0->Column("tag", 	(Int_t) tag,		0);
			npi0->Column("nsig", 	(Float_t) nsig,		0.0f);
			npi0->Column("ev",  	(Int_t) fEvtCount,	0);
			npi0->Column("run",  	(Int_t) fRunNum,	0);
			npi0->Column("num", 	(Int_t) i,			0);
			npi0->Column("mode",	(Int_t) fMode,		0);
			npi0->Column("xmean", (Float_t) fPi0Mean,		0.0f);
			npi0->Column("xsig", 	(Float_t) fPi0Sigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, npi0);
			fQA->qaPi0("x", fPi0Cands[i], npi0);
			npi0->DumpData();
		}
	}
	fPi0Cands.Select(fPi0Sel);
	
	// *** era reco
	fEtaCands.Combine(fGammaCands, fGammaCands);
	fEtaCands.Select(fEtaPreSel);
	fEtaCands.SetType(221);
	if (fQAEta)
	{
		for (i=0; i<fEtaCands.GetLength();++i)
		{
			Float_t nsig = (Float_t) fabs(fEtaCands[i]->Mass()-fEtaMean)/fEtaSigma;
			Float_t tag  = nsig<fNsigAux;  // instead of fNsigTag, a fNsigAux cut is applied for eta
			
			neta->Column("tag", 	(Int_t) tag,		0);
			neta->Column("nsig", 	(Float_t) nsig,		0.0f);
			neta->Column("ev",  	(Int_t) fEvtCount,	0);
			neta->Column("run",  	(Int_t) fRunNum,	0);
			neta->Column("num", 	(Int_t) i,			0);
			neta->Column("mode",	(Int_t) fMode,		0);
			neta->Column("xmean", (Float_t) fEtaMean,		0.0f);
			neta->Column("xsig", 	(Float_t) fEtaSigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, neta);
			fQA->qaPi0("x", fEtaCands[i], neta);
			
			neta->DumpData();
		}
	}
	fEtaCands.Select(fEtaSel);
	
	// *** copy all lists to indexed ones for combinatorics

	fPidList[10] = fGammaCands;
	
	fPidList[11] = fPi0Cands;
	fPidList[12] = fKs0Cands;
	fPidList[13] = fEtaCands;
}

// -------------------------------------------------------------------------
// Fill the array of event shape variables (called once per event)
// here the connection between the variable indices and the values is made

void PndSoftTriggerTask::FillEventShapeVarArray()
{
	// *** Event Shape Vars

	// (  0) eslnpide        (  1) eslnpidmu       (  2) eslnpidpi       (  3) eslnpidk        (  4) eslnpidp           
	// (  5) esfw1           (  6) esfw2           (  7) esfw3           (  8) esfw4           (  9) esfw5
	// ( 10) esapl           ( 11) escir           ( 12) espla           ( 13) esnneut         ( 14) esnpart      
	// ( 15) esnchrg         ( 16) espmax          ( 17) espmaxl         ( 18) espmin          ( 19) espminl       
	// ( 20) esprapmax       ( 21) esptmax         ( 22) essumen         ( 23) essumenl        ( 24) essumetn       
	// ( 25) essumpc         ( 26) essumpc05       ( 27) essumpcl        ( 28) essumpt         ( 29) essumptc    
	// ( 30) esthr           ( 31) essph           ( 32) esptmin         ( 33) esncp10l        ( 34) esnne10l   
	
	// don't use PID mult values from fEventShape (based on AllCands and only one algo)
	for (int i=0;i<5;++i) fSTVarEvArray[i] = fPidMult_025[i]; 
	
	fSTVarEvArray[ 5] = fEventShape->FoxWolfMomR(1);
	fSTVarEvArray[ 6] = fEventShape->FoxWolfMomR(2);
	fSTVarEvArray[ 7] = fEventShape->FoxWolfMomR(3);
	fSTVarEvArray[ 8] = fEventShape->FoxWolfMomR(4) ;
	fSTVarEvArray[ 9] = fEventShape->FoxWolfMomR(5);
	
	fSTVarEvArray[10] = fEventShape->Aplanarity();
	fSTVarEvArray[11] = fEventShape->Circularity();
	fSTVarEvArray[12] = fEventShape->Planarity();
	fSTVarEvArray[13] = fEventShape->NNeutral();
	fSTVarEvArray[14] = fEventShape->NParticles();
	
	fSTVarEvArray[15] = fEventShape->NCharged();
	fSTVarEvArray[16] = fEventShape->PmaxCms();
	fSTVarEvArray[17] = fEventShape->PmaxLab();
	fSTVarEvArray[18] = fEventShape->PminCms();
	fSTVarEvArray[19] = fEventShape->PminLab();
	
	fSTVarEvArray[20] = fEventShape->PRapmax();
	fSTVarEvArray[21] = fEventShape->Ptmax();
	fSTVarEvArray[22] = fEventShape->NeutESumCms();
	fSTVarEvArray[23] = fEventShape->NeutESumLab();
	fSTVarEvArray[24] = fEventShape->NeutEtSumCms();
	
	fSTVarEvArray[25] = fEventShape->ChrgPSumCms();
	fSTVarEvArray[26] = fEventShape->SumChrgPminCms(0.5);
	fSTVarEvArray[27] = fEventShape->ChrgPSumLab();
	fSTVarEvArray[28] = fEventShape->PtSumLab();
	fSTVarEvArray[29] = fEventShape->ChrgPtSumCms();

	fSTVarEvArray[30] = fEventShape->Thrust();
	fSTVarEvArray[31] = fEventShape->Sphericity();
	fSTVarEvArray[32] = fEventShape->Ptmin();
	fSTVarEvArray[33] = fEventShape->MultChrgPminLab(1.0);
	fSTVarEvArray[34] = fEventShape->MultNeutEminLab(1.0);
	
}

// -------------------------------------------------------------------------

TLorentzVector PndSoftTriggerTask::BoostCms(TLorentzVector l)
{
	TLorentzVector lcm = l;
	lcm.Boost(-fIniP4.BoostVector());
	return lcm;
}

// -------------------------------------------------------------------------
void PndSoftTriggerTask::GetAngles(RhoCandidate *c, double &oang, double &decang)
{
	oang   = -999.;
	decang = -999.;
	
	if (c->NDaughters()!=2) return;

	RhoCandidate *d0 = c->Daughter(0);
	RhoCandidate *d1 = c->Daughter(1);

	// opening angle lab
	oang = d0->P3().Angle(d1->P3());

	// decay angle
	TLorentzVector d_cms = d0->P4();
	d_cms.Boost(-(c->P4().BoostVector()));
	decang  = d_cms.Vect().Angle(c->P3());
}


// -------------------------------------------------------------------------
// *** Get Poca vertex info (for secondary vertex cuts)
double PndSoftTriggerTask::GetPocaVtx(RhoCandidate* c, double &dist, double &ctau)
{
	// *** simple vtx finder
	TVector3 vtx, altvtx, primvtx;
	double qavtx = fPocaVertexer->GetPocaVtx(vtx, c);
	
	// *** determine poca of rest of tracks
	RhoCandList l;
	fAnalysis->FillList(l, "Charged");
	
	// remove c's daughters from charged tracks list
	l.RemoveFamily(c);

	// if at least 2 tracks left, compute the poca vertex of the residual tracks
	if (l.GetLength()>1) fPocaVertexer->GetPocaVtx(altvtx, l);
	else altvtx = fPrimVtx; // else use the primary vtx found before
	
	dist = 999.;
	// *** if vertex of either all charged or residual tracks was found, compute distance and ctau
	if (altvtx.Mag()>0.) dist = (vtx-altvtx).Mag();
	
	ctau = dist*c->M()/c->P();

	return qavtx;
}


// -------------------------------------------------------------------------
// transform a variable string to an integer code
// code conventions are for code ABCDEF
// - ABCDEF < 100: special variable directly filled (e.g. mmiss = 80, mdif = 81, tag = 99)
// - ABCD defines candidate (e.g. x=9999, xd0=9099, xd1=9199, xd0d1=9019, xd0d0g2=9001 etc)
// - EF defines variable: 
//    - E: 1=kin, 2=pid, 3=vtx, 4=ang
//    
//   E\F    |    0    |    1    |    2    |    3    |    4    |    5 
// ----------------------------------------------------------------------
//  1 (kin) |    p    |   pt    |  tht    |  pcm    |  thtcm  |
//  2 (pid) |  pide   |  pidmu  |  pidpi  |  pidk   |  pidp   |  pidmax
//  3 (vtx) |  pocqa  | pocdist | pocctau |         |         |
//  4 (ang) |  oang   | decang  | cdecang |         |         |

int PndSoftTriggerTask::CodeVariable(TString v)
{
	int i=0, code=0, A=9, B=9, C=9, D=9, EF=0;
	
	if (v=="mmiss") return 80;
	if (v=="xmdif") return 82;
	if (v=="tag")   return 99;
	
	for (i=0; i<fSTNQuant; ++i) 
		if (v.EndsWith(fSTVarSuff[i])) 
		{
			EF=fSTQuantCode[i]; 
			v=v(0,v.Length()-fSTVarSuff[i].Length());
			i=1000;
		} 
	
	if (i<1000) {cout <<"[PndSoftTriggerTask] ***** Unknown variable suffix in variable '"<<v<<"'"<<endl; return -1;}
	
	// get rid of special daughter namings for pi0 gammas and KS pi+-
	v.ReplaceAll("pi1","d0");
	v.ReplaceAll("pi2","d1");
	v.ReplaceAll("g1","d0");
	v.ReplaceAll("g2","d1");
	
	TRegexp r1("xd[0-4]$");
	TRegexp r2("xd[0-4]d[0-4]$");
	TRegexp r3("xd[0-4]d[0-4]d[0-4]$");
	
	if (v!="x" && v(r1)=="" && v(r2)=="" && v(r3)=="") {cout <<"[PndSoftTriggerTask] ***** Unknown candidate prefix in variable '"<<v<<"'"<<endl; return -1;}
	
	int l = v.Length();
	
	if (l>1) B = TString(v(2,1)).Atoi();
	if (l>3) C = TString(v(4,1)).Atoi();
	if (l>5) D = TString(v(6,1)).Atoi();
	
	code=EF+100*D+1000*C+10000*B+100000*A;
	
	return code;
}

// -------------------------------------------------------------------------
// Fill individual candidate specific variables (called for every candidate)
// here the connection between the variable indices and the values is made
// in case variables are connected with more computing effort, they are filled simultaneously

double PndSoftTriggerTask::GetVarValue(RhoCandidate *c, int id)
{	
	// previously filled event shape var?
	if (id>=0 && id<35) return fSTVarEvArray[id];
	
	// is variable already in cache?
	if (fSTVarCandArray.find(id) != fSTVarCandArray.end()) return fSTVarCandArray[id];
	
	// *** now compute variable values based on code 'id'
	
	// these are special cases with simple code numbers
	if (id==80) return (fIniP4 - c->P4()).M();         // mmiss
	if (id==82) return (c->M() - c->Daughter(0)->M()); // D* mdif
	if (id==99) return 1.0;                            // artificial value for tag>0 (= empty cut)
		
	// determine quantity and candidate from id code
	int quant = id%100;
	int cand  = id/100;
	int B = (id/10000)%10, C = (id/1000)%10, D = (id/100)%10;
	
	// find particle under consideration
	RhoCandidate *cnd=0;
	
	if (B==9) cnd = c;
	else if (C==9) cnd = c->Daughter(B);
	else if (D==9) cnd = c->Daughter(B)->Daughter(C);
	else cnd = c->Daughter(B)->Daughter(C)->Daughter(D);
	
	// coding for variable suffix (to translate variable names to code)
	//   E\F    |    0    |    1    |    2    |    3    |    4    |    5 
	// ----------------------------------------------------------------------
	//  1 (kin) |    p    |   pt    |  tht    |  pcm    |  thtcm  |
	//  2 (pid) |  pide   |  pidmu  |  pidpi  |  pidk   |  pidp   |  pidmax
	//  3 (vtx) |  pocqa  | pocdist | pocctau |         |         |
	//  4 (ang) |  oang   | decang  | cdecang |         |         |
	
	double val = -999.;
	double pocqa, pocdist, pocctau, oang, decang, cdecang, pidmax;
	TLorentzVector bl;
	
	switch (quant)
	{
		case 10 : // p
			fSTVarCandArray[id] = cnd->P();  break;
		
		case 11 : // pt 
			fSTVarCandArray[id] = cnd->Pt(); break;
		
		case 12 : // tht 
			fSTVarCandArray[id] = cnd->P4().Theta(); break;
		
		case 13 : case 14 : // pcm, thtcm
			bl = BoostCms(cnd->P4());
			fSTVarCandArray[cand*100+13] = bl.P();
			fSTVarCandArray[cand*100+14] = bl.Theta();
			break;
			
		case 20 : case 21 : case 22 : case 23 : case 24 :  // pide,..., pidp
			fSTVarCandArray[id] = cnd->GetPidInfo(quant%10);
			break;
		
		case 25 : // pidmax
			pidmax = 0;
			for (int i=0;i<5;++i) if (cnd->GetPidInfo(i)>pidmax) pidmax = cnd->GetPidInfo(i);
			fSTVarCandArray[id] = pidmax;
			break;
		
		case 30 : case 31 : case 32 : // pocqa, pocdist, pocctau
			fSTVarCandArray[cand*100+30] = GetPocaVtx(cnd, pocdist, pocctau);
			fSTVarCandArray[cand*100+31] = pocdist; 
			fSTVarCandArray[cand*100+32] = pocctau; 
			break;
		
		case 40 : case 41 : case 42 : // oang, decang, cdecang
			GetAngles(cnd, oang, decang);
			cdecang = cos(decang);
			fSTVarCandArray[cand*100+40] = oang; 
			fSTVarCandArray[cand*100+41] = decang; 
			fSTVarCandArray[cand*100+42] = cdecang; 
			break;
		default:
			cout <<"[PndSoftTriggerTask] ***** wrong variable code '"<<quant<<"'"<<endl;
	}
	
	return fSTVarCandArray[id];
}

// -------------------------------------------------------------------------
// Fill the array of candidate specific variables (called for every candidate)
// here the connection between the variable indices and the values is made

void PndSoftTriggerTask::FillVarArray(RhoCandidate *c, int mcode, Bool_t tmva)
{
	int i=0;
	
	fSTVarCandArray.clear();   // reset cand var cache (used during fill of values)
	
	// fill all requested variables
	if (tmva) 
		for (i=0; i<fSTSelmap[mcode].tmvanvar; ++i) 
			fSTInputValues[i] = GetVarValue(c, fSTSelmap[mcode].tmvavarid[i]);  // fill variables for tmva
	else
		for (i=0; i<fSTSelmap[mcode].ncut; ++i) 
			fSTInputValues[i] = GetVarValue(c, fSTSelmap[mcode].varid[i]);      // fill variables for cut selection
}	

// -------------------------------------------------------------------------
// *** In case a particle is a D*0, D*+, D_s*+, apply cut on mass difference 
// *** for QA NTuple writeout (smaller file size)
bool PndSoftTriggerTask::AcceptDstCut(RhoCandidate *c)
{
	int pdg = abs(c->PdgCode());
	
	if ( (pdg==413 || pdg==423 || pdg==433) && fabs(c->M() - c->Daughter(0)->M() - 0.143) > fDstMDiffCut ) return false;
	
	return true;
}

// -------------------------------------------------------------------------
// *** Apply full selection to a candidate
bool PndSoftTriggerTask::AcceptCandidate(int mode, RhoCandidate *c, RhoParticleSelectorBase *sel)
{
	int mcode = fSTencode[fSTModeIndex]*1000+mode;	
	
	if (fVerbose>1) cout <<"AcceptCandidate : mode="<<mode<<" mcode="<<mcode;

	// no selection defined for this mode
	if ( fSTSelmap.find(mcode) == fSTSelmap.end() ) {if (fVerbose>1) cout <<endl;return false;} 
	if (fVerbose>1) cout <<" found cut set";
	
	// not accepted by final (mass) selector
	if ( sel && !(sel->Accept(c)) ) {if (fVerbose>1) cout <<endl;return false;}
	// not accepted by D* mass diff cut
	if (!AcceptDstCut(c)) return false;
		
	if (fVerbose>1) cout <<" accepted by precuts"<<endl;
	
	// *** 
	// *** check for cut selection
	// *** 
	STCutSet cs = fSTSelmap[mcode];
	FillVarArray(c, mcode);
	
	bool acc = true;
	
	for (int i=0;i<cs.ncut;++i)
	{
		if (fVerbose>1) cout <<"  -> checking var["<<cs.varid[i]<<"] ("<<fSTInputValues[i]<<") "<<fSTOps[cs.op[i]]<<" "<<cs.cutval[i]<<endl;
		
		switch (cs.op[i]) // which operator is used for this cut?
		{
		case 0:	// check var >= value
			if ( !(fSTInputValues[i]>=cs.cutval[i]) ) acc=false; 
			break;
		case 1:	// check var > value
			if ( !(fSTInputValues[i]>cs.cutval[i]) ) acc=false; 
			break;
		case 2:	// check var == value
			if ( !(fSTInputValues[i]==cs.cutval[i]) ) acc=false; 
			break;
		case 3:	// check var <= value
			if ( !(fSTInputValues[i]<=cs.cutval[i]) ) acc=false; 
			break;
		case 4:	// check var < value
			if ( !(fSTInputValues[i]<cs.cutval[i]) ) acc=false; 
			break;
		default : 
			acc=false; 
			break;
		}
	}
	
	// *** 
	// *** check for TMVA selector in addition
	// *** 
	if (cs.tmvaread)
	{
		FillVarArray(c, mcode, true);
		if (fVerbose>1) cout <<"  -> checking TMVA "<<cs.tmvameth<<endl;
		float mvaout = cs.tmvaread->EvaluateMVA(cs.tmvameth);
		if (mvaout<cs.tmvacut) acc=false;
	}
	
	
	if (fVerbose>1)
	{
		if (acc) cout <<" final accept"<<endl;
		else cout <<endl;
	}
	
	return acc;
}


// -------------------------------------------------------------------------
// *** Determine pdg code of anti particle
int PndSoftTriggerTask::AntiPdg(int pdg)
{
	int antipdg = pdg;
	TParticlePDG *part = fPdg->GetParticle(pdg)->AntiParticle();
	if (part) antipdg = part->PdgCode();
	
	return antipdg;
}

// -------------------------------------------------------------------------
// *** create a list based on indices
void PndSoftTriggerTask::CombineList(RhoCandList &l, int mothpdg, int amothpdg, std::vector<int> &idx, std::vector<int> &aidx, bool cc)
{
	l.Cleanup();
	
	int nd = idx.size();
	
	switch (nd) 
	{
	case 2: 
		l.Combine(fPidList[idx[0]], fPidList[idx[1]], mothpdg); 
		if (cc) l.CombineAndAppend(fPidList[aidx[0]], fPidList[aidx[1]], amothpdg);
		break;
		
	case 3: 
		l.Combine(fPidList[idx[0]], fPidList[idx[1]], fPidList[idx[2]], mothpdg);
		if (cc) l.CombineAndAppend(fPidList[aidx[0]], fPidList[aidx[1]], fPidList[aidx[2]], amothpdg);
		break;
		
	case 4: 
		l.Combine(fPidList[idx[0]], fPidList[idx[1]], fPidList[idx[2]], fPidList[idx[3]], mothpdg); 
		if (cc) l.CombineAndAppend(fPidList[aidx[0]], fPidList[aidx[1]], fPidList[aidx[2]], fPidList[aidx[3]], amothpdg); 
		break;

	case 5: 
		l.Combine(fPidList[idx[0]], fPidList[idx[1]], fPidList[idx[2]], fPidList[idx[3]], fPidList[idx[4]], mothpdg); 
		if (cc) l.CombineAndAppend(fPidList[aidx[0]], fPidList[aidx[1]], fPidList[aidx[2]], fPidList[aidx[3]], fPidList[aidx[4]], amothpdg); 
		break;
	default: return;
	}
	
}

// -------------------------------------------------------------------------
// *** General common combinatorics based on PndSoftTriggerLine input
int PndSoftTriggerTask::DoCombinatorics(RhoCandList &l, PndSoftTriggerLine *tl)
{
	l.Cleanup();
	fPidList[14].Cleanup();
	fPidList[15].Cleanup();
	
	int mothpdg = tl->GetMotherPdg();
	if (mothpdg<0) {cout << "[PndSoftTriggerTask] **** Invalid mother for combinatorics."<<endl; return 0;}
	
	int nd = tl->GetNDaughters();    // cache for mapping: pdgcode -> fPdgList indices
	
	std::vector<int> idx, aidx, auxidx, auxaidx;
	
	
	// fetch pdg code of antiparticle for mother
	int amothpdg = AntiPdg(mothpdg);
	
	// do we need to add charged conjugate combinatorics?
	bool cc = tl->GetCC();
	
	if (fVerbose>1)
	{
		for (int i=0; i<nd; ++i) cout <<tl->GetDaughterPdg(i)<<" ";	
		if (cc) cout <<" +cc";
		cout <<endl;
	}
	
	// flag whether pdg goes to idx list or auxidx list
	bool auxmode=false; 

	// idx for the mothers of the aux list
	int auxmothpdg, auxamothpdg; 
	
	// cache the indices of the fPidList according to the pdg codes
	for (int i=0; i<nd; ++i)
	{
		int dpdg = tl->GetDaughterPdg(i);
		
		// skip the codes for '[' and ']'
		if (dpdg==-99) continue;
		if (dpdg==-98) {auxmode=false; continue;} // filling auf aux list finished

		// if no list for particle with code 'dpdg'
		if ( fSTMapListIndex.find(dpdg) == fSTMapListIndex.end() )
		{
			// is it start of aux list definition? (e.g. D*0 -> D0 [K- pi+] pi0)
			if (tl->GetDaughterPdg(i+1) != -99)	
			{
				// if not, throw error!
				cout << "[PndSoftTriggerTask] **** Invalid daughter pdg code: "<<dpdg<<endl; 
				return 0;
			}
			else // start aux mode and decide about cc of aux list
			{
				auxmode = true;
				auxmothpdg  = dpdg;
				auxamothpdg = AntiPdg(dpdg);
				idx.push_back(14);
				if (auxamothpdg==auxmothpdg) aidx.push_back(14);
				else aidx.push_back(15);
				continue;
			}
		}
		if (auxmode)
		{
			auxidx.push_back(fSTMapListIndex[dpdg]);
			auxaidx.push_back(fSTMapListIndex[AntiPdg(dpdg)]); 
		}
		else
		{
			idx.push_back(fSTMapListIndex[dpdg]);
			aidx.push_back(fSTMapListIndex[AntiPdg(dpdg)]); 
		}
	}
	
	// does one of the lists have too many daughters?
	if (idx.size()>5)    {cout << "[PndSoftTriggerTask] **** Too many daughters ("<<idx.size()<<") for combinatorics of target resonance."<<endl;return 0;}
	if (auxidx.size()>5) {cout << "[PndSoftTriggerTask] **** Too many daughters ("<<auxidx.size()<<") for combinatorics of auxiliary resonance."<<endl;return 0;}
	
	if (fVerbose>1)
	{
		cout <<"("<<mothpdg<<"/"<<amothpdg<<") : ";
		for (int i=0; i<idx.size(); ++i) cout <<"("<<idx[i]<<"/"<<aidx[i]<<") "; 
		cout <<endl<<endl;
		if (auxidx.size()>0)
		{
			cout <<"("<<auxmothpdg<<"/"<<auxamothpdg<<") : ";
			for (int i=0; i<auxidx.size(); ++i) cout <<"("<<auxidx[i]<<"/"<<auxaidx[i]<<") "; cout <<endl;
		}
	}
	
	// do the combinatorics for aux list and final list
	if (tl->GetAuxNeeded())
	{
		// create different aux lists is pdg!=anti-pdg (e.g. D0 -> K- pi+ and anti-D0 -> K+ pi-)
		if (auxmothpdg!=auxamothpdg)
		{
			CombineList(fPidList[14], auxmothpdg, 0,  auxidx, auxidx, false);
			CombineList(fPidList[15], auxamothpdg, 0, auxaidx, auxaidx, false);
		}
		// create one aux list e.g. for eta_c -> ...
		else 
		{
			// check whether final state is its anti-final state (e.g. pi+ pi- pi0 = pi- pi+ pi0)
			// or not (e.g. eta_c -> KS K- pi+ != KS K+ pi-)
			// this can be done by summing all pdg codes for particles with pdg!=anti-pdg
			// if sum = 0, final state and anti-final state are the same (i.e. FS has for each particle the according anti-particle)
			int pdgsum = 0;
			for (int i=0;i<auxidx.size();++i) if (auxidx[i]<10) pdgsum+=fSTPidIndex[auxidx[i]];
			
			// if FS = anti-FS, just do combinatorics once (not adding the composites from anti-FS list)
			if (pdgsum==0) 
				CombineList(fPidList[14], auxmothpdg, 0,  auxidx, auxidx, false);
			// if FS != anti-FS, append also comb from anti-FS list (e.g. etac from (KS K- pi+) + (KS K+ pi-))
			else 
				CombineList(fPidList[14], auxmothpdg, auxamothpdg,  auxidx, auxaidx, true);	
		}
	}
	
	// create the final list using (or not using) the aux list
	CombineList(l, mothpdg, amothpdg, idx, aidx, cc);
		
	return l.GetLength();
}


// -------------------------------------------------------------------------
// *** General common tagging methode
int PndSoftTriggerTask::TagMode(PndSoftTriggerLine *tl, int &npre)
{
	// *** counter for full selection accepted cands
	int nacc = 0; 
	npre = 0;
	
	if ( !tl->GetTagActive() ) return 0;
	if ( fEcm < tl->GetThreshold() ) return 0;
	
	int mode       = tl->GetModeCode();
	RhoTuple *n    = tl->GetRhoTuple();
	TString prefix = tl->GetPrefix();
	double mean    = tl->GetMean();
	double sigma   = tl->GetSigma();
	
	RhoMassParticleSelector *sel = tl->GetSelector();
	
	RhoCandList l;

	// ** combinatorics
	DoCombinatorics(l, tl);
	
	// ** preselection
	l.Select(tl->GetQASelector());

	// *** store QA
	for (int i=0;i<l.GetLength();++i)
	{
		bool acc = false;
		bool tag = sel->Accept(l[i]) && AcceptDstCut(l[i]);
		
		// full selection?
		if (fApplyFullSelection>0) 
			acc = AcceptCandidate(mode, l[i], sel);
		// if not, simple mass window selection (and D* cut if applicable)
		else acc = tag;
		
		// for full selection in open mode (=2), trigger w/o detailed cuts are accepted based on mass window only 
		if ( !acc && fApplyFullSelection==2 && fSTSelmap.find(fSTencode[fSTModeIndex]*1000+mode) == fSTSelmap.end() )
			acc = tag;
				
		if (acc) nacc++;		
		if (tag) npre++;
		
		fAnalysis->McTruthMatch(l[i]);
		
		if (n && AcceptDstCut(l[i]))
		{
			// in case we only want to store MC truth matched candidates, check!
			if (fQAMctOnly && (l[i]->GetMcTruth())==0) continue;
			
			fQA->qaComp(prefix, l[i], n);
			fQA->qaEventShapeShort("es", fEventShape, n);
			
			// replace PID mult values from event shape by actual counts with individual algos
			n->Column("eslnpide", (Int_t)  fPidMult_025[0],	0);
			n->Column("eslnpidmu",(Int_t)  fPidMult_025[1],	0);
			n->Column("eslnpidpi",(Int_t)  fPidMult_025[2],	0);
			n->Column("eslnpidk", (Int_t)  fPidMult_025[3],	0);
			n->Column("eslnpidp", (Int_t)  fPidMult_025[4],	0);
			
			fQA->qaP4("beam", fIniP4, n);
			n->Column("primvx",   (Float_t)   fPrimVtx.X(), 0.0f);
			n->Column("primvy",   (Float_t)   fPrimVtx.Y(), 0.0f);
			n->Column("primvz",   (Float_t)   fPrimVtx.Z(), 0.0f);
			n->Column("primvqa",  (Float_t)   fPrimVtxQa  , 0.0f);

			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			Float_t nsig = (Float_t) fabs(l[i]->Mass()-mean)/sigma;
			
			n->Column("ev",  	(Int_t)   fEvtCount,	0);
			n->Column("num", 	(Int_t)   i,			0);
			n->Column("run",  	(Int_t)   fRunNum,		0);
			n->Column("mode",	(Int_t)   fMode,		0);
			n->Column("recmode",(Int_t)   fRecoilMode,  0);
			n->Column("reccnt", (Int_t)   fRecoilCnt,   0);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column(prefix+"mean",(Float_t) mean,		0.0f);
			n->Column(prefix+"sig", (Float_t) sigma,	0.0f);
			n->Column("tag", 	(Int_t)   tag,			0);
			n->Column("acc",    (Int_t) acc,            0);
			
			n->DumpData();
		}
	}
	
	return nacc;
}

ClassImp(PndSoftTriggerTask)
