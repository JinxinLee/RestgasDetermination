// ************************************************************************
//
// PANDA Simple Particle Combiner Class
//
// Offers simple combinatorics
//
// ************************************************************************
//
// Parameters: 
// - fAna   : PndAnalysis instance
// - decay  : decay specification, e.g. "phi -> K+ K-; D_s+ -> phi pi+ cc" (cc indicates charged conjugate; particle used have to be defined beforehand)
// - params : configuration parameters, e.g. "mwin=0.5:mwin(phi)=0.2:emin=0.1:pid=Loose:pide=Tight:algo=PidChargedProbability"
//   - mwin     : mass window for all composites
//   - mwin(X)  : mass window for composite 'X'; X (or cc of X) has to appear in decay 
//   - emin     : minimum energy threshold for neutrals
//   - pmin     : minimum momentum threshold for charged
//   - pid      : PID criterion ("All","VeryLoose","Loose","Tight","VeryTight","Best") for all species
//   - pide, pidmu, pidpi, pidk, pidp : PID criterion for specific species
//   - algo     : PID algorithm (e.g. "PidAlgoEmcBayes;PidAlgoDrc" or "PidChargedProbability") for all species
//   - algoe, ... algop : PID algorithm for specific species
//
//
// K.Goetzen 01/2015
//
// ************************************************************************

// The header file
#include "PndSimpleCombiner.h"

// C++ headers
#include <iostream>
#include <assert.h> 

// ROOT headers
#include "TParticlePDG.h"
#include "TObjArray.h"
#include "TObjString.h"

// RHO headers
#include "RhoMassParticleSelector.h"
#include "RhoEnergyParticleSelector.h"
#include "RhoMomentumParticleSelector.h"
#include "RhoTuple.h"

// PANDA headers
#include "PndAnalysis.h"

using std::cout;
using std::endl;


// -------------------------------------------------------------------------
//  constructor
// -------------------------------------------------------------------------
PndSimpleCombiner::PndSimpleCombiner(PndAnalysis *fAna, TString decay, TString params) : 
	fAnalysis(fAna), fDecay(decay), fGlobParams(params), fNLists(11), fVerbose(0), 
	fEmin(0.), fPmin(0.), fESel(0), fPSel(0)		
{	
	// initialize mapping pdg -> list index and list name
	int pdgcodes[] = {-11, 11, -13, 13, 211, -211, 321, -321, 2212, -2212, 22};
	TString pdgnames[] = {"ElectronMinus", "ElectronPlus", "MuonMinus", "MuonPlus", "PionPlus", "PionMinus", "KaonPlus", "KaonMinus", "ProtonPlus", "ProtonMinus", "Neutral"};
	
	fPdgIdxMap.clear();
	fIdxPdgMap.clear();
	fIdxListNameMap.clear();
	
	for (int i=0;i<11;++i) 
	{
		fPdgIdxMap[pdgcodes[i]]  = i;             // maps pdg code -> list index
		fIdxPdgMap[i]            = pdgcodes[i];   // maps list index -> pdg code
		fIdxListNameMap[i]       = pdgnames[i];   // maps list index -> generic list name (ElectronPlus, PionMinus, ...; see above)
	}
	// set initial pid configuration
	SetPid("All", "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts");
	
	assert(ParseDecay(decay));
	ParseParams(params);
}

// -------------------------------------------------------------------------
// destructor
PndSimpleCombiner::~PndSimpleCombiner()
{
	if (fESel) delete fESel;
	if (fPSel) delete fPSel;

	for (int i=0;i<fDecayInfoArray.size();++i) if (fDecayInfoArray[i].msel) delete fDecayInfoArray[i].msel;
}

// -------------------------------------------------------------------------

int PndSimpleCombiner::SplitString(TString s, TString delim, StringList &toks)
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

