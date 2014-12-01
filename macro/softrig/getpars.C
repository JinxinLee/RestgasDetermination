#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TRegexp.h"
#include "TCanvas.h"

#include <iostream>

void getpars(TString fname)
{
	TRegexp regntp("n[0-9]+");
	TString ntp = fname(regntp);
	TString smode = ntp(1,ntp.Length());
	int mode = smode.Atoi()/10;
	
	bool dst = (mode == 11 || mode == 13 || mode == 15); 
		
	TFile *f=new TFile(fname,"READ");
	TTree *t=(TTree*)f->Get(ntp);

	TCanvas *c1=new TCanvas("c1","c1",800,600);
	t->Draw("xm>>h","xmct");
	
	TH1F *h=(TH1F*)gDirectory->Get("h");
	
	TF1 f1("f1","gaus(0)");
	f1.SetParameters(h->GetMaximum(), h->GetMean(), h->GetRMS());
	
	h->Fit("f1","q");
	
	printf("MEAN = %6.4f  SIGMA = %6.4f\n", f1.GetParameter(1), f1.GetParameter(2));
	
	if (dst)
	{
		t->Draw("xmdif>>hd","xmct");
	
		TH1F *hd=(TH1F*)gDirectory->Get("hd");
	
		TF1 f2("f2","gaus(0)");
		f2.SetParameters(hd->GetMaximum(), hd->GetMean(), hd->GetRMS());
	
		hd->Fit("f2","q");
		
		printf("dM: MEAN = %6.4f  SIGMA = %6.4f\n", f2.GetParameter(1), f2.GetParameter(2));
	}
}
