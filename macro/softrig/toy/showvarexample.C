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
#include "TLegend.h"

#define MAX 1000
#define BINS 500
#define NCAN 3

std::map<int, int> evcnt;

int idx[MAX];
TString vars[MAX];

double N0_sig, N0_bg;

// *** setup  style
void setStyle()
{
	gStyle->SetPadTopMargin(0.12);
	gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadLeftMargin(0.17);
	gStyle->SetPadRightMargin(0.02);

	gStyle->SetLabelSize(0.075,"X");
	gStyle->SetLabelSize(0.065,"Y");

	gStyle->SetStatY(0.87);
	gStyle->SetStatX(0.98);
	gStyle->SetStatW(0.4);
	gStyle->SetStatH(0.12);
	gStyle->SetOptStat(0);//"e"); // only entries

	gStyle->SetTitleH(0.1);
	gStyle->SetTitleX(0.15);
	gStyle->SetTitleOffset(1.1,"x");
	gStyle->SetTitleXSize(0.076);
	gStyle->SetNdivisions(505);
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

double suppression(TH1* h1, TH1* h2, double cut)
{
	int n=h1->GetNbinsX();
	double sum1=0, sum2=0;
	double int1 = h1->Integral();
	double int2 = h2->Integral();
	
	int i=1;
	
	while (h1->GetBinCenter(i)<cut)
	{
		sum1 += h1->GetBinContent(i)/int1;
		sum2 += h2->GetBinContent(i++)/int2;	
	}
	return sum2;
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

void drawHistos(TTree *t, TString var, TString title, TString precut, TString newcut, double val, double val2=-999.)
{
	double sigl, sigh, bgl, bgh;

	TLine l;
	l.SetLineColor(1);
	l.SetLineStyle(2);
	l.SetLineWidth(2);
	
	TLatex lt;
	lt.SetTextSize(0.06);
	lt.SetTextColor(2);
	TLatex lt2;
	lt2.SetTextSize(0.06);
	lt2.SetTextColor(kBlue+2);
	
/*	double preeffb = countEvents(t, "mode==900&&tag");
	double preeffs = countEvents(t, "mode!=900&&tag");
	preeffb/=500000.;
	preeffs/=50000.;*/
	
	
	double eff=countEvents(t, "mode==900&&tag&&"+precut+"&&"+newcut);
	cout <<eff<<endl;
	eff/=500000.;
	double seff=countEvents(t, "mode!=900&&tag&&"+precut+"&&"+newcut);
	cout <<seff<<endl;
	seff/=50000.;
	
	TString sigcut = "mode!=900&&tag&&"+precut;
	TString bgcut = "mode==900&&tag&&"+precut;
	
	findLimits(t,var,sigcut, sigl, sigh);
	findLimits(t,var,bgcut, bgl, bgh);
			
	if (sigl>bgl) sigl=bgl;
	if (sigh<bgh) sigh=bgh;
	
	if (var=="dtht") sigh=0.5;
	
	TH1F *h1=new TH1F("h1",title,200,sigl,sigh);
	TH1F *h2=new TH1F("h2",title,200,sigl,sigh);
	h2->SetLineColor(2);
	h1->SetLineWidth(2);
	h2->SetLineWidth(2);

	t->Project("h1",var, sigcut);
	t->Project("h2",var, bgcut);
	
	double signi = suppression(h1,h2, val );
	
	h1->Scale(1.0/h1->Integral());
	h2->Scale(1.0/h2->Integral());
	h1->SetTitleSize(0.05);
	
	double maxi = h1->GetMaximum();
	if (h2->GetMaximum()>maxi ) maxi = h2->GetMaximum();
	maxi*=1.1;
	h1->SetMaximum(maxi);
	h2->SetMaximum(maxi);
				
	h1->DrawNormalized();
	h2->DrawNormalized("same");

	double axmin = h1->GetXaxis()->GetXmin(),axmax = h1->GetXaxis()->GetXmax(); 
	
	l.DrawLine(val,0, val, maxi*0.9);
	if (val2!=-999) l.DrawLine(val2,0, val2, maxi*0.9);
	
	//lt.DrawLatex(axmin+(axmax-axmin)*0.03,0.65*maxi,TString::Format("#epsilon_{BG,cut} = %5.2f%%",100*(1-signi)));
	lt.DrawLatex(axmin+(axmax-axmin)*0.53,0.53*maxi,TString::Format("#epsilon_{bg} = %5.2f%%",100*eff));
	lt2.DrawLatex(axmin+(axmax-axmin)*0.53,0.65*maxi,TString::Format("#epsilon_{sig} = %5.2f%%",100*seff));
	
	TLegend *leg1 = new TLegend(0.67,0.7,0.98,0.88);
	leg1->AddEntry("h1","signal","l");
	leg1->AddEntry("h2","BG","l");
	leg1->Draw();
	
}

void showvarexample()
{
	setStyle();
	
	int i,j;
	
	TFile *f=new TFile("data/M55_ndpm.root","READ");
	//TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get("ndpm");
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1600,400);
	c1->Divide(4,1);
	TObjArray* branches = t->GetListOfBranches();
	
	TString bgcut = "mode==900 && tag";

/*	TString name1= "p(#Lambda) cms [GeV/c]";
	TString var1 = "lampcm"; 
	TString cut1 = "lampcm>1.8";
	double val1 = 1.8;

	TString name2= "Thrust";
	TString var2 = "thr"; 
	TString cut2 = "thr>0.95"; 
	double val2 = 0.95;

	TString name3= "p_{t}(#Lambda) [GeV/c]";
	TString var3 = "lampt"; 
	TString cut3 = "lampt>1";
	double val3 = 1;*/


//dpcm>1.9&&dp>4&&ptmax>0.7&&dtht>0.09             *** sup 1000
	
	TString name1= "dpcm [GeV/c]";
	TString var1 = "dpcm"; 
	TString cut1 = "dpcm>1.9";
	double val1 = 1.9;
	double val11 =-999;

	TString name2= "dp [GeV/c]";
	TString var2 = "dp"; 
	TString cut2 = "dp>4"; 
	double val2 = 4;

	TString name3= "ptmax [GeV/c]";
	TString var3 = "ptmax"; 
	TString cut3 = "ptmax>0.7";
	double val3 = 0.7;

	TString name4= "dtht [rad]";
	TString var4 = "dtht"; 
	TString cut4 = "dtht>0.09";
	double val4 = 0.09;

	//abs(dpcm-2.05)<0.2&&dp>2&&dpt>0.5&&ptmax>0.5	 *** eff 90


// 	TString name1= "dpcm [GeV/c]";
// 	TString var1 = "dpcm"; 
// 	TString cut1 = "abs(dpcm-2.05)<0.2";
// 	double val1 = 1.85;
// 	double val11 =2.25;
// 
// 	TString name2= "dp [GeV/c]";
// 	TString var2 = "dp"; 
// 	TString cut2 = "dp>2"; 
// 	double val2 = 2;
// 
// 	TString name3= "ptmax [GeV/c]";
// 	TString var3 = "ptmax"; 
// 	TString cut3 = "ptmax>0.5";
// 	double val3 = 0.5;
// 
// 	TString name4= "dpt [GeV/c]";
// 	TString var4 = "dpt"; 
// 	TString cut4 = "dpt>0.5";
// 	double val4 = 0.5;

	double preEff = countEvents(t, bgcut);
	cout <<preEff<<endl;
	preEff/=500000.;
	

/*	t->SetEventList(0);
	TEventList el1("el1");
	t->Draw(">>el1",precut);
	t->SetEventList(&el1);*/
	
	c1->cd(1);
	drawHistos(t, var1,name1, "1",cut1,val1,val11);
	c1->cd(2);
	drawHistos(t, var2,name2, cut1,cut2,val2);
	c1->cd(3);
	drawHistos(t, var3,name3, cut1+"&&"+cut2,cut3,val3);
	c1->cd(4);
	drawHistos(t, var4,name4, cut1+"&&"+cut2+"&&"+cut3,cut4,val4);
	c1->Update();
	
	
	
	cout<<100*preEff<<endl;
			
}