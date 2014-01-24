#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TEventList.h"
#include "TDirectory.h"
#include <iostream>
#include <map>


#define MAX 200
#define BINS 500
#define NCAN 3

std::map<int, int> evcnt;

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


void cutqa(TString pre, TString ntp, TString var, TString cut="", int n0sig=0, int n0bkg=0)
{
	TString bgcode="900";
	TFile *f=new TFile("M"+pre+"_"+ntp+".root","READ");
	TTree *t=(TTree*)f->Get(ntp);
	
	int bkgmode = pre.Atoi()*100+50;
	
	Float_t ev, mode;
	t->SetBranchAddress("evt",&ev);
	t->SetBranchAddress("mode",&mode);
	
	if (cut=="") cut="1";
	
	bool twod = var.Contains(":");
	
	int i,j,k;

	int nsig, nbkg;
	
	TString sigcut="tag&&mode!="+bgcode+"&&mct";
	t->Draw(var+">>h(150)",sigcut);
	TH1F *h=(TH1F*)gDirectory->Get("h");
	nsig=h->GetEntries();
	sigcut+="&&"+cut;
	
	TString bkgcut="tag&&mode=="+bgcode;
	t->Draw(var+">>h(150)",bkgcut);
	h=(TH1F*)gDirectory->Get("h");
	nbkg=h->GetEntries();
	bkgcut+="&&"+cut;
	
	t->Draw(var+">>hs(150)",sigcut);
	TH1F *hs=(TH1F*)gDirectory->Get("hs");
	hs->SetLineColor(2);
	t->Draw(var+">>hb(150)",bkgcut,"same");
	TH1F *hb= (TH1F*)gDirectory->Get("hb");
	
/*	hs->Scale(1./hs->GetEntries());
	hb->Scale(1./hb->GetEntries());*/
	
	if (twod)
	{
		hs->SetMarkerColor(2);		
		hb->Draw();hs->Draw("same");
	}
	else
	if (hs->GetMaximum()>hb->GetMaximum())
	{
		hs->Draw();hb->Draw("same");
	}
	else
	{
		hb->Draw();hs->Draw("same");
	}

	int nev_sig=0, nev_bkg=0, ncnd_sig=0, ncnd_bkg=0;
	
/*	TEventList el("el","");
	
	// bkg count
	t->Draw(">>el","tag&&mode!="+pre+"50&&"+cut);	
	t->SetEventList(&el);*/
// 	t->SetBranchStatus("*",0);
// 	t->SetBranchStatus("ev",1);
// 	t->SetBranchStatus("tag",1);
// 	t->SetBranchStatus("mode",1);

/*	Float_t currev=-1;
	
	for (j=0;j<el.GetN();++j)
	{
		t->GetEntry(el.GetEntry(j));
		if (currev!=ev)
		{
			nev_sig++;
			currev=ev;
		}
	}*/
	cout <<"CAND: N_sig = "<<hs->GetEntries();
/*	t->SetEventList(0);
	t->Draw(">>el","tag&&mode=="+pre+"50&&"+cut);
	t->SetEventList(&el);
	currev=-1;
	
	for (j=0;j<el.GetN();++j)
	{
		t->GetEntry(el.GetEntry(j));
		if (currev!=ev)
		{
			nev_bkg++;
			currev=ev;
		}
	}*/
	cout <<"   N_bkg = "<<hb->GetEntries()<<endl;;
	cout <<"CAND: eff_sig = "<< hs->GetEntries()/nsig<<"   eff_bkg = "<<hb->GetEntries()/nbkg<<endl;
	
/*	t->SetBranchStatus("*",1);*/
	t->SetEventList(0);
	
	nev_sig = countEvents(t, "tag&&mode!="+bgcode+"&&"+cut);
	nev_bkg = countEvents(t, "tag&&mode=="+bgcode+"&&"+cut);
	
	cout <<"EVTS: N_sig = "<< nev_sig<<"   N_bkg = "<<nev_bkg<<endl;
	if (n0sig>0 && n0bkg>0)
		cout <<"EVTS: eff_sig = "<< nev_sig*0.001/n0sig<<"   eff_bkg = "<<nev_bkg*0.001/n0bkg<<endl;
	
}