void PndSimpleCombiner::InitDecayInfo(SCDecayInfo &info, int pdg, int idx)
{
	info.mpdg  = pdg;     // mother(composite) pdg code
	info.midx  = idx;     // add another list for the new composite
	info.daucc = false;   // in case only the daughters have a cc, but not the mother, we have to care (e.g. etac -> Ks K+ pi-)
	info.mwin  = 0;       // default: no mass selection
	info.msel  = 0;       // default: no mass selection
	info.dpdg.clear();    // pdgs of daughters
	info.didx.clear();    // list index of daughters
}

// -------------------------------------------------------------------------

bool PndSimpleCombiner::ParseDecay(TString decay)
{
	StringList subdec;
	
	int ndec = SplitString(decay, ";", subdec);
	
	// loop over subdecays
	for (int i=0;i<ndec;++i)
	{	
		StringList dectoks;
		// does decay string contain exactly one '->' in the middle of the string?
		subdec[i].ReplaceAll("->",">");
		SplitString(subdec[i],">",dectoks);
				
		if ( dectoks.size()!=2 || !TDatabasePDG::Instance()->GetParticle(dectoks[0]) ) {cout <<"[PndSimpleCombiner] **** ERROR : Invalid decay pattern '"<<subdec[i].Data()<<"'"<<endl; return false;}
		
		// split string again; first token is supposed to be the decaying resonance
		TString curstr = dectoks[0]+" "+dectoks[1];
		SplitString(curstr," ",dectoks);
		
		// too many daughters (>5)
		if (dectoks.size()>6) {cout <<"[PndSimpleCombiner] **** ERROR : Exceeding max number of daughters (5): '"<<subdec[i].Data()<<"'"<<endl; return false;}
		
		SCDecayInfo info;
		InitDecayInfo(info, TDatabasePDG::Instance()->GetParticle(dectoks[0])->PdgCode(), fNLists++);
		
		fPdgIdxMap[info.mpdg]  = info.midx;                     // add new list to the pdg <-> list index maps
		fIdxPdgMap[info.midx]  = info.mpdg;
		
		bool cc=true;
		
		// loop over daughters
		for (int j=1;j<dectoks.size();++j)
		{
			if (dectoks[j]=="nocc") { cc = false; continue; }
			
			if (!TDatabasePDG::Instance()->GetParticle(dectoks[j])) {cout <<"[PndSimpleCombiner] **** ERROR : Unknown particle '"<<dectoks[j].Data()<<"'"<<endl; return false;}
			
			// pdg code of daughter
			int dpdg = TDatabasePDG::Instance()->GetParticle(dectoks[j])->PdgCode();
			// add corresponding index if exists
			if ( fPdgIdxMap.find(dpdg) == fPdgIdxMap.end() ) {cout <<"[PndSimpleCombiner] **** ERROR : Undefined list '"<<dectoks[j].Data()<<"'"<<endl; return false;}
			if ( dpdg == info.mpdg ) {cout <<"[PndSimpleCombiner] **** ERROR : Invalid recursion in '"<<subdec[i].Data()<<"'"<<endl; return false;}
			
			info.dpdg.push_back(dpdg);
			info.didx.push_back(fPdgIdxMap[dpdg]);
		}
		info.ndaug = info.didx.size();
		fDecayInfoArray.push_back(info);
		
		// do we need the cc decay definition
		if (cc)
		{
			// does the cc mode exist?
			// either the mother or the FS has to have a cc not being identical
			// example: 1) D0 -> K- pi+       has cc -> D0bar -> K+ pi-     (different FS and mother)
			//          2) D0 -> KS pi+ pi-   has cc -> D0bar -> KS pi+ pi- (different mother)
			//          3) etac -> KS K- pi+  has cc -> etac -> KS K+ pi-   (different FS)
			//          4) phi -> K+ K-       has _no_ cc!                  (identical FS and mother)
			
			// does the mother have a cc? (case 1 + 2)
			// then we add a new list
			if (!CCInvariant(info.mpdg)) 
			{
				SCDecayInfo ainfo;
				InitDecayInfo(ainfo, AntiPdg(info.mpdg), fNLists++);

				fPdgIdxMap[ainfo.mpdg]  = ainfo.midx;
				fIdxPdgMap[ainfo.midx]  = ainfo.mpdg;
				
				for (int j=0;j<info.dpdg.size();++j)
				{
					int apdg = AntiPdg(info.dpdg[j]);
					if (apdg==-999999 || fPdgIdxMap.find(apdg) == fPdgIdxMap.end() ) {cout <<"[PndSimpleCombiner] **** ERROR : No list for PDG code "<<apdg<<endl; return false;}
					ainfo.dpdg.push_back(apdg);
					ainfo.didx.push_back(fPdgIdxMap[apdg]);
				}
				ainfo.ndaug = info.didx.size();
				fDecayInfoArray.push_back(ainfo);
			}
			// only the daughters have a cc (case 3)
			// then we simply set the daucc switch, which will be taken into account during combinatorics
			else if (!CCInvariant(info.dpdg))
			{
				for (int j=0;j<info.dpdg.size();++j)
				{
					int apdg = AntiPdg(info.dpdg[j]);
					if (apdg==-999999 || fPdgIdxMap.find(apdg) == fPdgIdxMap.end() ) {cout <<"[PndSimpleCombiner] **** ERROR : No list for PDG code "<<apdg<<endl; return false;}
				}
				
				fDecayInfoArray[fDecayInfoArray.size()-1].daucc=true;
			}
		}
	}
	
	return true;
}

