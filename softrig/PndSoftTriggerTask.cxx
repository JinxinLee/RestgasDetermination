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
		
using std::cout;
using std::endl;

const int MAXCUT=50;

// *** Holds the cut set for a certain mode
struct STCutSet
{
	int    ncut;
	int    varid[MAXCUT];
	int    op[MAXCUT];
	double cutval[MAXCUT];
};

// *** maps needed for selection parsing
std::map<TString, int> fSTVarmap;
std::map<int, STCutSet> fSTSelmap;
std::map<int, TString> fSTOps;
std::map<int, PndSoftTriggerLine*> fSTTriggers;
std::map<int, int> fSTMapListIndex;

typedef std::map<int, PndSoftTriggerLine*>::iterator TrigIt;

//                            e+  e-  mu+  mu- pi+   pi-  K+    K-    p     pb   gam  pi0  KS   eta   aux  anti-aux
int fSTPidIndex[16] =       {-11, 11, -13, 13, 211, -211, 321, -321, 2212, -2212, 22, 111, 310, 221,   0,   0};

// *** mapped variable names for selection
//                       0            1             2           3           4          5           6          7           8         9         
TString fSTnames[] = {"eslnpide",  "eslnpidmu", "eslnpidpi", "eslnpidk", "eslnpidp", "esthr",    "esapl",   "esfw1",   "esnpart", "esptmax",

//                       10           11            12          13          14         15          16         17          18        19  
                      "detemcsum", "detemcmax", "p",         "pt",       "pcm",      "tht",      "d0pt",    "d1pt",    "d0pidk",  "d1tht",
                      
//                       20,          21            22          23          24         25          26         27          28        29
				      "mmiss",     "essumpt",   "essumptcl", "d0pcm",    "d1p",      "thtcm",    "ecm",     "esfw4",   "esfw2",   "esptmin", 
				       
//                       30,          31            32          33          34         35          36         37          38        39
				      "espmin",    "d0pide",    "d1pide",    "d0pidpi",  "d1pidpi",  "essumptc", "esfw5",   "essumpc", "d1pidk",  "oang",
				       
//                       40,          41            42          43          44         45          46         47          48        49
                      "espmax",    "essumenl",  "d2pidk",   "d3pidk",   "essumpcl", "d0pidmu", "d1pidmu", "essumen", "d0tht",     "d0p" };

//                       50,          51            52          53          54         55          56         57          58        59

// array to hold the current variable values; indices are according to the fSTnames array
// has to be filled for every tag mode
double fSTVarArray[100];

// codes for the available energies
int fSTencode[]   = {24, 38, 45, 55};
int fSTModeIndex = 0;

