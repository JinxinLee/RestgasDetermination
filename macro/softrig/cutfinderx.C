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
#include "TRegexp.h"
#include <map>
#include <utility>
#include <algorithm>

#define MAX 1000
#define BINS 500
#define NCAN 3

std::map<int, int> evcnt;
std::map<TString, TString> mctvar;

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

double bestSignificance(TH1* h1, TH1* h2, double &bestcut, double fac2=1.)
{
	int n=h1->GetNbinsX();
	double sum1=0, sum2=0;
	double bestr=0, bestl=0;
	double int1 = h1->Integral();
	double int2 = h2->Integral();
	double bestcutl=-999.;
	double bestcutr=999.;
	
	bestcut=-999.;
	
	for (int i=1;i<n;++i)
	{
		double cur1 = h1->GetBinContent(i)/int1;
		double cur2 = h2->GetBinContent(i)/int2;
		sum1 += cur1;
		sum2 += cur2;
		
		double vall = 0, valr=0;
		
		if ((sum1+fac2*sum2)>0) vall=sum1/sqrt(fac2*sum2+sum1);
		if ((1.-sum1+fac2*(1.-sum2))>0) valr=(1.-sum1)/sqrt(1.-sum1+fac2*(1.-sum2));
		
		if (valr>bestr) { bestr=valr; bestcutr = h1->GetBinCenter(i);}
		if (vall>bestl) { bestl=vall; bestcutl = h1->GetBinCenter(i);}
	}
	
	if (bestr>bestl) 
	{
		bestcut = bestcutr;
		return bestr;
	}
	
	bestcut = bestcutl;
	return bestl;
}

