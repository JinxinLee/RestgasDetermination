#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TEventList.h"
#include "TDirectory.h"
#include <iostream>
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TObjArray.h"
#include "TPRegexp.h"
#include "TRegexp.h"
#include <map>
#include <utility>
#include <algorithm>

#define MAX 1000
#define BINS 500
#define NCAN 3

std::map<int, int> evcnt;
std::map<int, int> evcntrec[10];
std::map<TString, TString> mctvar;

double signi[MAX];
int idx[MAX];
TString vars[MAX];
TString cuts[MAX];
double cut[MAX];

double minh[MAX], maxh[MAX];

double N0_sig, N0_bg;
double Nsigev, Nbgev;
bool dstarmode;

// ---------------------------------------------------------------

bool mycompare(int i, int j)
{
  return signi[i]>signi[j];
}

// ---------------------------------------------------------------

int uid(int ev, int run, int mode)
{
	return ev+10000*run+(((mode/100)%10)*20+mode%10)*100000;
}

// ---------------------------------------------------------------

int countEvents(TTree *t, TEventList &el)
{
	t->SetEventList(0);
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("recmode",1);
	
	Int_t ev, run, mode, rec;
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	t->SetBranchAddress("recmode",&rec);
	
	evcnt.clear();
	for (int j=0;j<10;++j) evcntrec[j].clear();
	
	for (int i=0;i<el.GetN();++i)
	{
		t->GetEntry(el.GetEntry(i));
		evcnt[uid(ev,run,mode)]+=1;
		if (rec<10) evcntrec[rec][uid(ev,run,mode)]+=1;
	}
	t->SetBranchStatus("*",1);

	return evcnt.size();
}

// ---------------------------------------------------------------

double bestEffEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double supr, int id)
{	
	int i;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus(varname,1);
	
	Bool_t varb;
	Float_t varf, var;
	Int_t ev, run, mode, vari, dtype;
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	
	TString leaftype = t->GetLeaf(varname)->GetTypeName();
	
	if (leaftype=="Float_t")	 { dtype = 0; t->SetBranchAddress(varname,&varf); }
	else if (leaftype=="Int_t")	 { dtype = 1; t->SetBranchAddress(varname,&vari); }
	else if (leaftype=="Bool_t") { dtype = 2; t->SetBranchAddress(varname,&varb); }
	
	std::map<int, int> sigcnt, bgcnt, sigcnt2;	
	std::vector<pair<double, int> > sigvals, bgvals;
	
	int Nbgval = elb.GetN();
	double min = 1e9, max=-1e9;
	
	// prepare the pairs of variable value, eventnumber for signal and count signals
	for (i=0;i<els.GetN();++i)
	{
		t->GetEntry(els.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)varb; break; case 1: var = (Float_t) vari; break; default: var=varf; }
		
		sigvals.push_back(std::make_pair(var, uid(ev,run,mode)  ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)varb; break; case 1: var = (Float_t) vari; break; default: var=varf; }
		
		bgvals.push_back(std::make_pair(var, uid(ev,run,mode) ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
	
	// sort signals by first element = variable value
	sort(bgvals.begin(), bgvals.end());

	i=0;
	bgcnt.clear();
	// find cut for current efficiency requirement left cut
	while ( (bgcnt.size()/(double)Nbgev)<(1-supr) && i<Nbgval ) bgcnt[bgvals[i++].second]+=1;
	double leftcut = bgvals[i].first;
	
	bgcnt.clear();
	i=bgvals.size()-1;
	// find cut for current efficiency requirement right cut
	while ( (bgcnt.size()/(double)Nbgev)<(1-supr) && i>=0) bgcnt[bgvals[i--].second]+=1; 
	double rightcut = bgvals[i].first;
	
	sigcnt.clear();
	sigcnt2.clear();
	
	for (i=0;i<sigvals.size();++i)
	{
		if (sigvals[i].first<leftcut)  sigcnt[sigvals[i].second]+=1;
		if (sigvals[i].first>rightcut) sigcnt2[sigvals[i].second]+=1;
	}
	
	if (max>25) max=25;
	minh[id]=min-(max-min)*0.05; 
	maxh[id]=max+(max-min)*0.05;

	t->SetBranchStatus("*",1);
		
	double lefteff  = sigcnt.size()/Nsigev;
	double righteff = sigcnt2.size()/Nsigev;

	bestcut = rightcut;
	if (lefteff>righteff) 
	{
		if (dtype==0) cuts[id] = TString::Format("%s<%.5f",varname.Data(),leftcut);
		else cuts[id] = TString::Format("%s<=%.1f",varname.Data(),leftcut);

		bestcut = leftcut;
		return lefteff;
	}

	if (dtype==0) cuts[id] = TString::Format("%s>%.5f",varname.Data(),rightcut);
	else cuts[id] = TString::Format("%s>=%.1f",varname.Data(),rightcut);

	return righteff;
}

// ---------------------------------------------------------------

double bestSuppressionEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double eff, int id)
{	
	int i;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus(varname,1);
	
	Bool_t varb;
	Float_t varf, var;
	Int_t ev, run, mode, vari, dtype=0;
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	
	TString leaftype = t->GetLeaf(varname)->GetTypeName();
	
	if (leaftype=="Float_t")	 { dtype = 0; t->SetBranchAddress(varname,&varf); }
	else if (leaftype=="Int_t")	 { dtype = 1; t->SetBranchAddress(varname,&vari); }
	else if (leaftype=="Bool_t") { dtype = 2; t->SetBranchAddress(varname,&varb); }
	
	std::map<int, int> sigcnt, bgcnt, bgcnt2;	
	std::vector<pair<double, int> > sigvals, bgvals;
	
	int Nsigval = els.GetN();
	double min = 1e9, max=-1e9;
	
	// prepare the pairs of variable value, eventnumber for signal and count signals
	for (i=0;i<els.GetN();++i)
	{
		t->GetEntry(els.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)varb; break; case 1: var = (Float_t) vari; break; default: var=varf; }
		sigvals.push_back(std::make_pair(var, uid(ev,run,mode) ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		switch (dtype) {case 2: var = (Float_t)varb; break; case 1: var = (Float_t) vari; break; default: var=varf; }
		bgvals.push_back(std::make_pair(var, uid(ev,run,mode) ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
	
	// sort signals by first element = variable value
	sort(sigvals.begin(), sigvals.end());

	i=0;
	sigcnt.clear();
	// find cut for current efficiency requirement left cut
	while ( (sigcnt.size()/(double)Nsigev)<eff && i<Nsigval ) sigcnt[sigvals[i++].second]+=1;
	double leftcut = sigvals[i].first;
	
	sigcnt.clear();
	i=sigvals.size()-1;
	// find cut for current efficiency requirement right cut
	while ( (sigcnt.size()/(double)Nsigev)<eff && i>=0) sigcnt[sigvals[i--].second]+=1; 
	double rightcut = sigvals[i].first;
	
	bgcnt.clear();
	bgcnt2.clear();
	
	for (i=0;i<bgvals.size();++i)
	{
		if (bgvals[i].first<=leftcut)  bgcnt[bgvals[i].second];
		if (bgvals[i].first>=rightcut) bgcnt2[bgvals[i].second];
	}
	
	if (max>25) max=25;
	minh[id]=min-(max-min)*0.05; 
	maxh[id]=max+(max-min)*0.05;

	t->SetBranchStatus("*",1);
	
	
	double leftsupr = (Nbgev-bgcnt.size())/Nbgev;
	double rightsupr = (Nbgev-bgcnt2.size())/Nbgev;

	if (leftcut!=leftcut || rightcut!=rightcut) return 0;
	
	bestcut = rightcut;
	if (leftsupr>rightsupr) 
	{
		if (dtype==0) cuts[id] = TString::Format("%s<%.5f",varname.Data(),leftcut);
		else cuts[id] = TString::Format("%s<=%.1f",varname.Data(),leftcut);
		
		bestcut = leftcut;
		return leftsupr;
	}
	
	if (dtype==0) cuts[id] = TString::Format("%s>%.5f",varname.Data(),rightcut);
	else cuts[id] = TString::Format("%s>=%.1f",varname.Data(),rightcut);

	return rightsupr;
}

// ---------------------------------------------------------------

TString findcut(TTree *t, TEventList &els, TEventList &elb, double supr, double &bestqa)
{
	int i;
	
	for (i=0;i<MAX;++i) { idx[i]=i; signi[i]=0.;}

	TObjArray* branches = t->GetListOfBranches();	
	bestqa = -999.;
	
	TString bestcut="";
	
	cout <<"000/000"<<flush;
	for(i=0; i<=branches->GetLast(); ++i)
	{
//		cout <<"#"<<flush;
		printf("\b\b\b\b\b\b\b%03d/%03d",i,(int)branches->GetEntries());fflush(stdout);
		TBranch* branch = (TBranch*)branches->UncheckedAt(i);
		vars[i]=branch->GetName(); 
		TString v=vars[i];

		if ( v=="ev" || v=="mode" || v=="run" || v=="nsig" ) continue;
		if ( v.Contains("pdg") || v.Contains("beam") || v.Contains("mct") ) continue;
		if (v.BeginsWith("t") && v!="thr") continue;
		if (v.EndsWith("vx") || v.EndsWith("vy") || v.EndsWith("vz") || v.EndsWith("pocmag")) continue;

		bool ok=false;
		
		if ( v.BeginsWith("es") || v=="mmiss" || v.EndsWith("d0m") ) ok=true;
		if ( v.EndsWith("p") || v.EndsWith("tht") || v.EndsWith("pcm") || v.EndsWith("thtcm") || v.EndsWith("pt") ) ok=true;
		if ( ok || v.EndsWith("ang") || v.Contains("poc") ) ok=true;
		if ( ok || v.Contains("pid") || v.Contains("min") || v.Contains("max") || v.Contains("sum") || v.Contains("fw") ) ok=true;
		if ( ok || v.EndsWith("sph") || v.EndsWith("apl") || v.EndsWith("pla") || v.EndsWith("thr") || v.EndsWith("cir") ) ok=true;
		
		if ( v=="xmdif" && dstarmode ) ok = true;
		if ( v=="xd0m" && dstarmode ) ok = false;
		
		if (!ok) continue;
	
		double qa=0;
		
		if (supr<0)	qa = bestEffEvt(t, vars[i], els, elb, cut[i], -supr, i);
		else        qa = bestSuppressionEvt(t, vars[i], els, elb, cut[i], supr, i);	
		
		if (qa>bestqa) 
		{
			bestqa  = qa;
			bestcut = cuts[i];
		}
	}
	return bestcut;
}

// ---------------------------------------------------------------

void autocutx(TString fname, TString precut="", double supr=0.95, double target=0.0001, double mineff = 0.1, double minreleff = 0.0, int evmult=10000, double norm=1.0, int n0s=-1)
{
// 	gStyle->SetTitleX(0.2);
// 	gStyle->SetTitleY(0.993);
// 	gStyle->SetTitleH(0.07);
// 	gStyle->SetPadTopMargin(0.08);
// 	gStyle->SetTitleBorderSize(0);
	gStyle->SetOptStat(0);
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1500,600);
	c1->Divide(5,2);
	
	TString tagcut = "tag";
	
	TRegexp ren("M[0-9][0-9][0-9]"); 
	TRegexp regntp("n[0-9]+");
	TRegexp regS("[0-9]+S");
	TRegexp regB("[0-9]+B");
	
	TString en = fname(ren);  en = en(1,5); 
	TString s  = fname(regS); s  = s(0,s.Length()-1);
	TString b  = fname(regB); b  = b(0,b.Length()-1);
	
	TString ntp   = fname(regntp);
	TString smode = ntp(1,ntp.Length());
	int mode      = smode.Atoi();
	dstarmode     = (mode>=110 && mode<=119) || (mode>=130 && mode<=138) || (mode>=150 && mode<=151);
	
	if (n0s<0) n0s = s.Atoi();
	int n0b = b.Atoi();
	
	N0_sig = n0s*evmult;
	N0_bg = n0b*evmult;
	
	cout <<"tree '"<<ntp.Data()<<"' with N_sig = "<<N0_sig<<", N_bg = "<<N0_bg<<endl;

	TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	TEventList elsall("elsall"), elball("elball");
	TEventList els("els"), elb("elb");
	
	if (precut=="") precut = tagcut;
	else precut = tagcut+"&&"+precut;
	
	TString sigcut = "xmct";
	TString bgcut  = "!xmct";
	
	t->Draw(">>elsall",tagcut+"&&"+sigcut);
	t->Draw(">>elball",tagcut+"&&"+bgcut);
	float nsig = countEvents(t,elsall);
	float nbg = countEvents(t,elball);
	
	double beff=1., seff=1., rseff=1.;
	
	double inisupr = supr;
	
	TRegexp rnum("[0-9]+\\.[0-9]+$");
	int cnt=1;
	
	bool stop=false;
	TString oldcut=precut;	
		
	while (cnt<20 && !stop && beff>target && seff>=mineff && rseff>=minreleff) 
	{		
		c1->cd(cnt++); //gPad->SetLogy();
		
		t->SetLineColor(1);	t->Draw("xm",precut);
		t->SetLineColor(2);	t->Draw("xm",precut+"&&"+sigcut,"same");
		t->SetLineColor(4);	t->Draw("xm",precut+"&&"+bgcut,"same");
		c1->Update();
				
		t->Draw(">>els",sigcut+"&&"+precut);
		t->Draw(">>elb",bgcut+"&&"+precut);

		Nbgev  = countEvents(t, elb);
		Nsigev = countEvents(t, els);
		
	   	beff  = Nbgev/N0_bg;
	   	seff  = Nsigev/N0_sig;
		rseff = Nsigev/nsig;
		
		printf("S=%5d  B=%6d (%.2f): ",(int)Nsigev, (int)Nbgev, supr);
		
		// stop if target reached or signal efficiency too small
		if (beff<=target || seff<mineff || rseff<minreleff) continue;
		
		double qa=0.;
		
		TString bestcut =findcut(t, els, elb, supr, qa);
		
		/*
		while (qa<0.1) 
		{	
			supr   *= 0.95; 
			cout <<" try again with qa="<<supr<<endl;
			bestcut = findcut(t, els, elb, supr, qa);
		}*/
		
		TString thenum = bestcut(rnum);
		TString thevar = bestcut(0,bestcut.Index(thenum));
		TRegexp rcut(thevar+"[0-9]+\\.[0-9]+");
		TString theoldcut = precut(rcut);
		
		//cout <<bestcut<<" "<<thenum<<" "<<thevar<<" "<<theoldcut<<endl;
		
		if (theoldcut!="") 
			precut.ReplaceAll(theoldcut,bestcut);
		else
			precut += "&&"+bestcut;
			
		cout <<"  -> "<<precut<<endl;
		
		if (oldcut == precut) {cout <<"STOP"<<endl; stop = true; continue;}
		oldcut = precut;
	}
	
	cout <<"\n"<<en<<smode<<" : "<<precut.Data()<<endl;
	cout <<"SIG EVT: "<<Nsigev<<" ev    BG: "<<Nbgev<<" ev"<<endl;

	printf("SIG EFF : %6.1f%%    BG EFF : %7.3f%%\n",Nsigev/N0_sig*100.,Nbgev/N0_bg*100.);
	printf("SIG REL : %6.1f%%    BG REL : %7.3f%%\n\n",Nsigev/nsig*100.,Nbgev/nbg*100.);

	cout<<"Recoil : "; for (int k=0;k<10;++k) printf("    %02d ",k);cout <<endl;
	cout<<"Eff    : "; for (int k=0;k<10;++k) printf("%6.1f%%",(double) evcntrec[k].size()/evmult*norm*100.);cout <<endl<<endl;

	t->SetEventList(0);
}
