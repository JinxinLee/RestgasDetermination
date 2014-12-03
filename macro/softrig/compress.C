#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TEventList.h"
#include "TDirectory.h"
#include <iostream>
#include "TObjArray.h"
#include "TRegexp.h"
#include <vector>

typedef std::vector<TString> StrVec;

int SplitString(TString s, TString delim, StrVec &toks)
{
	toks.clear();
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
	{
		TString token = (((TObjString*)tok->At(i))->String()).Strip(TString::kBoth);
		toks.push_back(token);
	}
	return toks.size();
}

void compress(TString ntp, TString fnamein, TString fnameout, TString bnames="", TString precut="tag" )
{
	if (bnames=="*" && precut=="") 
	{
		cout <<"Nothing to compress. Exiting."<<endl;
		return;
	}
	
	TString smode = ntp(1,ntp.Length());
	int mode = smode.Atoi()/10;
	bool dst = (mode == 11 || mode == 13 || mode == 15); 
	
	// default branches for SoftTrigger	
	if (bnames=="")
	{
		bnames += "ev mode run recmode reccnt nsig mmiss xm xmct";
		bnames += " es* *p *tht *pcm *thtcm *pt *ang *pid* *poc*";
		if (dst) bnames += " xmdif";
		
		// vetos
		bnames +=" !*beam* !*vx !*vy !*vz !t* !*pdg* !*pocmag tag";
	}
	
	StrVec toks;
	SplitString(bnames," ", toks);
	
	TFile *fi = new TFile(fnamein,"READ");
	TTree *ti = (TTree*) fi->Get(ntp);
	
	cout <<"Converting tree '"<<ntp<<"': "<<fnamein<<" ("<<ti->GetNbranches()<<" br, "<<ti->GetEntriesFast()<<" ev) ... "<<flush;

	ti->Draw(">>el",precut);
	TEventList *el = (TEventList*)gDirectory->Get("el");
	
 	ti->SetBranchStatus("*",0);
	for (int i=0;i<toks.size();++i)
 	{
 		if (toks[i].BeginsWith("!"))
 			ti->SetBranchStatus(TString(toks[i](1,100)),0);
 		else
 			ti->SetBranchStatus(toks[i].Data(),1);
 	}
	
	ti->SetEventList(el);
	
	TFile *fo = new TFile(fnameout, "RECREATE");	
	TTree *to = ti->CopyTree("");
//	to->SetEventList(el);
	
	cout <<"to "<<fnameout<<" ("<<to->GetNbranches()<<" br, "<<to->GetEntriesFast()<<" ev) "<<endl;
	
	fo->Write();
	fo->Close();
	fi->Close();
}
