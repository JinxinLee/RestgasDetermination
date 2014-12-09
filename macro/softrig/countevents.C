#include <algorithm>
#include <map>
#include <utility>
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TROOT.h"
#include "TEventList.h"
#include "TDirectory.h"

#define MAX 1000
#define BINS 100
#define NCAN 3

typedef std::map<int, int> CountMap;

CountMap evcnt;

Int_t ev, run, mode, rec, nbranch;

// ---------------------------------------------------------------

int uid(int lev, int lrun, int lmode)
{
	return lev+10000*lrun+(((lmode/100)%10)*20+lmode%10)*100000;
}

// ---------------------------------------------------------------

void countevents(TString fname, TString ntp, TString cut)
{
	TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	t->Draw(">>el",cut);
	TEventList *el = (TEventList*)gDirectory->Get("el");
	
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	t->SetBranchAddress("recmode",&rec);
	
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("recmode",1);
		
	evcnt.clear();
	
	for (int i=0;i<el->GetN();++i)
	{
		t->GetEntry(el->GetEntry(i));
		evcnt[uid(ev,run,mode)]+=1;
	}
	t->SetBranchStatus("*",1);

	printf("EVT = %d   CAND = %d\n",evcnt.size(), el->GetN());
}
