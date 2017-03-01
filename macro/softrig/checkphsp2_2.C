#include "TH2F.h"
#include "TH1F.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TDirectory.h"
#include "TF1.h"
#include "TEfficiency.h"
#include "TColor.h"
#include "TRegexp.h"
#include "TMath.h"
#include "TROOT.h"

#include <iostream>
#include <vector>

double frsig, frmean, frchi, fremp;

// -----------------------------------------------------------------------

double Median(const TH2F *h,  const TH2F *h2) 
{
	int n1 = h->GetXaxis()->GetNbins(); 
	int n2 = h->GetYaxis()->GetNbins();
	
	int nfilled=0, N = h->GetEntries();
	
	double x[1000];
	double y[1000];
	
	for (int i=1;i<n1+1;++i)
	for (int j=1;j<n2+1;++j)
	{
		double cont = h->GetBinContent(i,j);
		double cont2 = h2->GetBinContent(i,j);
		
		if (cont>(0.001*N) && cont2>0.)
		{
			x[nfilled]   = cont2/cont;
			y[nfilled++] = 1.;
		}
	}
	return TMath::Median(nfilled, &x[0], &y[0]);
}

// -----------------------------------------------------------------------
void delete_histo(TString name)
{
	TH1 *h = (TH1*)gDirectory->Get(name);
	if (h) delete h;
}

// -----------------------------------------------------------------------

void palette2()
{  
/*   Double_t red[4]    = { 0.00, 176/256., 1.00, 1.00};
   Double_t grn[4]  = { 0.00, 53/256., 168/256., 1.00};
   Double_t blu[4]   = { 0.00, 0.00, 0.00, 0.50};
   Double_t stp[4] = { 0.00, 0.35 ,0.65, 1.00 };*/
//	int hex[8] = {0x482223,0x64394A,0x6D5877,0x5E7E9E,0x3EA5B0,0x48C9A9,0x8DE890,0xE7FE76};
// 	int hex[8] = {0x593432,0x6A4754,0x6A6175,0x5D7E8B,0x5A9A8D,0x7AB17F,0xB4C270,0xF8CB74};
// 	Double_t blu[8], red[8], grn[8], stp[8];
// 	
// 	for (int i=0;i<8;++i)
// 	{
// 		blu[7-i] = (hex[i] & 0xff)/255.;
// 		grn[7-i] = ((hex[i]>>8) & 0xff)/255.;
// 		red[7-i] = ((hex[i]>>16) & 0xff)/255.;
// 		stp[7-i] = (double)(7-i)/7.0;
// // 		cout << red[i]<<" "<<grn[i]<<" "<<blu[i]<<" "<<stp[i]<<endl;
// 	}
    Double_t blu[] = {1,1,0};
    Double_t red[] = {0,1,1};
    Double_t grn[] = {0,1,0};
    Double_t stp[] = {0,0.5,1.0};

	TColor::CreateGradientColorTable(3,stp,red,grn,blu,100) ;  
    gStyle->SetNumberContours(100);
/*	Double_t blu[3] = {1,0.5,0};
	Double_t red[3] = {1,0.5,0};
	Double_t grn[3] = {1,0.5,0};
	Double_t stp[3] = {0,0.5,1};
	
	Int_t n=100;
	TColor::CreateGradientColorTable(3,stp,red,grn,blu,n);*/
 }

// -----------------------------------------------------------------------

void cleanup()
{
	delete_histo("h1");
	delete_histo("h2");
	delete_histo("hp");
	delete_histo("heff");
	//delete_histo("hqa");
}

// -----------------------------------------------------------------------

void config_pad(TVirtualPad* p, double b=0.1, double r=0.12, double t=0.08,  double l=0.1)
{
	p->SetTopMargin(t);
	p->SetBottomMargin(b);
	p->SetLeftMargin(l);
	p->SetRightMargin(r);
/*	p->SetGridx();
	p->SetGridy();*/
}

// -----------------------------------------------------------------------

double qadalitz(TH2F* h1, TH2F* h2, TH1F *hpull)
{
	double globeff = h2->GetEntries()/h1->GetEntries();
	hpull->Reset();
	
	int nx=h1->GetNbinsX();
	int ny=h1->GetNbinsX();
	
	double sum=0;
	double coverage1=0, coverage2=0;
	
	for (int i=1;i<=nx;++i)
	for (int j=1;j<=ny;++j)
	{
		if (h1->GetBinContent(i,j)>0) coverage1+=1.;
		if (h2->GetBinContent(i,j)>0) coverage2+=1.;
		
		double c1 = h1->GetBinContent(i,j);
		double c2 = h2->GetBinContent(i,j);
		
		if (c1>0)
		{	
			double eff = c2/c1;
			double efferr = sqrt(2.)*eff/sqrt(c1);
			if (eff>0.0001) hpull->Fill((eff-globeff)/efferr);
		}
	}
	
	TF1 f1("f1","gaus");
	f1.SetParameters(hpull->GetMaximum(), hpull->GetMean(), hpull->GetRMS());
	hpull->Fit("f1","");
	
	double mean  = f1.GetParameter(1);
	double sigma = f1.GetParameter(2);
	double chindf = f1.GetChisquare()/f1.GetNDF(); 
	if (chindf<1.) chindf=1.;
	
	cout <<"m="<<mean<<" sigma="<<sigma<<" chi/ndf="<<chindf<<" emtpyfrac="<<fabs(1.0 - coverage2/coverage1)<<endl;
	return (fabs(sigma-1.) + fabs(mean) + fabs(1.0-chindf) + fabs(1.0 - coverage2/coverage1));
}
// -----------------------------------------------------------------------

