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

#define MAX 1000
#define BINS 500
#define NCAN 3

std::map<int, int> evcnt;

double signi[MAX];
int idx[MAX];
TString vars[MAX];
double cut[MAX];

double N0_sig, N0_bg;

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

double bestEff(TH1* h1, TH1* h2, double &bestcut, double supr2=0.8, double fac2=1.)
{
	int n=h1->GetNbinsX();
	double sum1=0, sum2=0;
	double bestr=0, bestl=0;
	double int1 = h1->Integral();
	double int2 = h2->Integral();
	double bestcutl=-999.;
	double bestcutr=999.;
	
	bestcut=-999.;
	int i=1;
	
	while (sum2<supr2)
	{
		sum1 += h1->GetBinContent(i)/int1;
		sum2 += h2->GetBinContent(i++)/int2;	
	}
	bestr=(1.0-sum1);
	bestcutr = h1->GetBinCenter(i-1);
	
	sum1=0;
	sum2=0;
	i=n;
	
	while (sum2<supr2)
	{
		sum1 += h1->GetBinContent(i)/int1;
		sum2 += h2->GetBinContent(i--)/int2;	
	}
	
	bestl=(1.0-sum1);
	bestcutl = h1->GetBinCenter(i-1);
		
	if (bestr>bestl) 
	{
		bestcut = bestcutr;
		return bestr;
	}
	
	bestcut = bestcutl;
	return bestl;
}
double bestSuppression(TH1* h1, TH1* h2, double &bestcut, double eff=0.9)
{
	int n=h1->GetNbinsX();
	double sum1=0, sum2=0;
	double bestr=0, bestl=0;
	double int1 = h1->Integral();
	double int2 = h2->Integral();
	double bestcutl=-999.;
	double bestcutr=999.;
	
	bestcut=-999.;
	int i=1;
	
	while (sum1<eff)
	{
		sum1 += h1->GetBinContent(i)/int1;
		sum2 += h2->GetBinContent(i++)/int2;	
	}
	bestr=(1.0-sum2);
	bestcutr = h1->GetBinCenter(i-1);
	
	sum1=0;
	sum2=0;
	i=n;
	
	while (sum1<eff)
	{
		sum1 += h1->GetBinContent(i)/int1;
		sum2 += h2->GetBinContent(i--)/int2;	
	}
	
	bestl=(1.0-sum2);
	bestcutl = h1->GetBinCenter(i-1);
		
	if (bestr>bestl) 
	{
		bestcut = bestcutr;
		return bestr;
	}
	
	bestcut = bestcutl;
	return bestl;
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

void cutfinder(TString pre, TString ntp, TString sigcut, TString precut="", double n0s=50., double n0b=200., double supr=0.9 )
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
	
	TFile *f=new TFile("M"+pre+"_"+ntp+".root","READ");
	//TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1800,600);
	c1->Divide(6,2);
	
	TObjArray* branches = t->GetListOfBranches();
	
	TString bgcut = "!("+sigcut+")";
	TString bstring = precut+" "+sigcut;

/*	if (precut!="") 
	{
		sigcut+="&&"+precut;
		bgcut +="&&"+precut;
	}*/

	t->SetEventList(0);
	TEventList el1("el1");
	t->Draw(">>el1",precut);
	t->SetEventList(&el1);
	
	
	int cnt=1;
	
	for (i=0;i<MAX;++i) { idx[i]=i; signi[i]=0.;}
	
/*	bstring.ReplaceAll("("," ");bstring.ReplaceAll(")"," ");
	bstring.ReplaceAll("&"," ");bstring.ReplaceAll("|"," ");bstring.ReplaceAll("!"," ");
	bstring.ReplaceAll(">"," ");bstring.ReplaceAll("<"," ");bstring.ReplaceAll("="," ");
	
	bstring+=" ev";
	
	TObjArray* ar=bstring.Tokenize(" ");

	t->SetBranchStatus("*",0);
	
	for (Int_t i = 0; i <ar->GetLast()+1; i++) 
	{
		TString subStr = ((TObjString *)ar->At(i))->GetString();
		if (!subStr.IsFloat()) 
		{
			cout << "\"" << subStr << "\" ";
			t->SetBranchStatus(subStr,1);
		}
	}	
	cout <<endl;*/
	for(i=0; i<=branches->GetLast(); ++i)
	{
	
		TBranch* branch = (TBranch*)branches->UncheckedAt(i);
		vars[i]=branch->GetName(); 
		//t->SetBranchStatus(vars[i],1);
		
		if (vars[i]=="evt") continue;
		if (vars[i]=="mode") continue;
		if (vars[i].Contains("dec")) continue;
		if (vars[i].BeginsWith("t") && vars[i]!="thr") continue;
		
		findLimits(t,vars[i],sigcut, sigl, sigh);
		findLimits(t,vars[i],bgcut, bgl, bgh);
		
		if (sigl>bgl) sigl=bgl;
		if (sigh<bgh) sigh=bgh;
		
/*		cout <<sigl<<" - "<<sigh<<endl;*/
		if (sigl==sigh) continue;
		TH1F h1("h1","",200,sigl,sigh);
		TH1F h2("h2","",200,sigl,sigh);
		h2.SetLineColor(2);
		
		t->Project("h1",vars[i],sigcut);
		t->Project("h2",vars[i], bgcut);
		
		if (h1.GetEntries()<3 || h2.GetEntries()<3) continue;
		
		signi[i] = bestSuppression(&h1,&h2, cut[i], supr);
		//signi[i] = bestEff(&h1,&h2, cut[i], supr);

		cout <<vars[i]<<"  "<<flush;//<<" : "<< vars[i]<<" : signi="<<signi[i]<<endl;
		
/*		c1->cd(cnt%20+1);
		h2.DrawNormalized();
		h1.DrawNormalized("same");
		c1->Update();
		cnt++;*/
		//t->SetBranchStatus(vars[i],0);
		
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
	
	for (j=0;j<12;++j)
	{		
		i=myidx[j];
		cout << vars[i]<<" : qa = "<<signi[i]<<"  cut = "<<cut[i]<<endl;
		if (j<12)
		{
			findLimits(t,vars[i],sigcut, sigl, sigh);
			findLimits(t,vars[i],bgcut, bgl, bgh);
					
			if (sigl>bgl) sigl=bgl;
			if (sigh<bgh) sigh=bgh;
			
			TH1F h1("h1",vars[i],200,sigl,sigh);
			TH1F h2("h2",vars[i],200,sigl,sigh);
			h2.SetLineColor(2);
		
			t->Project("h1",vars[i], sigcut);
			t->Project("h2",vars[i], bgcut);
			
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
			
			lt.DrawLatex(axmin+(axmax-axmin)*0.6,1.01*maxi,TString::Format("sig = %6.4f",signi[i]));

			c1->Update();
		}
		
	}
	
	if (precut!="") 
	{
		sigcut+="&&"+precut;
		bgcut +="&&"+precut;
	}
	
	t->SetEventList(0);
	
	float nsig = countEvents(t,sigcut);
	float nbg  = countEvents(t,bgcut);
	
	cout <<"SIG EVT : "<<nsig<<"    "<<"BG EVT : "<<nbg<<endl;
	cout <<"SIG EFF : "<<nsig/N0_sig<<"    "<<"BG EFF : "<< nbg/N0_bg<<endl;
}