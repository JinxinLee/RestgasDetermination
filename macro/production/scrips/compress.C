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

int COMSplitString(TString s, TString delim, StrVec &toks)
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

int compress(TString ntp="", TString fnamein="", TString fnameout="", TString bnames="", TString precut="" )
{
	if (ntp=="" || fnamein=="" || fnameout=="")
	{
		cout <<"USAGE:\ncompress(TString tree, TString infile, TString outfile, TString bnames, TString precut)\n\n"; 
		cout <<"  tree    : name of the TTree to be compressed.\n";
		cout <<"  infile  : input file name.\n";
		cout <<"  outfile : output file name.\n";
		cout <<"  bnames  : blank separated list of branch variable names to be copied; can make use of name* / *name /*name*; !(*)name(*) excludes variables (default = \"*\").\n";
		cout <<"  precut  : cut to be applied before compressing branches (default = \"\"). Either bnames!=\"*\" or precut!=\"\".\n\n";
	
		return 0;
	}
	
	if (bnames=="*" && precut=="") 
	{
		cout <<"Nothing to compress. Exiting."<<endl;
		return 0;
	}
	
	TFile *fi = new TFile(fnamein,"READ");
	TTree *ti = (TTree*) fi->Get(ntp);
	
	// do preselection
	ti->Draw(">>el",precut);
	TEventList *el = (TEventList*)gDirectory->Get("el");

	int N0 = ti->GetEntries();
	int Ns = el->GetN();
	
	// Separate names of branches to be considered
	StrVec bnam;
	COMSplitString(bnames," ", bnam);
	
	// Enable/disable branches
	ti->SetBranchStatus("*",0);
	for (unsigned int i=0;i<bnam.size();++i)
	  if (bnam[i].BeginsWith("!")) {TString tmp = bnam[i](1,1000); ti->SetBranchStatus(tmp,0);}
		else ti->SetBranchStatus(bnam[i],1);
		
	bnam.clear();
	TObjArray* blist = ti->GetListOfBranches();
	for(unsigned int i=0; i<=blist->GetLast(); ++i)
	{
		TString name = ((TBranch*)blist->UncheckedAt(i))->GetName();
		if (ti->GetBranchStatus(name)>0) bnam.push_back(name); 
	}
	
	// print selected branches
	cout<<"**** Pre cut       : \"" << precut << "\"" << endl;
	cout<<"**** Selected      : " << Ns << " / "<< N0 << " entries"<< endl;	
	cout<<"**** Branch pattern: \"" << bnames << "\"" << endl;
	cout<<"**** "<<bnam.size()<<" branches found: \n";
	for (unsigned int i=0;i<bnam.size();++i) printf("%s  ",bnam[i].Data());
	cout <<endl<<endl;
		
	cout <<"Converting tree '"<<ntp<<"': "<<fnamein<<" ("<<ti->GetNbranches()<<" br, "<<ti->GetEntriesFast()<<" ev) ... "<<flush;

	ti->SetEventList(el);
	
	TFile *fo = new TFile(fnameout, "RECREATE");	
	TTree *to = ti->CopyTree("");
	
	cout <<"to "<<fnameout<<" ("<<to->GetNbranches()<<" br, "<<to->GetEntriesFast()<<" ev) "<<endl;
	
	fo->Write();
	fo->Close();
	fi->Close();
    
	return 0;
}