double qadalitz2(TH2F* h1, TH2F* h2, TH1F *hpull, TH2F* heff)
{
	int nx=h1->GetNbinsX();
	int ny=h1->GetNbinsX();
	
	double N = h1->GetEntries();
	
	double coverage1=0, coverage2=0;
	for (int i=1;i<=nx;++i)
	for (int j=1;j<=ny;++j)
	{
		if (h1->GetBinContent(i,j)>0.0001*N) coverage1+=1.;
		if (h1->GetBinContent(i,j)>0.0001*N && h2->GetBinContent(i,j)>0) coverage2+=1.;
	}
	
	if (coverage2==0 || h1->GetEntries()<10 || h2->GetEntries()<10) return 10;
	
	double globeff = Median(h1,h2);//h2->GetEntries()/h1->GetEntries()*coverage1/coverage2;
	
	cout <<"c1="<<coverage1<<" c2="<<coverage2<<" geff="<<globeff<<endl;
	
	hpull->Reset();
	
	TEfficiency teff(*h2, *h1);
	teff.SetStatisticOption(TEfficiency::kFNormal);

	double perfbins=0.;
	
	for (int i=1;i<=nx;++i)
	for (int j=1;j<=ny;++j)
	{
		double c1 = h1->GetBinContent(i,j);
		double c2 = h2->GetBinContent(i,j);
		
		if (c1>0.0001*N)
		{	
			int gbin = teff.GetGlobalBin(i,j);
			
			double eff = teff.GetEfficiency(gbin);
			double efferr = teff.GetEfficiencyErrorUp(gbin);
			if (eff>0.0001 && efferr>0) 
			{
				hpull->Fill((eff-globeff)/efferr);
				heff->SetBinContent(i,j,(eff-globeff)/efferr);
			}
			if (eff>0.99999) perfbins+=1.;
		}
		else 
		{
			h1->SetBinContent(i,j,0);
			h2->SetBinContent(i,j,0);
		}
	}
	
	if (hpull->GetEntries()<5) return 0.001;
	
	TF1 f1("f1","gaus");
	f1.SetParameters(hpull->GetMaximum(), hpull->GetMean(), hpull->GetRMS());
	f1.SetParLimits(1,-8,8);
	f1.SetParLimits(2,0,20);
	
	hpull->Fit("f1","q");
	
	frmean   = f1.GetParameter(1);
	frsig    = f1.GetParameter(2);
	frchi    = f1.GetChisquare()/f1.GetNDF(); 
	fremp    = fabs(1.0 - coverage2/coverage1);
	double facperf  = fabs(1.0 - perfbins/coverage1);
	
	if (frchi<1.) frchi=1.;
	
	cout <<"m="<<frmean<<" sigma="<<frsig<<" chi/ndf="<<frchi<<" emtpyfrac="<<fremp<<"  not perf="<<facperf<<endl;
	return (fabs(frsig-1.) + fabs(frmean) + fabs(1.0-frchi) + fremp)*facperf;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

int checkphsp2_2(TString filepatt, double step=.1, TString cut="")
{
	palette2();
	
	TCanvas *c2=new TCanvas("c2","c2",1000,10,800,600);
	gStyle->SetOptStat(0);
	gStyle->SetTitleH(0.07);
	
	if (cut=="") cut="1";
	
	TRegexp reg("MI[0-9][0-9][0-9]");
	TRegexp regn("ntp[0-9]");
	
	TString mname=filepatt(reg);
	mname = mname(2,3);
	
	TString ntp=filepatt(regn);

	TChain *n=new TChain(ntp);
	if (filepatt.EndsWith(".root")) filepatt.ReplaceAll(".root","");
	n->Add(filepatt+"*.root");

	
	double minE = int(n->GetMinimum("beamm")*10+0.1)/10.;
	double maxE = 5.5;
	int bins = int((maxE-minE+0.05)/step)+1;
	
	cout <<"minE="<<minE<<" maxE="<<maxE<<" bins="<<bins<<endl;

	TH1F *hqa = new TH1F("hqa","Phsp QA for M"+mname, bins, minE-step/2., maxE+step/2.);
	hqa->SetMarkerStyle(8);
	hqa->SetMarkerSize(1.5);
	hqa->SetMarkerColor(2);
	hqa->SetStats(0);
	hqa->SetYTitle("QA [AU]");
	hqa->SetXTitle("E_{cm} [GeV]");
	hqa->GetYaxis()->SetTitleOffset(1.2);
	
	int cnt=1;
	
	c2->cd();
	TH1F *htsten = new TH1F("htsten","tst", bins, minE-step/2., maxE+step/2.);
	n->Draw("beamm>>htsten",cut,"");

	int Npoints=0;
	for (int j=0;j<bins;++j) if (htsten->GetBinContent(j+1)>0) Npoints++;
	
	int wid = 230;
	TCanvas *c1=new TCanvas("c1","c1",10,10,wid*Npoints,wid*4);
	c1->Divide(Npoints,4);

	c2->cd();
	
	for (int j=0;j<bins;++j)
	{
		cleanup();
		if (htsten->GetBinContent(j+1)<1) continue;
		
		double ecm = hqa->GetBinCenter(j+1);
		cout <<mname<<" Ecm:"<<ecm<<endl;
		TString ecut = cut+TString::Format("&&abs(beamm-%f)<0.01",ecm);
		
		// find limits for Dalitz plot
		int nsel = n->Draw("xdal02:xdal12>>hhh",ecut,"goff");
		if (nsel>500000) nsel=500000;
		TH2F *h = (TH2F*)gDirectory->Get("hhh");
		if (h->GetEntries()==0) continue;
		
		double xmin=1000., ymin=1000., xmax=0., ymax=0.;
		double *x02=n->GetV2(), *x12 = n->GetV1();
		for (int i=0;i<nsel;++i)
		{
			if (x02[i]>xmax) xmax=x02[i];
			if (x02[i]<xmin) xmin=x02[i];
			if (x12[i]>ymax) ymax=x12[i];
			if (x12[i]<ymin) ymin=x12[i];
		}

		TH2F *h1 = new TH2F("h1",TString::Format("M%s @ %3.1f GeV",mname.Data(),ecm),20,xmin,xmax,20,ymin,ymax);
		TH2F *h2 = new TH2F("h2",TString::Format("M%s @ %3.1f GeV triggered",mname.Data(),ecm),20,xmin,xmax,20,ymin,ymax);
		TH2F *heff = new TH2F("heff",TString::Format("rel eff. M%s @ %3.1f GeV ",mname.Data(),ecm),20,xmin,xmax,20,ymin,ymax);
		TH1F *hp = new TH1F("hp",TString::Format("M%s @ %3.1f GeV pull",mname.Data(),ecm),50,-8,8);
		
		h1->Sumw2();
		h2->Sumw2();
		
		int N=n->GetEntries();
		n->Project("h1","xdal02:xdal12",ecut);
		n->Project("h2","xdal02:xdal12",ecut+"&&trig");
		h1->SetContour(100);
		h2->SetContour(100);
		
		//cout <<ecut<<"  : "<<h1->GetEntries()<<" "<<h2->GetEntries()<<endl;
		//c1->cd(1);h1->DrawCopy("colz");c1->cd(2);h2->DrawCopy("colz");c1->Update();
		//continue;
		double qa = qadalitz2(h1, h2, hp, heff);
		c1->cd(cnt); config_pad(gPad); h1->DrawCopy("colz");
		//h2->SetMaximum(h1->GetMaximum());
		c1->cd(cnt+Npoints); config_pad(gPad); h2->DrawCopy("colz");
		//h2->Divide(h1);
		//h2->SetMaximum(1.0);
		// convert in plot with sigma deviation
		//h2->SetTitle(TString::Format("rel eff. M%s @ %3.1f GeV ",mname.Data(),ecm));
		heff->SetMinimum(-8);
		heff->SetMaximum(8);
		
		c1->cd(cnt+Npoints*2); config_pad(gPad); heff->DrawCopy("colz");
		c1->cd(cnt+Npoints*3); gPad->SetTopMargin(0.08);//hp->Draw();
		hp->DrawCopy();
		
		TLatex lat;
		lat.DrawLatex(-7.5,hp->GetMaximum()*0.9,TString::Format("QA = %4.2f",qa));
		cout <<"QA = "<<qa<<endl;
		c1->Update();
		
		if (qa!=qa || qa>20) qa=20;
		
		hqa->SetBinContent(j+1,qa);
		
		c2->cd();
		gPad->SetGridx();gPad->SetGridy();
		config_pad(gPad,0.14,0.05,0.08,0.12);
		hqa->SetMaximum(21);
		hqa->Draw("P");
		c2->Update();
		cnt++;
	}
	
	c1->SaveAs("QA_plots_M"+mname+".gif");
	c2->SaveAs("QA_value_M"+mname+".gif");
  return 0;
}
