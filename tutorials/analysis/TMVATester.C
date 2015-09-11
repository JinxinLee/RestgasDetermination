#include "TMVA/Reader.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TString.h"
#include "TRegexp.h"
#include "TEventList.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TSystem.h"

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

TString getFromCut(TString vars)
{
	TString toks[50];
	int n=SplitString(vars, "&&", toks, 50);
	TRegexp rvar("[_a-zA-Z][_a-zA-Z0-9]*");
	cout <<"n="<<n<<endl;
	TString res="";
	
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
TString getFromWeightFile(TString wfile)
{
	TString tmp=gSystem->GetFromPipe("grep Expression "+wfile);
	TString v[30];
	int n=SplitString(tmp,"/>",v,30);
	TString res="";
	TRegexp r("Expression=\"[_a-zA-Z0-9]+\"");
	
	for (int i=0;i<n;++i)
	{
		TString exp = (v[i])(r);
		exp.ReplaceAll("Expression=","");
		exp.ReplaceAll("\"","");
		res+=exp+" ";
	}
		
	res = res.Strip(TString::kBoth);
	return res;
}
// ---------------------------------------------------------------

void TMVATester(TString fname="", TString treename="", TString sigcut="", TString wfile="",  TString precut="")
{
	TString vars="";
	
	if ( fname=="" || treename=="" || wfile=="" || sigcut==""  ) 
	{
		cout << "USAGE:\n";
		cout << "TMVATester.C( <input>, <tree>, <sigcut>, <weights>, [precut] )\n\n";
		cout << "   <input>   : input file name containing TTree <tree>\n";
		cout << "   <tree>    : name of the TTree containing signal and background\n";
		cout << "   <sigcut>  : cut separating signal from background -> bgcut = !(sigcut)\n";
		cout << "   <weights> : file containing the weights from training, usually stored in weights/...\n";
		cout << "   [precut]  : optional precut before training; should be the same as for training!'\n\n";
		return;
	}
	
	if (vars.Contains("&&")) vars = getFromCut(vars);
	if (vars=="") vars = getFromWeightFile(wfile);
	cout <<"Vars : "<<vars<<endl;
	
	TString bkgcut="!("+sigcut+")";

	if (precut!="")
	{
		cout <<"Precut : "<<precut<<endl;
		sigcut += "&&" + precut;
		bkgcut += "&&" + precut;
	}
	
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
	
	// apply precut cut and create signal and background event lists
	t->Draw(">>els",sigcut);
	t->Draw(">>elb",bkgcut);
	TEventList *els=(TEventList*)gDirectory->Get("els");
	TEventList *elb=(TEventList*)gDirectory->Get("elb");
	
	int Ns = els->GetN();
	int Nb = elb->GetN();
	
	TH1F *hs=new TH1F("hs","network output: signal(blue), bkg(red)",50,-1,1);
	TH1F *hb=new TH1F("hb","network output background",50,-1,1);
	hb->SetLineColor(2);

	TString friendname = fname;
	friendname.ReplaceAll(".root","_tmvaout.root");
	TFile *ftm = new TFile(friendname,"recreate");
	TNtuple *tf = new TNtuple("tf","tf","tmvaout");

	
	for (int i=0;i<t->GetEntriesFast();++i)
	{
		t->GetEntry(i);
		for (int j=0;j<Nbr;++j) 
		{
			Float_t var=0;
			switch (types[j]) {case 2: var = (Float_t)bbranch[j]; break; case 1: var = (Float_t) ibranch[j]; break; default: var=fbranch[j]; }
			freader[j] = var;
		}
		float resp=reader->EvaluateMVA("BDT");
		tf->Fill(resp);
	}	
	
	tf->Write();
	ftm->Close();

	// loop through signal event list
	for (int i=0;i<Ns;++i)
	{
		t->GetEntry(els->GetEntry(i));
		
		for (int j=0;j<Nbr;++j) 
		{
			Float_t var=0;
			switch (types[j]) {case 2: var = (Float_t)bbranch[j]; break; case 1: var = (Float_t) ibranch[j]; break; default: var=fbranch[j]; }
			freader[j] = var;
		}
		double resp=reader->EvaluateMVA("BDT");
		hs->Fill(resp);
	}	
	// loop through backgound event list
	for (int i=0;i<Nb;++i)
	{
		t->GetEntry(elb->GetEntry(i));
		
		for (int j=0;j<Nbr;++j) 
		{
			Float_t var=0;
			switch (types[j]) {case 2: var = (Float_t)bbranch[j]; break; case 1: var = (Float_t) ibranch[j]; break; default: var=fbranch[j]; }
			freader[j] = var;
		}
		double resp=reader->EvaluateMVA("BDT");
		hb->Fill(resp);
	}	
	
	
	// create the plots
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1200,600);
	c1->Divide(2,1);
	c1->cd(1);
	hs->Scale(1./hs->GetEntries());
	hb->Scale(1./hb->GetEntries());
	double maxi = max(hs->GetMaximum(), hb->GetMaximum())*1.05;
	
	hs->SetMaximum(maxi);
	hb->SetMaximum(maxi);
	hs->DrawCopy();
	hb->DrawCopy("same");
	
	c1->cd(2);
	double sums=1., sumb=0.;
	
	TGraph *gr=new TGraph();
	
	for (int i=0;i<=hs->GetNbinsX();++i)
	{
		sums-=hs->GetBinContent(i);
		sumb+=hb->GetBinContent(i);
		gr->SetPoint(i,sumb,sums);
	}
	gr->Draw("ALP");	
	gr->SetMarkerStyle(20);
	gr->SetMarkerSize(0.7);
	gr->GetHistogram()->SetTitle("ROC curve");
	gr->GetHistogram()->SetXTitle("Background suppression");
	gr->GetHistogram()->SetYTitle("Signal Efficiency");
}