// -------------------------------------------------------------------------

bool PndSimpleCombiner::ParseParams(TString params)
{
	StringList parm;
	SplitString(params,":",parm);
	
	for (int i=0;i<parm.size();++i)
	{
		if (parm[i]=="ebrem") 
		{
			fIdxListNameMap[0]="ElectronMinusBrem";
			fIdxListNameMap[1]="ElectronPlusBrem";
			continue;
		}
		
		StringList pair;
		SplitString(parm[i],"=",pair);
		
		if (pair.size()!=2) {cout <<"[PndSimpleCombiner] **** WARNING : Invalid parameter setting '"<<parm[i]<<"' ignored"<<endl; continue;}
		
		// ****
		// global mass window setting
		// can be either : mwin(part)=0.5        -> cut +- 0.25 around nominal mass of part
		// or            : mwin(part)=0.25|0.75  -> cut 0.25 < mpart < 0.75
		// ****
		if (pair[0]=="mwin")
		{
			double window = pair[1].Atof();
			
			for (int j=0;j<fDecayInfoArray.size();++j)
			{
				SCDecayInfo &info = fDecayInfoArray[j];
				if (info.msel) delete info.msel;
				info.mwin = window;
				info.msel = new RhoMassParticleSelector("msel",TDatabasePDG::Instance()->GetParticle(info.mpdg)->Mass(),window);
			}
		}
		// ****
		// mass window for one composite
		// ****
		else if (pair[0].BeginsWith("mwin"))
		{
			// extract particle name from string 'mwin(D0)'
			pair[0] = pair[0](5,pair[0].Length()-6);
			
			if (!TDatabasePDG::Instance()->GetParticle(pair[0])) {cout <<"[PndSimpleCombiner] **** WARNING : Unknown particle type '"<<pair[0]<<"'"<<endl;continue;}
			
			int pdg = TDatabasePDG::Instance()->GetParticle(pair[0])->PdgCode();
			if (fPdgIdxMap.find(pdg) == fPdgIdxMap.end()) {cout <<"[PndSimpleCombiner] **** WARNING : Unknown particle list '"<<pair[0]<<"'"<<endl;continue;}
			
			// set default mean and window
			double mean = TDatabasePDG::Instance()->GetParticle(pdg)->Mass(), window = 0;
			
			// range setting like 1.2|1.6 ?
			if (pair[1].Contains("|"))
			{
				double lower = TString((pair[1])(0,pair[1].Index("|"))).Atof();
				double upper = TString((pair[1])(pair[1].Index("|")+1,1000)).Atof();
				
				mean   = (upper+lower)/2.;
				window = upper-lower;
				
				if (mean<0 || window <=0) {cout <<"[PndSimpleCombiner] **** WARNING : Invalid mass window defintion '"<<parm[i]<<"'"<<endl;continue;}
			}
			else window = pair[1].Atof();
			
			for (int j=0;j<fDecayInfoArray.size();++j)
			{
				SCDecayInfo &info = fDecayInfoArray[j];
				// only set for this particle type
				if (abs(info.mpdg) == abs(pdg))
				{
					if (info.msel) delete info.msel;
					info.mwin = window;
					info.msel = new RhoMassParticleSelector("msel",mean,window);
				}
			}	
		}
		
		// ****
		// check for pid setting
		// ****
		if (pair[0] == "pid")   SetPid(pair[1]);
		
		if (pair[0] == "pide")  SetPidElectron(pair[1]);
		if (pair[0] == "pidmu") SetPidMuon(pair[1]);
		if (pair[0] == "pidpi") SetPidPion(pair[1]);
		if (pair[0] == "pidk")  SetPidKaon(pair[1]);
		if (pair[0] == "pidp")  SetPidProton(pair[1]);
		
		if (pair[0] == "algo") SetPid("",pair[1]);
		
		if (pair[0] == "algoe")  SetPidElectron("",pair[1]);
		if (pair[0] == "algomu") SetPidMuon("",pair[1]);
		if (pair[0] == "algopi") SetPidPion("",pair[1]);
		if (pair[0] == "algok")  SetPidKaon("",pair[1]);
		if (pair[0] == "algop")  SetPidProton("",pair[1]);
		
		// ****
		// E_min or p_min
		// ****
		if (pair[0] == "emin") {fEmin = pair[1].Atof(); fESel = new RhoEnergyParticleSelector("eSel",50.+fEmin,100.);}
		if (pair[0] == "pmin") {fPmin = pair[1].Atof(); fPSel = new RhoMomentumParticleSelector("pSel",50.+fPmin,100.);}
	}
        return kTRUE;
}

