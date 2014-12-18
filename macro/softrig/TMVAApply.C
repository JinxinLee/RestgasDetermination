#include "TMVA/Reader.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TRegexp.h"
#include "TEventList.h"
#include "TLeaf.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>

#define MAX 1000

Float_t fbranch[MAX], freader[MAX];
Int_t   ibranch[MAX], types[MAX];
Bool_t  bbranch[MAX];

Int_t ev, run, mode, rec, nbranch;

typedef std::map<int, int> CountMap;
typedef std::vector<pair<float, int> > ValueMap;

// ---------------------------------------------------------------

int gettype(TTree *t, TString varname)
{
	if (t->GetBranch(varname)==0) return -1;
	
	TString leaftype = t->GetLeaf(varname)->GetTypeName();
	
	if (leaftype=="Float_t")	 return 0; 
	else if (leaftype=="Int_t")	 return 1;  
	else if (leaftype=="Bool_t") return 2;
	
	return -1;
}

// ---------------------------------------------------------------

int SplitString(TString s, TString delim, TString *toks, int maxtoks)
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

// ---------------------------------------------------------------

int init(TTree *t, TString vars)
{
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	t->SetBranchAddress("recmode",&rec);
	
	TString toks[30];
	int N = SplitString(vars," ",toks,30);
	
	for (int i=0;i<N;++i)
	{
		TString v = toks[i];
		types[i] = gettype(t, v);
		switch (types[i])
		{
			case 0: t->SetBranchAddress(v, &(fbranch[i])); break;
			case 1: t->SetBranchAddress(v, &(ibranch[i])); break;
			case 2: t->SetBranchAddress(v, &(bbranch[i])); break;
		}
		//cout <<toks[i]<<" of type "<<types[i]<<endl;
	}   
	
	return N;
}

// ---------------------------------------------------------------

int uid(int lev, int lrun, int lmode)
{
	return lev+10000*lrun+(((lmode/100)%10)*20+lmode%10)*100000;
}

// ---------------------------------------------------------------

int countEvents(TTree *t, TEventList &el)
{
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("recmode",1);
		
	CountMap evcnt;
	
	for (int i=0;i<el.GetN();++i)
	{
		t->GetEntry(el.GetEntry(i));
		evcnt[uid(ev,run,mode)]+=1;
	}
	t->SetBranchStatus("*",1);

	return evcnt.size();
}

// ---------------------------------------------------------------

TString getFromCut(TString vars)
{
	TString toks[50];
	int n=SplitString(vars, "&&", toks, 50);
	TRegexp rvar("[_a-zA-Z][_a-zA-Z0-9]*");
	
	TString res=" ";
	
	for (int i=0;i<n;++i)
	{
		TString v = toks[i](rvar);
		if (v!="")
		{
			if (v=="tag" || res.Contains(" "+v+" ")) continue;
			res+=v+" ";
		}
	}
	
	res = res.Strip(TString::kBoth);
	
	return res;
}
// ---------------------------------------------------------------

void TMVAApply(TString fname, TString vars, TString wfile, Float_t bglevel=0.0001)
{
	if (vars.Contains("&&")) vars = getFromCut(vars);
	cout <<"Vars : "<<vars<<endl;
	
	TString sigcut = "tag&&mode%1000!=900";
	TString bkgcut = "tag&&mode%1000==900";

	// determine ntp name and S0 and B0
	TRegexp rntp("n[0-9][0-9][0-9]");
	TRegexp regS("[0-9]+S");
	TRegexp regB("[0-9]+B");
	
	TString s  = fname(regS); s  = s(0,s.Length()-1);
	TString b  = fname(regB); b  = b(0,b.Length()-1);
	int n0s = s.Atoi()*10000;
	int n0b = b.Atoi()*10000;	
	TString treename = fname(rntp);	
	
	TFile *f = TFile::Open(fname);
	TTree *t =(TTree*) f->Get(treename);
	
	// prepare variables and branch mapping
	int Nbr = init(t,vars);
	TString varname[30];
	SplitString(vars," ",varname,30);
	
	// prepare TMVA reader
	TMVA::Reader *reader = new TMVA::Reader("Silent");
	for (int i=0;i<Nbr;++i) reader->AddVariable(varname[i], &freader[i]);
	reader->BookMVA("BDT",wfile);
	
	// initial number of S and B events
	TEventList elsall("elsall"), elball("elball");
	t->Draw(">>elsall",sigcut);
	t->Draw(">>elball",bkgcut);
	float nsig = countEvents(t,elsall);
	float nbkg = countEvents(t,elball);
	
	// apply tag cut
	t->Draw(">>el","tag");
	TEventList *el=(TEventList*)gDirectory->Get("el");
	
	int N = el->GetN();
	
	CountMap sigcnt, bkgcnt;
	ValueMap sigvals, bkgvals;

	// loop through TTree
	for (int i=0;i<N;++i)
	{
		t->GetEntry(el->GetEntry(i));
		
		if (i%10000==0) cout <<"#"<<flush;
			
		for (int j=0;j<Nbr;++j) 
		{
			Float_t var=0;
			switch (types[j]) {case 2: var = (Float_t)bbranch[j]; break; case 1: var = (Float_t) ibranch[j]; break; default: var=fbranch[j]; }
			freader[j] = var;
		}
		
		float mvares = reader->EvaluateMVA( "BDT");
		
		if (mode%1000==900)	
			bkgvals.push_back( std::make_pair(mvares, uid(ev,run,mode) ));
		else 
			sigvals.push_back( std::make_pair(mvares, uid(ev,run,mode) ));
	}
	
	sort(bkgvals.begin(), bkgvals.end());
	
	int nbgmax = bglevel*n0b, i = bkgvals.size()-1;
	while ((int)bkgcnt.size()<nbgmax && i>=0) bkgcnt[bkgvals[i--].second]+=1;
	double cut = bkgvals[i].first;

	for (i=0;i<(int)sigvals.size();++i)
		if (sigvals[i].first>cut) sigcnt[sigvals[i].second]+=1;
	cout <<endl;
	printf("S : eff = %0.3f     N = %5d / %5d   N0 = %d\n", (double)sigcnt.size()/n0s, (int)sigcnt.size(), (int)nsig, n0s);
	printf("B : eff = %0.5f   N = %5d / %5d   N0 = %d\n",   (double)bkgcnt.size()/n0b, (int)bkgcnt.size(), (int)nbkg, n0b);
	cout <<"\nCUT = "<<cut<<endl;
}