// -----   Default constructor   -------------------------------------------
PndSoftTriggerTask::PndSoftTriggerTask(double pmom, int mode, int runnum, TString trigfilename) :
	FairTask("Panda Softtrigger Task"),
	fVerbose(0), fMode(mode), fEvtCount(0), fRunNum(runnum), fSigCount(0), fNsigTag(8.0),	fNsigAux(5.0),	
	fTriggerFileName(trigfilename), fPhotosMax(0), fPhotosThresh(0.05), 
	fIniP4(0,0,0,0), fEcm(0.), fPbarMom(pmom),
	fQAPi0(false),fQAEta(false),fQAKs0(false),fQAEvent(false), fQAMc(false),
	fGammaMinE(0.03), fPi0MinE(0.0), fEtaMinE(0.0), fTrackMinP(0.15), fIniPidCut(0.0),
	fEventShape(NULL), 
	fQA(NULL),
	fPi0Sel(NULL), fEtaSel(NULL), fKs0Sel(NULL),
	fMomentumSel(NULL), fEnergySel(NULL),
	ntp(0),	nks0(0), npi0(0), neta(0)	
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
	
	// *** create ntuple
	if (fQAEvent) ntp = new RhoTuple("ntpev","Soft Trigger Common");
	if (fQAKs0)  nks0 = new RhoTuple("nks0","K_S -> pi+ pi-");
	if (fQAPi0)  npi0 = new RhoTuple("npi0","pi0 -> gamma gamma");
	if (fQAEta)  neta = new RhoTuple("neta","eta -> gamma gamma");
	if (fQAMc)   nmc  = new RhoTuple("nmc", "MC info");
		
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
	if (fCfgFileName!="" && fApplyFullSelection) ReadConfiguration();

	// *** set mode index for current beam momentum
	fSTModeIndex = 0;
	double diff = 1000.;
	
	for (int i=0;i<4;++i) 
	{
		double en = (double)fSTencode[i]/10.;
		if (fabs(fEcm-en)<diff)
		{
			diff=fabs(fEcm-en);
			fSTModeIndex = i;
		}
	}

	// *** initialize triggers
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
	{
		PndSoftTriggerLine *tl = it->second;
		tl->Init();
		
		if (fVerbose>0)
		{
			cout <<"*** MODE: "<<it->first<<endl;
			tl->Print();
			cout <<endl<<endl;
		}
	}

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
	
	fPi0QaMin = Pi0Mass - 0.1;		
	fPi0QaMax = Pi0Mass + 0.1;		
	
	fEtaQaMin = EtaMass - 0.2;		
	fEtaQaMax = EtaMass + 0.2;		
	
	fKs0QaMin = Ks0Mass - 0.2;		
	fKs0QaMax = Ks0Mass + 0.2;		
	
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
	if (fSTTriggers.find(mode) == fSTTriggers.end()) return;
	
	fSTTriggers[mode]->SetWriteQA(qa);
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
	if (fSTTriggers.find(mode) == fSTTriggers.end()) return;
	
	fSTTriggers[mode]->SetTagActive(tag);
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
	for (int i=0;i<MAXCUT;++i) fSTVarmap[fSTnames[i]] = i;
	fSTOps[0]=">";
	fSTOps[1]="==";
	fSTOps[2]="<";

	ifstream file(fCfgFileName.Data());
	
	if (!file.is_open()) 
	{
		cout <<"[PndSoftTriggerTask] **** Unable to open selection file "<<fCfgFileName.Data()<<endl;
		fApplyFullSelection=false;
	}
	else cout <<"[PndSoftTriggerTask] **** Reading selection from "<<fCfgFileName.Data()<<endl;
	
	char line[500];
	
	TString toks[30];
	TString toks2[5];

	// loop through file line by line
	while (!file.eof())
	{
		file.getline(line,499);
		TString sline(line);

		// empty lines or comments are skipped (a comment has to have a # as _first_ char)
		if (sline.BeginsWith("#")||sline=="") continue;	 
		
		// split the line into tokens; token 0 ist the mode code, token 1 is the complete cut string
		// e.g. '38400 : eslnpidp>0&&abs(pcm-2.105)<0.695&&esthr>0.9&&pt>0.8'
		int N = SplitString(sline, ":", toks,30);
		if (N>2)  {cout <<"invalid line: "<<sline.Data()<<endl; continue;}

		// extract the mode code by converting to integer
		int mcode = toks[0].Atoi();
		
		// now split the cut string into single cuts
		N = SplitString(toks[1],"&&",toks,30);
		
		
		// replace windows cuts 'abs(<name>-x)<y' by 2 single cuts
		for (int i=0;i<N;++i) 
		{
			toks[i].ReplaceAll(" ","");
			toks[i].ReplaceAll("==","=");
			
			// is cut a window cut?
			if (toks[i].BeginsWith("abs"))
			{
				int pos1 = toks[i].First('-'); // position of '-'
				int pos2 = toks[i].Last(')');  // position of closing abs bracket 
				int pos3 = toks[i].Last('<');  // position of operator '<'
				
				TString name = TString(toks[i](4,pos1-4));  // name is from first char after 'abs(' to '-' sign
				double val = TString(toks[i](pos1+1,(pos2-pos1)-1)).Atof(); // val to cut on is from '-' to closing bracket ')'
				double win = TString(toks[i](pos3+1,1000)).Atof();  // window size is from '<' to end
				toks[i]= TString::Format("%s>%.3f",name.Data(), val-win);      // construct the 1st and
				toks[N++] = TString::Format("%s<%.3f",name.Data(), val+win);   // 2nd single cut
			}
		}
		
		// now encode the string cuts into sets of (variable index, operator, cutvalue)
		// and put the cut sets into a map with key being the mode code
		STCutSet cs;
		cs.ncut = N;
		
		bool ok = true;  // if a variable is not know switch to false
		
		for (int i=0;i<N;++i)
		{
			int op=-1;
			if (toks[i].Contains(">")) {SplitString(toks[i],">",toks2,5); op = 0;}
			else if (toks[i].Contains("=")) {SplitString(toks[i],"=",toks2,5); op = 1;}
			else if (toks[i].Contains("<")) {SplitString(toks[i],"<",toks2,5); op = 2;}
			else // fail
			{
				i=N+1;
				ok=false;
			}
			
			if (fSTVarmap.find(toks2[0]) != fSTVarmap.end())
			{
				cs.varid[i]  = fSTVarmap[toks2[0]];
				cs.op[i]     = op;
				cs.cutval[i] = toks2[1].Atof();
			}
			else // fail
			{
				cout <<"[PndSoftTriggerTask] **** Unmapped variable: "<<toks2[0].Data()<<". Skipping mode "<<mcode<<"."<<endl;
				i=N+1;
				ok=false;
			}
		}
		
		if (ok) fSTSelmap[mcode] = cs;
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
	
	int tag_glob = 0, tag_pre = 0;

	PndOnlineFilterInfo* info=new ( (*fTcaOnlineFilterInfo)[0] ) PndOnlineFilterInfo();	
	
	// *** loop through all channels and tag
	for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
	{
		PndSoftTriggerLine *tl = it->second;		
		int ntag =  TagMode( tl , tag_pre);
		tl->SetNTagged( ntag );
		info->SetNTag(it->first, ntag);                      
		tag_glob += ntag;
	}
	
	Float_t tagged  = (tag_glob>0);
	Float_t taggedm = (tag_pre>0);  // tagged by mass cut only
	
	// *** write common information	
	if (fQAEvent)
	{
		ntp->Column("ev",		(Float_t) 	fEvtCount,	0.0f);
		ntp->Column("run",  	(Float_t) 	fRunNum,	0.0f);
		ntp->Column("mode",		(Float_t)	fMode,		0.0f);
		ntp->Column("ecm",		(Float_t)	fEcm,		0.0f);

		fQA->qaP4("beam", fIniP4, ntp);
		ntp->Column("primvx",   (Float_t)   fPrimVtx.X(), 0.0f);
		ntp->Column("primvy",   (Float_t)   fPrimVtx.Y(), 0.0f);
		ntp->Column("primvz",   (Float_t)   fPrimVtx.Z(), 0.0f);
		ntp->Column("primvqa",  (Float_t)   fPrimVtxQa  , 0.0f);
		
		// write tags of individual modes
		for (TrigIt it=fSTTriggers.begin(); it!=fSTTriggers.end(); ++it)
		{
			PndSoftTriggerLine *tl = it->second;
			if ( tl->GetTagActive() ) ntp->Column("tag"+tl->GetName(), (Float_t) tl->GetNTagged(), 0.0f);
		}

		ntp->Column("tagall",	(Float_t)	tag_glob,	0.0f);
		ntp->Column("tagpre",	(Float_t)	tag_pre,	0.0f);
		ntp->Column("tag",		(Float_t)	tagged,		0.0f);
		ntp->Column("tagm",		(Float_t)	taggedm,	0.0f);
		
		fQA->qaEventShape("es", fEventShape, ntp);
		
		// replace PID mult values from fEventShape by actual counts with individual algorithms
		ntp->Column("eslnpide", (Float_t)  fPidMult_025[0],		0.0f );
		ntp->Column("eslnpidmu",(Float_t)  fPidMult_025[1],		0.0f );
		ntp->Column("eslnpidpi",(Float_t)  fPidMult_025[2],		0.0f );
		ntp->Column("eslnpidk", (Float_t)  fPidMult_025[3],		0.0f );
		ntp->Column("eslnpidp", (Float_t)  fPidMult_025[4],		0.0f );
		
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
			
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			n->Column("run",  	(Float_t) fRunNum,		0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column("mode",	(Float_t) fMode,		0.0f);
			n->Column("ksmean", (Float_t) fKs0Mean,		0.0f);
			n->Column("kssig", 	(Float_t) fKs0Sigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, n);
			fQA->qaKs0("ks", fKs0Cands[i], n);
			
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
			
			npi0->Column("tag", 	(Float_t) tag,			0.0f);
			npi0->Column("nsig", 	(Float_t) nsig,			0.0f);
			npi0->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			npi0->Column("run",  	(Float_t) fRunNum,		0.0f);
			npi0->Column("num", 	(Float_t) i,			0.0f);
			npi0->Column("mode",	(Float_t) fMode,		0.0f);
			npi0->Column("pi0mean", (Float_t) fPi0Mean,		0.0f);
			npi0->Column("pi0sig", 	(Float_t) fPi0Sigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, npi0);
			fQA->qaPi0("pi0", fPi0Cands[i], npi0);
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
			
			neta->Column("tag", 	(Float_t) tag,			0.0f);
			neta->Column("nsig", 	(Float_t) nsig,			0.0f);
			neta->Column("ev",  	(Float_t) fEvtCount,	0.0f);
			neta->Column("run",  	(Float_t) fRunNum,		0.0f);
			neta->Column("num", 	(Float_t) i,			0.0f);
			neta->Column("mode",	(Float_t) fMode,		0.0f);
			neta->Column("etamean", (Float_t) fEtaMean,		0.0f);
			neta->Column("etasig", 	(Float_t) fEtaSigma,	0.0f);
			
			fQA->qaP4("beam", fIniP4, neta);
			fQA->qaPi0("eta", fEtaCands[i], neta);
			
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
//                       0            1             2           3           4          5           6          7           8         9         
//TString fSTnames[]={"eslnpide",  "eslnpidmu", "eslnpidpi", "eslnpidk", "eslnpidp", "esthr",    "esapl",   "esfw1",   "esnpart", "esptmax",

//                       10           11            12          13          14         15          16         17          18        19  
//                    "detemcsum", "detemcmax", "p",         "pt",       "pcm",      "tht",      "d0pt",    "d1pt",    "d0pidk",  "d1tht",
                      
//                       20,          21            22          23          24         25          26         27          28        29
//				      "mmiss",     "essumpt",   "essumptcl", "d0pcm",    "d1p",      "thtcm",    "ecm",     "esfw4",   "esfw2",   "esptmin", 
				       
//                       30,          31            32          33          34         35          36         37          38        39
//				      "espmin",    "d0pide",    "d1pide",    "d0pidpi",  "d1pidpi",  "essumptc", "esfw5",   "essumpc", "d1pidk",  "oang",
				       
//                       40,          41            42          43          44         45          46         47          48        49
//                    "espmax",    "essumenl",  "d2pidk",   "d3pidk",   "essumpcl", "d0pidmu", "d1pidmu", "essumen", "d0tht",     "d0p" };

	int i=0;
	// don't use PID mult values from fEventShape (based on AllCands and only one algo)
	for (i=0;i<5;++i) fSTVarArray[i] = fPidMult_025[i]; 
		
	fSTVarArray[ 5] = fEventShape->Thrust();
	fSTVarArray[ 6] = fEventShape->Aplanarity();
	fSTVarArray[ 7] = fEventShape->FoxWolfMomR(1);
	fSTVarArray[ 8] = fEventShape->NParticles();
	fSTVarArray[ 9] = fEventShape->Ptmax();

	fSTVarArray[10] = fEventShape->DetEmcSum();
	fSTVarArray[11] = fEventShape->DetEmcMax();
	
	fSTVarArray[21] = fEventShape->PtSumLab();
	fSTVarArray[22] = fEventShape->ChrgPtSumLab();
	
	fSTVarArray[27] = fEventShape->FoxWolfMomR(4);
	fSTVarArray[28] = fEventShape->FoxWolfMomR(2);
	
	fSTVarArray[29] = fEventShape->Ptmin();	
	fSTVarArray[30] = fEventShape->PminCms();	
	fSTVarArray[35] = fEventShape->ChrgPtSumCms();	
	fSTVarArray[36] = fEventShape->FoxWolfMomR(5);	
	fSTVarArray[37] = fEventShape->ChrgPSumCms();	
	
	fSTVarArray[40] = fEventShape->PmaxCms();	
	fSTVarArray[41] = fEventShape->NeutESumLab();
	fSTVarArray[44] = fEventShape->ChrgPSumLab();
	fSTVarArray[47] = fEventShape->NeutESumCms();

}

// -------------------------------------------------------------------------
// Fill the array of candidate specific variables (called for every candidate)
// here the connection between the variable indices and the values is made

void PndSoftTriggerTask::FillVarArray(RhoCandidate *c)
{
//                       0            1             2           3           4          5           6          7           8         9         
//TString fSTnames[] = {"eslnpide",  "eslnpidmu", "eslnpidpi", "eslnpidk", "eslnpidp", "esthr",    "esapl",   "esfw1",   "esnpart", "esptmax",

//                       10           11            12          13          14         15          16         17          18        19  
//                    "detemcsum", "detemcmax", "p",         "pt",       "pcm",      "tht",      "d0pt",    "d1pt",    "d0pidk",  "d1tht",
                      
//                       20,          21            22          23          24         25          26         27          28        29
//				      "mmiss",     "essumpt",   "essumptcl", "d0pcm",    "d1p",      "thtcm",    "ecm",     "esfw4",   "esfw2",   "esptmin", 
				       
//                       30,          31            32          33          34         35          36         37          38        39
//				      "espmin",    "d0pide",    "d1pide",    "d0pidpi",  "d1pidpi",  "essumptc", "esfw5",   "essumpc", "d1pidk",  "oang",
				       
//                       40,          41            42          43          44         45          46         47          48        49
//                    "espmax",    "essumenl",  "d2pidk",   "d3pidk",   "essumpcl", "d0pidmu", "d1pidmu", "essumen", "d0tht",     "d0p" };

	TVector3 p4boost = fIniP4.BoostVector();

	TLorentzVector l=c->P4();
	TLorentzVector lcm = l;
	lcm.Boost(-p4boost);
	
	int nd = c->NDaughters();
	TLorentzVector ld[4], ldcm[4];
	
	for (int i=0;i<4;++i)
		if (nd>i) 
		{
			ld[i]    = c->Daughter(i)->P4();
			ldcm[i]  = ld[i];
			ldcm[i].Boost(-p4boost);
		}
	
	//PndPidCandidate *mic = (PndPidCandidate*)c->GetRecoCandidate();
	
	fSTVarArray[12] = l.P();
	fSTVarArray[13] = l.Pt();
	fSTVarArray[14] = lcm.P();
	fSTVarArray[15] = l.Theta();
	fSTVarArray[16] = nd>0 ? ld[0].Pt() : -999.0;
	fSTVarArray[17] = nd>1 ? ld[1].Pt() : -999.0;
	fSTVarArray[18] = nd>0 ? c->Daughter(0)->GetPidInfo(3) : -999.0;
	fSTVarArray[19] = nd>1 ? ld[1].Theta() : -999.0;
	fSTVarArray[20] = (fIniP4-l).M();
	fSTVarArray[23] = ldcm[0].P();
	fSTVarArray[24] = ld[1].P();
	fSTVarArray[25] = lcm.Theta();
	fSTVarArray[26] = lcm.E();
	
	fSTVarArray[31] = nd>0 ? c->Daughter(0)->GetPidInfo(0) : -999.0;
	fSTVarArray[32] = nd>1 ? c->Daughter(1)->GetPidInfo(0) : -999.0;
	fSTVarArray[33] = nd>0 ? c->Daughter(0)->GetPidInfo(2) : -999.0;
	fSTVarArray[34] = nd>1 ? c->Daughter(1)->GetPidInfo(2) : -999.0;
	fSTVarArray[38] = nd>1 ? c->Daughter(1)->GetPidInfo(3) : -999.0;
	fSTVarArray[39] = nd>1 ? ld[0].Vect().Angle(ld[1].Vect()) : -999.0;
	fSTVarArray[42] = nd>2 ? c->Daughter(2)->GetPidInfo(3) : -999.0;
	fSTVarArray[43] = nd>3 ? c->Daughter(3)->GetPidInfo(3) : -999.0;
	fSTVarArray[45] = nd>0 ? c->Daughter(0)->GetPidInfo(1) : -999.0;
	fSTVarArray[46] = nd>1 ? c->Daughter(1)->GetPidInfo(1) : -999.0;
	fSTVarArray[48] = ld[0].Theta();
	fSTVarArray[49] = ld[0].P();
}

// -------------------------------------------------------------------------

bool PndSoftTriggerTask::AcceptCandidate(int mode, RhoCandidate *c, RhoParticleSelectorBase *sel)
{
	int mcode = fSTencode[fSTModeIndex]*1000+mode;	
	
	if (fVerbose>1) cout <<"AcceptCandidate : mode="<<mode<<" mcode="<<mcode;

	// no selection defined for this mode
	if ( fSTSelmap.find(mcode) == fSTSelmap.end() ) {if (fVerbose>1) cout <<endl;return false;} 
	if (fVerbose>1) cout <<" found cut set";
	// not accepted by final (mass) selector
	if ( sel && !(sel->Accept(c)) ) {if (fVerbose>1) cout <<endl;return false;}
	
	if (fVerbose>1) cout <<" accepted by precuts"<<endl;
	
	STCutSet cs = fSTSelmap[mcode];
	FillVarArray(c);
	
	bool acc = true;
	
	for (int i=0;i<cs.ncut;++i)
	{
		if (fVerbose>1) cout <<"  -> checking var["<<cs.varid[i]<<"] ("<<fSTVarArray[cs.varid[i]]<<") "<<fSTOps[cs.op[i]]<<" "<<cs.cutval[i]<<endl;
		switch (cs.op[i]) // which operator is used for this cut?
		{
		case 0:	// check var > value
			if ( !(fSTVarArray[cs.varid[i]]>cs.cutval[i]) ) acc=false; 
			break;
		case 1:	// check var == value
			if ( !(fSTVarArray[cs.varid[i]]==cs.cutval[i]) ) acc=false; 
			break;
		case 2:	// check var < value
			if ( !(fSTVarArray[cs.varid[i]]<cs.cutval[i]) ) acc=false; 
			break;
		default : 
			acc=false; 
			break;
		}
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
	
/*	switch (nd) 
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
		if (cc) l.Combine(fPidList[aidx[0]], fPidList[aidx[1]], fPidList[aidx[2]], fPidList[aidx[3]], amothpdg); 
		break;

	case 5: 
		l.Combine(fPidList[idx[0]], fPidList[idx[1]], fPidList[idx[2]], fPidList[idx[3]], fPidList[idx[4]], mothpdg); 
		if (cc) l.Combine(fPidList[aidx[0]], fPidList[aidx[1]], fPidList[aidx[2]], fPidList[aidx[3]], fPidList[aidx[4]], amothpdg); 
		break;
	default: return 0;
	}*/
	
	return l.GetLength();
}


// -------------------------------------------------------------------------
// *** General common tagging methode
int PndSoftTriggerTask::TagMode(PndSoftTriggerLine *tl, int &npre)
{
	// *** counter for full selection accepted cands
	int nacc = 0; 
	
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
		
		// full selection
		if (fApplyFullSelection>0) acc = AcceptCandidate(mode, l[i], sel);
		// simple mass window selection
		else acc = sel->Accept(l[i]);
		
		// for full selection in open mode (=2), trigger w/o detailed cuts are accepted based on mass window only 
		if ( !acc && fApplyFullSelection==2 && fSTSelmap.find(fSTencode[fSTModeIndex]*1000+mode) == fSTSelmap.end() )
			acc = sel->Accept(l[i]);
				
		if (acc) nacc++;
			
		if (n)
		{
			fQA->qaComp(prefix, l[i], n, &fPrimVtx);
			fQA->qaEventShapeShort("es", fEventShape, n);
			// replace PID mult values from event shape by actual counts with individual algos
			n->Column("eslnpide", (Float_t)  fPidMult_025[0],		0.0f );
			n->Column("eslnpidmu",(Float_t)  fPidMult_025[1],		0.0f );
			n->Column("eslnpidpi",(Float_t)  fPidMult_025[2],		0.0f );
			n->Column("eslnpidk", (Float_t)  fPidMult_025[3],		0.0f );
			n->Column("eslnpidp", (Float_t)  fPidMult_025[4],		0.0f );
			
			fQA->qaP4("beam", fIniP4, n);
			n->Column("primvx",   (Float_t)   fPrimVtx.X(), 0.0f);
			n->Column("primvy",   (Float_t)   fPrimVtx.Y(), 0.0f);
			n->Column("primvz",   (Float_t)   fPrimVtx.Z(), 0.0f);
			n->Column("primvqa",  (Float_t)   fPrimVtxQa  , 0.0f);

			Float_t nsig = (Float_t) fabs(l[i]->Mass()-mean)/sigma;
			Float_t tag  = nsig < tl->GetTagNSig();
			
			if (tag>0) npre++;
			
			Float_t mmiss = (fIniP4-(l[i]->P4())).M();
			
			n->Column("ev",  	(Int_t)   fEvtCount,	0);
			n->Column("run",  	(Int_t)   fRunNum,		0);
			n->Column("mode",	(Int_t)   fMode,		0);
			n->Column("recmode",(Int_t)   fRecoilMode,  0);
			n->Column("reccnt", (Int_t)   fRecoilCnt,   0);
			n->Column("mmiss",	(Float_t) mmiss,		0.0f);
			n->Column("tag", 	(Float_t) tag,			0.0f);
			n->Column("nsig", 	(Float_t) nsig,			0.0f);
			n->Column("num", 	(Float_t) i,			0.0f);
			n->Column(prefix+"mean",(Float_t) mean,		0.0f);
			n->Column(prefix+"sig", (Float_t) sigma,	0.0f);
			n->Column("acc",    (Float_t) acc,          0.0f);
			
			n->DumpData();
		}
	}
	
	return nacc;
}

ClassImp(PndSoftTriggerTask)