// -------------------------------------------------------------------------

void PndSimpleCombiner::SetPid(TString crit, TString algo)
{
	if (crit!="") fIdxPidCritMap.clear();
	if (algo!="") fIdxPidAlgoMap.clear();
	
	SetPidElectron(crit, algo);
	SetPidMuon(crit, algo);
	SetPidPion(crit, algo);
	SetPidKaon(crit, algo);
	SetPidProton(crit, algo);
}

// -------------------------------------------------------------------------

void PndSimpleCombiner::SetPidElectron(TString crit, TString algo) 
{
	if (crit!="") {	fIdxPidCritMap[0] = crit; fIdxPidCritMap[1] = crit;} 
	if (algo!="") { fIdxPidAlgoMap[0] = algo; fIdxPidAlgoMap[1] = algo;}
}

void PndSimpleCombiner::SetPidMuon(TString crit, TString algo)     
{
	if (crit!="") {fIdxPidCritMap[2] = crit; fIdxPidCritMap[3] = crit;} 
	if (algo!="") {fIdxPidAlgoMap[2] = algo; fIdxPidAlgoMap[3] = algo;}
}

void PndSimpleCombiner::SetPidPion(TString crit, TString algo)     
{
	if (crit!="") {fIdxPidCritMap[5] = crit; fIdxPidCritMap[4] = crit;} 
	if (algo!="") {fIdxPidAlgoMap[5] = algo; fIdxPidAlgoMap[4] = algo;}
}

void PndSimpleCombiner::SetPidKaon(TString crit, TString algo)    
{
	if (crit!="") {fIdxPidCritMap[7] = crit; fIdxPidCritMap[6] = crit;}
	if (algo!="") {fIdxPidAlgoMap[7] = algo; fIdxPidAlgoMap[6] = algo;}
}