int countEvents(TTree *t, TString ccut)
{
	t->SetEventList(0);
	t->SetBranchStatus("*",1);
	t->Draw(">>el",ccut);
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("evt",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus("mode",1);
	
	TEventList *el=(TEventList*)gDirectory->Get("el");
	
	Int_t ev,run,mode;
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	
	evcnt.clear();
	
	for (int i=0;i<el->GetN();++i)
	{
		t->GetEntry(el->GetEntry(i));
		evcnt[ev+run*10000+(int(mode)%10)*1000000]+=1;
	}
	t->SetBranchStatus("*",1);

	return evcnt.size();
}
int countEvents(TTree *t, TEventList &el)
{
	t->SetEventList(&el);
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus("mode",1);
	
	Int_t ev, run, mode;
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
	
	evcnt.clear();
	
	for (int i=0;i<el.GetN();++i)
	{
		t->GetEntry(el.GetEntry(i));
		evcnt[ev+10000*run+(((mode/100)%10)*20+mode%10)*100000]+=1;
	}
	t->SetBranchStatus("*",1);

	return evcnt.size();
}

double diffQA(TH1F &ht1, TH1F &ht2)
{
	int n=ht1.GetNbinsX();
	TH1F h1=ht1;
	TH1F h2=ht2;
	h1.Scale(1.0/h1.Integral());
	h2.Scale(1.0/h2.Integral());
	h1.Add(&h2,-1);
	
	double sum=0.;
	
	for (int i=1;i<=n;++i)
	{
		sum+=fabs(h1.GetBinContent(i));
	}
	
	return sum;
}

double bestEffEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double supr, int idx)
{	
	int i;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus(varname,1);
	
	Float_t var;
	Int_t ev, run, mode;
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
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
		sigvals.push_back(std::make_pair(var, ev+10000*run+(((mode/100)%10)*20+mode%10)*100000 ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		bgvals.push_back(std::make_pair(var, ev+10000*run+(((mode/100)%10)*20+mode%10)*100000 ));
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
	minh[idx]=min; maxh[idx]=max;

	t->SetBranchStatus("*",1);
		
	double lefteff  = sigcnt.size()/Nsigev;
	double righteff = sigcnt2.size()/Nsigev;

	//cout <<varname<<"(B="<<bgcnt.size()<<") l:"<<leftcut<<"("<<sigcnt.size()<<"/"<<lefteff<<")  r:"<<rightcut<<"("<<sigcnt2.size()<<"/"<<righteff<<")"<<endl;
	cout <<varname<<" "<<flush;
	
	bestcut = rightcut;
	if (lefteff>righteff) 
	{
		bestcut = leftcut;
		return lefteff;
	}
	return righteff;
}


double bestSuppressionEvt(TTree *t, TString varname, TEventList &els, TEventList &elb, double &bestcut, double eff, int idx)
{	
	int i;
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("ev",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("run",1);
	t->SetBranchStatus(varname,1);
	
	Float_t var;
	Int_t ev, run, mode;
	t->SetBranchAddress("ev",&ev);
	t->SetBranchAddress("run",&run);
	t->SetBranchAddress("mode",&mode);
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
		sigvals.push_back(std::make_pair(var, ev+10000*run+(((mode/100)%10)*20+mode%10)*100000 ));
		if (var>max) max = var;
		if (var<min) min = var;
	}
		
	// prepare the pairs of variable value, eventnumber for background and count background
	for (i=0;i<elb.GetN();++i)
	{
		t->GetEntry(elb.GetEntry(i));
		bgvals.push_back(std::make_pair(var, ev+10000*run+(((mode/100)%10)*20+mode%10)*100000 ));
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
		if (bgvals[i].first>max) max = bgvals[i].first;
		if (bgvals[i].first<min) min = bgvals[i].first;
	}
	
	if (max>25) max=25;
	minh[idx]=min; maxh[idx]=max;

	t->SetBranchStatus("*",1);
	
	
	double leftsupr = (Nbgev-bgcnt.size())/Nbgev;
	double rightsupr = (Nbgev-bgcnt2.size())/Nbgev;

	//cout <<varname<<" l:"<<leftcut<<"("<<leftsupr<<")  r:"<<rightcut<<"("<<rightsupr<<")"<<endl;
	//cout <<varname<<"(S="<<sigcnt.size()<<") l:"<<leftcut<<"("<<bgcnt.size()<<"/"<<leftsupr<<")  r:"<<rightcut<<"("<<bgcnt2.size()<<"/"<<rightsupr<<")"<<endl;
	cout <<varname<<" "<<flush;
	
	if (leftcut!=leftcut || rightcut!=rightcut) return 0;
	
	bestcut = rightcut;
	if (leftsupr>rightsupr) 
	{
		bestcut = leftcut;
		return leftsupr;
	}
	return rightsupr;
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

void cutfinderx(TString fname, TString precut="", double supr=0.9, int evmult=10000, double norm=1.0)
{
	gStyle->SetTitleX(0.2);
	gStyle->SetTitleY(0.993);
	gStyle->SetTitleH(0.07);
	gStyle->SetPadTopMargin(0.08);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetOptStat(0);
	
	TString tagcut = "tag";
	
	TRegexp regntp("n[0-9]+");
	TRegexp regS("[0-9]+S");
	TRegexp regB("[0-9]+B");
	
	TString s=fname(regS);
	s = s(0,s.Length()-1);
	TString b=fname(regB);
	b = b(0,b.Length()-1);
	
	TString ntp = fname(regntp);
	int n0s = s.Atoi();
	int n0b = b.Atoi();
	
	N0_sig = n0s*evmult;
	N0_bg = n0b*evmult;
	
	cout <<"tree '"<<ntp.Data()<<"' with N_sig = "<<N0_sig<<", N_bg = "<<N0_bg<<endl;

	int i,j;
	double sigl, sigh, bgl, bgh;
	
	TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	TEventList els("els");
	TEventList elsall("elsall");
	TEventList elb("elb");
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1800,550);
	c1->Divide(7,3);
	
	TObjArray* branches = t->GetListOfBranches();
	
	//TString bgcut = "!("+sigcut+")";
	
	if (precut=="") precut = tagcut;
	else precut = tagcut+"&&"+precut;
	
	TString sigcut = "xmct";
	TString bgcut  = "!xmct";
	
	TString bstring = precut+" "+sigcut;
	if (precut!="") 
	{
		sigcut+="&&"+precut;
		bgcut +="&&"+precut;
	}
	
	cout <<sigcut <<" "<<bgcut<<endl;
	t->Draw(">>elsall",sigcut);

	t->Draw(">>els",sigcut);
	t->Draw(">>elb",bgcut);
	
	float nsig = countEvents(t,elsall);
	Nsigev = countEvents(t, els);
	Nbgev = countEvents(t, elb);
	
	cout <<"SIG: "<<nsig<<" ev (mct: "<<Nsigev<<")  "<<els.GetN()<<" cn   BG: "<<Nbgev<<" ev  "<<elb.GetN()<<" cn"<<endl;

	int cnt=1;
	
	for (i=0;i<MAX;++i) { idx[i]=i; signi[i]=0.;}
	
	for(i=0; i<=branches->GetLast(); ++i)
	{
	
		TBranch* branch = (TBranch*)branches->UncheckedAt(i);
		vars[i]=branch->GetName(); 
		TString v=vars[i];
		//t->SetBranchStatus(vars[i],1);
		if ( v=="ev" || v=="mode" || v=="run" || v=="nsig" ) continue;
		if ( v.Contains("pdg") || v.Contains("beam") || v.Contains("mct") ) continue;
		if (v.BeginsWith("t") && v!="thr") continue;

		bool ok=false;
		
		if ( v.EndsWith("p") || v.EndsWith("tht") || v.EndsWith("pcm") || v.EndsWith("thtcm") || v.EndsWith("pt") ) ok=true;
		if ( ok || v.EndsWith("ang") || v.Contains("poc") ) ok=true;
		if ( ok || v.Contains("pid") || v.Contains("min") || v.Contains("max") || v.Contains("sum") || v.Contains("fw") ) ok=true;
		if ( ok || v.EndsWith("sph") || v.EndsWith("apl") || v.EndsWith("pla") || v.EndsWith("thr") || v.EndsWith("cir") ) ok=true;
		
		if (!ok) continue;
	
		if (supr<0)
			signi[i] = bestEffEvt(t, vars[i], els, elb, cut[i], -supr, i);
		else
			signi[i] = bestSuppressionEvt(t, vars[i], els, elb, cut[i], supr, i);	
	}
	cout <<endl;
	 
	//t->SetBranchStatus("*",1);
	
	cout <<"\n\nBEST 20 vars:"<<endl<<endl;
 	
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
	
	for (j=0;j<21;++j)
	{		
		i=myidx[j];
		cout << vars[i]<<" : qa = "<<signi[i]<<"  cut = "<<cut[i]<<endl;
		if (j<20)
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
		else
		{
			c1->cd(21);
			t->SetEventList(&els);
			t->Draw("recmode>>hrec");
			TH1F *hrec = (TH1F*)gDirectory->FindObject("hrec");
			hrec->Scale(norm/evmult);
			hrec->Draw();
		}
		
	}
	
/*	c1->cd(21);
	t->SetEventList(&els);
	TH1F h1("h1","mode",200,0,100);
	t->Project("h1","mode%1000");*/
	
	
	c1->Update();
	
	//float nsig = countEvents(t,elsall);
	//float nbg  = countEvents(t,elb);
	cout <<"CUT     : "<<precut.Data()<<endl;
	cout <<"SIG: "<<nsig<<" ev (mct: "<<Nsigev<<")  "<<els.GetN()<<" cn   BG: "<<Nbgev<<" ev  "<<elb.GetN()<<" cn"<<endl;
	cout <<"SIG EVT : "<<nsig<<"    "<<"BG EVT : "<<Nbgev<<endl;
	cout <<"SIG EFF : "<<nsig/N0_sig<<"    "<<"BG EFF : "<< Nbgev/N0_bg<<endl;
	
	t->SetEventList(0);
}
