#include "TChain.h"
#include "TCanvas.h"
#include "TH2F.h"

void inspectvar(int mode, TString var, TString opt="cont")
{
	TString path="/data.local1/kgoetzen/data/softrig/fsim/merge4/shr/"; 
	TChain n(TString::Format("n%3d",mode));
	n.Add(TString::Format("%sM*_n%3d_*root",path.Data(),mode));
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,800,600);
	
	double min = n.GetMinimum(var), max = n.GetMaximum(var), d=max-min;
	
	min-=0.05*d;
	max+=0.05*d;
	
// 	TH2F *h=new TH2F("h",var+" (mode "+TString::Format("%d",mode)+")",72,2.2,5.8,50,min,max);
// 	h->SetStats(0);
// 	h->SetXTitle("E_{cm} [GeV]");
// 	h->SetYTitle(var);
// 	
// 	n.Draw(var+":mode/1e8-0.05>>h","xmct");
// 	n.Draw(var+":mode/1e5+0.05>>+h","mode%1000==900");

	TH2F *h=new TH2F("h",var+" (mode "+TString::Format("%d",mode)+")",360,2.2,5.8,200,min,max);
	h->SetStats(0);
	h->SetXTitle("E_{cm} [GeV]");
	h->SetYTitle(var);
	
	n.Draw(var+":mode/1e8-0.13+recmode*0.01>>h","xmct");
	n.Draw(var+":mode/1e5+0.05>>+h","mode%1000==900");
	n.Draw(var+":mode/1e5+0.03>>+h","mode%1000==900");
	n.Draw(var+":mode/1e5+0.04>>+h","mode%1000==900");
	n.Draw(var+":mode/1e5+0.06>>+h","mode%1000==900");

	h->SetContour(50);
	h->Draw(opt);
	c1->Update();
}