void PndSimpleCombiner::SetPidProton(TString crit, TString algo)   
{
	if (crit!="") {fIdxPidCritMap[9] = crit; fIdxPidCritMap[8] = crit;}
	if (algo!="") {fIdxPidAlgoMap[9] = algo; fIdxPidAlgoMap[8] = algo;}
}


// -------------------------------------------------------------------------
// fill all generic lists, which are used by any of the composites
void PndSimpleCombiner::FillGenericLists()
{
	// fill all generic lists, which are used by any of the composites
	// loop through composites
	bool filled[11] = {0};  // marker, whether list is already filled
	
	int n = fDecayInfoArray.size();
	for (int i=0;i<n;++i)
	{
		SCDecayInfo info = fDecayInfoArray[i];
		
		for (int j=0;j<info.ndaug;++j)
		{
			int lidx  = info.didx[j];
			
			if (lidx>10 || filled[lidx]) continue; // this list is not generic or already filled
				
			filled[lidx] = true;                   // mark list as filled
			int laidx = fPdgIdxMap[AntiPdg(info.dpdg[j])]; 	// index of antiparticle list (only relevant for charged)

			if (lidx<10) // charged lists
			{
				fAnalysis->FillList(fList[lidx],fIdxListNameMap[lidx]+fIdxPidCritMap[lidx],fIdxPidAlgoMap[lidx]);
				if (fPSel) fList[lidx].Select(fPSel);
				
				// also fill anti pdg generic list for charged generic ...
				fAnalysis->FillList(fList[laidx],fIdxListNameMap[laidx]+fIdxPidCritMap[laidx],fIdxPidAlgoMap[laidx]);
				if (fPSel) fList[laidx].Select(fPSel);
				filled[laidx] = true;              // ... and mark list as filled              
			}
			else if (lidx==10) //neutrals
			{
				fAnalysis->FillList(fList[lidx],fIdxListNameMap[lidx]);
				if (fESel) fList[lidx].Select(fESel);
			}
			if (lidx<11 && fVerbose) 
			{
				cout <<"idx :"<<lidx<<" pdg:"<<fIdxPdgMap[lidx]<<" N:"<<fList[lidx].GetLength()<<endl;
				if (lidx<10) cout <<"aidx:"<<laidx<<" pdg:"<<fIdxPdgMap[laidx]<<" N:"<<fList[laidx].GetLength()<<endl;
			}
		}
	}
}

// -------------------------------------------------------------------------
// check whether a list of pdg codes is its cc state (e.g. KS pi+ pi-)
// idea: if the sum of all pdg codes from particles having an anti-particle
//       is 0 and #positive codes = #negative codes, then its true 
bool PndSimpleCombiner::CCInvariant(std::vector<int> &vpdg)
{
	int sum=0, nminus=0, nplus=0;
	
	for (int i=0;i<vpdg.size();++i)
	{
		if (!CCInvariant(vpdg[i])) 
		{
			sum += vpdg[i];
			if (vpdg[i]>0) nplus++;
			else nminus++;
		}
	}
	return (sum==0 && nplus==nminus);
}

// -------------------------------------------------------------------------
// Returns pdg code of anti-particle 
int  PndSimpleCombiner::AntiPdg(int pdg)
{
	if (TDatabasePDG::Instance()->GetParticle(pdg))
	{
		if (!TDatabasePDG::Instance()->GetParticle(pdg)->AntiParticle()) return pdg;
		else return TDatabasePDG::Instance()->GetParticle(pdg)->AntiParticle()->PdgCode();
	}
	return -999999;
} 


