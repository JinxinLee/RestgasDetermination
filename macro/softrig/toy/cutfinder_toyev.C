#include <algorithm>
#include "TFile.h"
#include "TTree.h"
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
#include <map>
#include <utility>
#include <algorithm>

#define MAX 1000
#define BINS 500
#define NCAN 3

std::map<int, int> evcnt;

double signi[MAX];
int idx[MAX];
TString vars[MAX];
double cut[MAX];

double minh[MAX], maxh[MAX];

double N0_sig, N0_bg;
double Nsigev, Nbgev;

bool mycompare(int i, int j)
{
  return signi[i]>signi[j];
}

int countEvents(TTree *t, TString ccut)
{
	t->SetEventList(0);
	t->SetBranchStatus("*",1);
	t->Draw(">>el",ccut);
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("evt",1);
	
	TEventList *el=(TEventList*)gDirectory->Get("el");
	
	Float_t ev;
	t->SetBranchAddress("evt",&ev);
	
	evcnt.clear();
	
	for (int i=0;i<el->GetN();++i)
	{
		t->GetEntry(el->GetEntry(i));
		evcnt[ev]+=1;
	}
	t->SetBranchStatus("*",1);

	return evcnt.size();
}
int countEvents(TTree *t, TEventList &el)
{
	t->SetEventList(&el);
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("evt",1);
	
	Float_t ev;
	t->SetBranchAddress("evt",&ev);
	
	evcnt.clear();
	
	for (int i=0;i<el.GetN();++i)
	{
		t->GetEntry(el.GetEntry(i));
		evcnt[ev]+=1;
	}
	t->SetBranchStatus("*",1);

	return evcnt.size();
}

double bestSepEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double eff, int idx)
{	
	int i,j;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("evt",1);
	t->SetBranchStatus(varname,1);
	
	Float_t ev, var;
	t->SetBranchAddress("evt",&ev);
	t->SetBranchAddress(varname,&var);
	
	std::map<int, int> sigcnt;
	std::map<int, int> bgcnt;
	std::map<int, int> bgcnt2;
	
	std::vector<pair<double, int> > sigvals;
	std::vector<pair<double, int> > bgvals;
	
	int Nsigval = els.GetN(), Nbgval = elb.GetN();
	double min = 1e9, max=-1e9;
	
	// prepare the pairs of variable value, eventnumber for signal and count signals
	for (i=0;i<els.GetN();++i)
	{
		t->GetEntry(els.GetEntry(i));
		sigvals.push_back(std::make_pair(var, ev));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		bgvals.push_back(std::make_pair(var, ev));
		if (var>max) max = var;
		if (var<min) min = var;
	}
	
	minh[idx]=min; maxh[idx]=max;
	
	// sort signals by first element = variable value
	sort(sigvals.begin(), sigvals.end());
	sort(bgvals.begin(), bgvals.end());

	double pos=min, step = (max-min)/500., bestqal=0, bestcutl=0, bestqar=0, bestcutr=0;
	int bestsig=0, bestbg=0;
	
	i=0, j=0;
	sigcnt.clear();
	bgcnt.clear();
	
	for (pos=min+step; pos<max; pos+=step)
	{
		while ( sigvals[i].first<=pos && i<Nsigval) sigcnt[sigvals[i++].second]+=1;
		while ( bgvals[j].first<=pos && j<Nbgval) bgcnt[bgvals[j++].second]+=1;
		
		double qa = sigcnt.size()/Nsigev * (Nbgev-bgcnt.size())/Nbgev;
		if (qa>bestqal)
		{
			bestqal = qa;
			bestcutl = pos;
			bestsig = sigcnt.size();
			bestbg = bgcnt.size();
		}
	}
	
	cout <<varname<<"  qa(l)="<<bestqal<<" S="<<bestsig <<" B="<<bestbg;
	
	i=sigvals.size()-1, j=bgvals.size()-1;
	sigcnt.clear();
	bgcnt.clear();

	for (pos=max-step; pos>min; pos-=step)
	{
		while ( sigvals[i].first>=pos && i>=0) sigcnt[sigvals[i--].second]+=1;
		while ( bgvals[j].first>=pos && j>=0) bgcnt[bgvals[j--].second]+=1;
		
		double qa = sigcnt.size()/Nsigev * (Nbgev-bgcnt.size())/Nbgev;
		if (qa>bestqar)
		{
			bestqar = qa;
			bestcutr = pos;
			bestsig = sigcnt.size();
			bestbg = bgcnt.size();
		}
	}
	cout <<"  qa(r)="<<bestqar<<" S="<<bestsig <<" B="<<bestbg<<endl;
	

	t->SetBranchStatus("*",1);
	
	bestcut=bestcutr;
	if (bestqal>bestqar) 
	{
		bestcut = bestcut;
		return bestqal;
	}
	
	return bestqar;
}


double bestSuppressionEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double eff, int idx)
{	
	int i;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("evt",1);
	t->SetBranchStatus(varname,1);
	
	Float_t ev, var;
	t->SetBranchAddress("evt",&ev);
	t->SetBranchAddress(varname,&var);
	
	std::map<int, int> sigcnt;
	std::map<int, int> bgcnt;
	std::map<int, int> bgcnt2;
	
	std::vector<pair<double, int> > sigvals;
	std::vector<pair<double, int> > bgvals;
	
	int Nsigval = els.GetN(), Nbgval = elb.GetN();
	double min = 1e9, max=-1e9;
	
	// prepare the pairs of variable value, eventnumber for signal and count signals
	for (i=0;i<els.GetN();++i)
	{
		t->GetEntry(els.GetEntry(i));
		sigvals.push_back(std::make_pair(var, ev));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		bgvals.push_back(std::make_pair(var, ev));
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
	
	minh[idx]=min; maxh[idx]=max;

	t->SetBranchStatus("*",1);
	
	
	double leftsupr = (Nbgev-bgcnt.size())/Nbgev;
	double rightsupr = (Nbgev-bgcnt2.size())/Nbgev;

	cout <<varname<<"(S="<<sigcnt.size()<<") l:"<<leftcut<<"("<<bgcnt.size()<<"/"<<leftsupr<<")  r:"<<rightcut<<"("<<bgcnt2.size()<<"/"<<rightsupr<<")"<<endl;
	
	bestcut = rightcut;
	if (leftsupr>rightsupr) 
	{
		bestcut = leftcut;
		return leftsupr;
	}
	return rightsupr;
}

double bestEffEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double supr, int idx)
{	
	int i;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("evt",1);
	t->SetBranchStatus(varname,1);
	
	Float_t ev, var;
	t->SetBranchAddress("evt",&ev);
	t->SetBranchAddress(varname,&var);
	
	std::map<int, int> sigcnt;
	std::map<int, int> bgcnt;
	std::map<int, int> sigcnt2;
	
	std::vector<pair<double, int> > sigvals;
	std::vector<pair<double, int> > bgvals;
	
	int Nsigval = els.GetN(), Nbgval = elb.GetN();
	double min = 1e9, max=-1e9;
	
	// prepare the pairs of variable value, eventnumber for signal and count signals
	for (i=0;i<els.GetN();++i)
	{
		t->GetEntry(els.GetEntry(i));
		sigvals.push_back(std::make_pair(var, ev));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		bgvals.push_back(std::make_pair(var, ev));
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
	
	minh[idx]=min; maxh[idx]=max;

	t->SetBranchStatus("*",1);
		
	double lefteff  = sigcnt.size()/Nsigev;
	double righteff = sigcnt2.size()/Nsigev;

	cout <<varname<<"(B="<<bgcnt.size()<<") l:"<<leftcut<<"("<<sigcnt.size()<<"/"<<lefteff<<")  r:"<<rightcut<<"("<<sigcnt2.size()<<"/"<<righteff<<")"<<endl;
	
	bestcut = rightcut;
	if (lefteff>righteff) 
	{
		bestcut = leftcut;
		return lefteff;
	}
	return righteff;
}

void findLimits(TTree *t, TString var, TString ccut, double &low, double &high, double frac = 0.98)
{
	low  = t->GetMinimum(var);
	high = t->GetMaximum(var);
	return;
	
	double miss = (1.-frac)/2;
	
	t->SetEventList(0);
	TEventList el("el");
	t->Draw(">>el",ccut);
	t->SetEventList(&el);
	
	double llow  = t->GetMinimum(var);
	double lhigh = t->GetMaximum(var);

	TH1F htemp("htemp","",500,llow, lhigh);
	t->Project("htemp",var);
	
	double sum=0.;
	double integ = htemp.Integral();
	
	int i=1;
	
	while (sum<miss) sum+=htemp.GetBinContent(i++)/integ;
	low = htemp.GetBinCenter(i-2);
	sum=0.; i=500;
	
	while (sum<miss) sum+=htemp.GetBinContent(i--)/integ;
	high = htemp.GetBinCenter(i+2);
	
	t->SetEventList(0);	
}

void cutfinder_toyev(TString pre, TString ntp, TString sigcut, TString precut="", double n0s=50., double n0b=500., double supr=0.9 )
{
	gStyle->SetTitleY(0.993);
	gStyle->SetTitleH(0.07);
	gStyle->SetPadTopMargin(0.08);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetOptStat(0);
	
	N0_sig = n0s*1000.;
	N0_bg = n0b*1000.;


	int i,j;
	double sigl, sigh, bgl, bgh;
	
	TFile *f=new TFile("data/M"+pre+"_"+ntp+".root","READ");
	//TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	TEventList els("els");
	TEventList elsall("elsall");
	TEventList elb("elb");
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1500,900);
	c1->Divide(5,3);
	
	TObjArray* branches = t->GetListOfBranches();
	
	TString bgcut = "!("+sigcut+")";
	//sigcut+="&&mct";
	
	if (precut!="") 
	{
		sigcut+="&&"+precut;
		bgcut +="&&"+precut;
	}
	cout <<sigcut <<" "<<bgcut<<endl;
	t->Draw(">>elsall",sigcut);
	t->Draw(">>els",sigcut+"&&mct");
	t->Draw(">>elb",bgcut);
	
	Nsigev = countEvents(t, els);
	Nbgev = countEvents(t, elb);
	
	cout <<"SIG:"<<Nsigev<<" ev  "<<els.GetN()<<" cn   BG:"<<Nbgev<<" ev  "<<elb.GetN()<<" cn"<<endl;

	int cnt=1;
	
	for (i=0;i<MAX;++i) { idx[i]=i; signi[i]=0.;}
	
	for(i=0; i<=branches->GetLast(); ++i)
	{
	
		TBranch* branch = (TBranch*)branches->UncheckedAt(i);
		vars[i]=branch->GetName(); 
		//t->SetBranchStatus(vars[i],1);
		
		if (vars[i]=="evt") continue;
		if (vars[i]=="mode") continue;
		if (vars[i].Contains("pdg")) continue;
		if (vars[i].Contains("mct")) continue;
		if (vars[i].Contains("dec")) continue;
		if (vars[i].BeginsWith("t") && vars[i]!="thr") continue;
	
		if (supr<0)
			signi[i] = bestEffEvt(t, vars[i], els, elb, cut[i], -supr, i);
		else
			signi[i] = bestSuppressionEvt(t, vars[i], els, elb, cut[i], supr, i);
//		signi[i] = bestEffEvt(t, vars[i], els, elb, cut[i], supr, i);
		
	}
	 
	//t->SetBranchStatus("*",1);
	
	cout <<"\n\nBEST 12 vars:"<<endl<<endl;
 	
	std::vector<int> myidx (idx, idx+MAX);	
	
	std::sort(myidx.begin(), myidx.end(), mycompare);

	TLine l;
	l.SetLineColor(6);
	l.SetLineStyle(2);
	l.SetLineWidth(2);
	TLatex lt;
	lt.SetTextSize(0.06);
	TString target="supr";
	if (supr<0) target="eff";
	
	for (j=0;j<15;++j)
	{		
		i=myidx[j];
		cout << vars[i]<<" : qa = "<<signi[i]<<"  cut = "<<cut[i]<<endl;
		if (j<15)
		{
/*			findLimits(t,vars[i],sigcut, sigl, sigh);
			findLimits(t,vars[i],bgcut, bgl, bgh);
					
			if (sigl>bgl) sigl=bgl;
			if (sigh<bgh) sigh=bgh;
			*/
			TH1F h1("h1",vars[i],200,minh[i],maxh[i]);
			TH1F h2("h2",vars[i],200,minh[i],maxh[i]);
			h2.SetLineColor(2);
		
			t->SetEventList(&els);
			t->Project("h1",vars[i]);
			t->SetEventList(&elb);
			t->Project("h2",vars[i]);
			
			h1.Scale(1.0/h1.Integral());
			h2.Scale(1.0/h2.Integral());
			h1.SetTitleSize(0.05);
			
			c1->cd(j+1);
			double maxi = h1.GetMaximum();
			if (h2.GetMaximum()>maxi ) maxi = h2.GetMaximum();
			maxi*=1.1;
			h1.SetMaximum(maxi);
			h2.SetMaximum(maxi);
						
			h1.DrawNormalized();
			h2.DrawNormalized("same");

			double axmin = h1.GetXaxis()->GetXmin(),axmax = h1.GetXaxis()->GetXmax(); 
			
			l.DrawLine(cut[i],0, cut[i], maxi*0.9);
			
			lt.DrawLatex(axmin+(axmax-axmin)*0.6,1.01*maxi,TString::Format("%s = %6.4f",target.Data(),signi[i]));

			//c1->Update();
		}
		
	}
	
	c1->Update();
	
	float nsig = countEvents(t,elsall);
	float nbg  = countEvents(t,elb);
	
	cout <<"SIG EVT : "<<nsig<<"    "<<"BG EVT : "<<nbg<<endl;
	cout <<"SIG EFF : "<<nsig/N0_sig<<"    "<<"BG EFF : "<< nbg/N0_bg<<endl;
}