// -------------------------------------------------------------------------
// Prints the configuration
void PndSimpleCombiner::Print()
{
	cout <<endl<<"[PndSimpleCombiner] **** Configuration"<<endl<<"---------------------------"<<endl;
	
	cout <<"Neutrals E_min = "<<fEmin<<endl;
	cout <<"Charged  p_min = "<<fPmin<<endl<<endl;
	
	// fill all generic lists, which are used by any of the composites
	// loop through composites
	int n = fDecayInfoArray.size();
	for (int i=0;i<n;++i)
	{
		SCDecayInfo info = fDecayInfoArray[i];
		cout <<"Decay "<<i<<" : " << TDatabasePDG::Instance()->GetParticle(info.mpdg)->GetName()<<"("<<info.mpdg<<"/"<<info.midx<<") -> ";
		for (int j=0;j<info.dpdg.size();++j) cout << TDatabasePDG::Instance()->GetParticle(info.dpdg[j])->GetName()<<"("<<info.dpdg[j]<<"/"<<info.didx[j]<<") ";
		
		cout <<"  daucc: "<<info.daucc;
		cout <<"  mass window:"<<info.mwin;
		cout <<endl;
	}
	cout <<endl;
	for (int i=0;i<10;++i)
	{
		printf("%-16s : %s, %s\n",fIdxListNameMap[i].Data(),fIdxPidCritMap[i].Data(),fIdxPidAlgoMap[i].Data());
	}
	
	cout <<endl;
}

// -------------------------------------------------------------------------
// The central routine doing the combinatorics
// Needs to be called after every PndAnalysis::GetEvent
void PndSimpleCombiner::Combine()
{
	// fill list of final state particles
	FillGenericLists();
	
	// loop through list definitions and produce composites
	int n = fDecayInfoArray.size();
	for (int i=0;i<n;++i)
	{
		SCDecayInfo info = fDecayInfoArray[i];
		CombineList(fList[info.midx], info.mpdg, info.didx);
		
		// do we need to add the cc FS?
		if (info.daucc)
		{
			// create index list with cc daughters
			std::vector<int> aidx;
			for (int j=0;j<info.didx.size();++j)
			{
				int apdg = AntiPdg(info.dpdg[j]);
				aidx.push_back(fPdgIdxMap[apdg]);			
			}
			// create temporary list with the cc combinatorics
			RhoCandList l;
			CombineList(l, info.mpdg, aidx);
			// append it to the original list
			fList[info.midx].Append(l);
		}
		// if there is a mass selector connected, apply it
		if (info.msel) fList[info.midx].Select(info.msel);
	}
}

// -------------------------------------------------------------------------
// create a list based on indices
int PndSimpleCombiner::CombineList(RhoCandList &l, int mpdg, std::vector<int> &idx)
{
	l.Cleanup();
	
	int nd = idx.size();
	
	switch (nd) 
	{
	case 2: l.Combine(fList[idx[0]], fList[idx[1]], mpdg); break;	
	case 3: l.Combine(fList[idx[0]], fList[idx[1]], fList[idx[2]], mpdg); break;	
	case 4: l.Combine(fList[idx[0]], fList[idx[1]], fList[idx[2]], fList[idx[3]], mpdg); break;
	case 5:	l.Combine(fList[idx[0]], fList[idx[1]], fList[idx[2]], fList[idx[3]], fList[idx[4]], mpdg); break;
	}
	
	return l.GetLength();
}

// -------------------------------------------------------------------------
// access lists by name, pdg or index

bool PndSimpleCombiner::GetList(RhoCandList &l, TString comp)
{
	if (!TDatabasePDG::Instance()->GetParticle(comp)) return false;
	
	return GetList(l, TDatabasePDG::Instance()->GetParticle(comp)->PdgCode());
}

// -------------------------------------------------------------------------

bool PndSimpleCombiner::GetList(RhoCandList &l, int pdg)
{
	l.Cleanup();
	
	if (fPdgIdxMap.find(pdg) == fPdgIdxMap.end()) return false;
	
	l = fList[fPdgIdxMap[pdg]];
	return true;
}

// -------------------------------------------------------------------------

bool PndSimpleCombiner::GetListN(RhoCandList &l, int idx)
{
	l.Cleanup();
	
	if (idx>=0 && idx<GetNLists()) 
	{
		l = fList[idx+11];
		return true;
	}
	
	return false;
}


