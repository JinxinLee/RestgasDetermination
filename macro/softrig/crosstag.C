#include <algorithm>
#include <map>
#include <utility>
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TEventList.h"
#include "TDirectory.h"
#include "TRegexp.h"
#include "TStyle.h"
#include "TColor.h"
#include "TPaletteAxis.h"
#include "TLegend.h"

std::map<int,int> codeidx, drawline;

Int_t tagline[60], mode, recmode, tag, tagall, tagm, codes[60], nlines;

// -----------------------------------------------------------------------

void palette2()
{  
	Double_t blu[3] = {1,1,1};
	Double_t red[3] = {1,0.5,0};
	Double_t grn[3] = {1,0.5,0};
	Double_t stp[3] = {0,0.5,1};
	
	TColor::CreateGradientColorTable(3,stp,red,grn,blu,100);
}

// -----------------------------------------------------------------------

void setStyle()
{	
	gStyle->SetPadTopMargin(0.05);
	gStyle->SetPadBottomMargin(0.1);
	gStyle->SetPadLeftMargin(0.1);
	gStyle->SetPadRightMargin(0.07);
	gStyle->SetTitleH(0.035);
	//gStyle->SetTitleX(0.1);
	//gStyle->SetTitleW(0.9);
	gStyle->SetPalette(1);
}

// -----------------------------------------------------------------------

void resizePalette(TH1* h)
{
	TPaletteAxis *palette = (TPaletteAxis*)h->GetListOfFunctions()->FindObject("palette");
	palette->SetX1NDC(0.94);
	palette->SetX2NDC(0.96);
	palette->SetY1NDC(0.1);
	palette->SetY2NDC(0.95);
}

// -----------------------------------------------------------------------

void init(TTree *t, double sqs)
{
	// prepare tree by setting branches
	t->SetBranchAddress("mode",&mode);
	t->SetBranchAddress("recmode",&recmode);
	t->SetBranchAddress("tag",&tag);
	t->SetBranchAddress("tagall",&tagall);
	t->SetBranchAddress("tagm",&tagm);
	
	t->SetBranchStatus("*",0);
	t->SetBranchStatus("tag*",1);
	t->SetBranchStatus("mode",1);
	t->SetBranchStatus("recmode",1);
	t->SetBranchStatus("tag",1);
	t->SetBranchStatus("tagall",1);
	t->SetBranchStatus("tagm",1);
	
	TObjArray* branches = t->GetListOfBranches();	
		
	nlines = 0;
	TRegexp reg("tag[0-9][0-9][0-9]");
	
	for(int i=0; i<=branches->GetLast(); ++i)
	{
		TBranch* branch = (TBranch*)branches->UncheckedAt(i);
		TString v=branch->GetName(); 
		if (v(reg)!="") 
		{
			int m = TString(v(3,3)).Atoi();
			if (sqs!=3.0 || m>=400) 
			{
				t->SetBranchAddress(v,&(tagline[nlines]));
				//t->SetBranchStatus(v,1);
				codeidx[m]=nlines;
				codes[nlines] = m;
				
				nlines++;
			}
		}
	}
	codeidx[900]=nlines;
	codes[nlines]=900;
	nlines++;
	
	drawline[110] = 1;
	drawline[120] = 1;
	drawline[130] = 1;
	drawline[140] = 1;
	drawline[200] = 1;
	drawline[400] = 1;
	drawline[500] = 1;
	drawline[600] = 1;
}

// -----------------------------------------------------------------------

void config_pad(TVirtualPad* p, double b=0.1, double r=0.07, double t=0.05,  double l=0.1)
{
	p->SetTopMargin(t);
	p->SetBottomMargin(b);
	p->SetLeftMargin(l);
	p->SetRightMargin(r);
	p->SetGridx();
	p->SetGridy();
}

// -----------------------------------------------------------------------

void config_histo(TH1* h, double labs=0.018, TString titley="", TString titlex="")
{
	h->GetXaxis()->SetLabelSize(labs);
	h->GetXaxis()->SetLabelOffset(0.01);
	h->GetXaxis()->SetTitleSize(0.03);
	h->GetXaxis()->SetTitleOffset(1.5);
	
	h->GetYaxis()->SetLabelSize(labs);
	h->GetYaxis()->SetLabelOffset(0.01);
	h->GetYaxis()->SetTitleSize(0.03);
	h->GetYaxis()->SetTitleOffset(1.5);

	h->GetZaxis()->SetLabelSize(0.018);
	h->GetZaxis()->SetLabelOffset(0.005);
	
	h->SetStats(0);
	h->SetContour(100);	
	
	if (titley!="") h->SetYTitle(titley);
	if (titlex!="") h->SetXTitle(titlex);
	
	//h->Sumw2();
	h->SetLineWidth(2);
}

// -----------------------------------------------------------------------

void config_histo2d(TH1* h, TString titley="", TString titlex="")
{
	double labs = 0.018+0.008*(57.-nlines)/43.;
	
	config_histo(h, labs, titley, titlex);

	for (int i=0;i<nlines-1;++i)
	{
		h->GetXaxis()->SetBinLabel(i+1,TString::Format("T%d",codes[i]));
		h->GetYaxis()->SetBinLabel(nlines-i,TString::Format("M%d",codes[i]));
	}
	
	h->GetYaxis()->SetBinLabel(1,"DPM");
	h->GetXaxis()->SetBinLabel(nlines,"any");
	h->GetXaxis()->LabelsOption("v");
}

// -----------------------------------------------------------------------

void config_histo1d(TH1* h, TString titley="", TString titlex="", int lincol=1, int fillcol=0, int fillstyle=0)
{
	double labs = 0.04+0.01*(57.-nlines)/43.;
	
	config_histo(h, labs, titley, titlex);
	
	h->GetYaxis()->SetTitleSize(0.05);
	h->GetYaxis()->SetTitleOffset(0.7);
	h->GetYaxis()->SetLabelSize(0.04);
	
	h->GetXaxis()->SetTitleSize(0.045);
	
	for (int i=0;i<nlines-1;++i)
		h->GetXaxis()->SetBinLabel(i+1,TString::Format("M%d",codes[i]));

	h->GetXaxis()->SetBinLabel(nlines,"DPM");
	h->GetXaxis()->LabelsOption("v");
	
	h->SetLineColor(lincol);
	h->SetFillColor(fillcol);
	h->SetFillStyle(fillstyle);
}

// -----------------------------------------------------------------------

void crosstag(TString fname, int fact=1, int saveplots=0, double sqs=-1.)
{
	setStyle();
	
	TFile *f = new TFile(fname,"READ");
	TTree *t = (TTree*)f->Get("ntpev");
	
	if (sqs<0.) sqs = TString(fname(fname.Index("/M")+2,3)).Atoi()/100.; // assume file name Mxxx_... with xxx = sqs*100
	if (sqs<0. || sqs>6.) return;

	
	// ********
	// init branches in trees, global vars etc
	// ********
	init(t, sqs);
	
	TString sqsstr = TString::Format("%03d",int(sqs*100.));
	
	TCanvas *c1=new TCanvas("c1","c1",10,10,1000,900);
	TCanvas *c3=new TCanvas("c3","c3",100,100,1000,900);
	TCanvas *c2=new TCanvas("c2","c2",400,20,1000,900);
	
	c2->Divide(1,2);
	
	TFile *ff=0;
	if (saveplots) ff=new TFile("crosstag_histos.root","UPDATE");
	
	
	// ********
	// 2D Plots
	// ********
	
	// normalization histo
	TH2F *hall   = new TH2F("hall"+sqsstr,"normalization",nlines,0,nlines,nlines,0,nlines);

	// matrix of tag line vs data mode
	TH2F *htag   = new TH2F("htag"+sqsstr,TString::Format("Cross tags @ %.1f GeV",sqs),nlines,0,nlines,nlines,0,nlines);
	
	// matrix of tag line vs data mode, containing the exclusive triggers for certain data modes
	TH2F *htagex = new TH2F("htagex"+sqsstr,TString::Format("Excl. cross tags @ %.1f GeV",sqs),nlines,0,nlines,nlines,0,nlines);
	
	config_histo2d(htag,"Data Mode","Trigger Line");
	config_histo2d(htagex,"Data Mode","Trigger Line");
	config_histo2d(hall);

	
	// ********
	// signal efficiencies
	// ********
	TH1F *hsig=new TH1F("hsig"+sqsstr,TString::Format("Signal efficiencies @ %.1f GeV",sqs),nlines,0,nlines);
	TH1F *hsigi=new TH1F("hsigi"+sqsstr,TString::Format("Signal efficiencies @ %.1f GeV",sqs),nlines,0,nlines);
	TH1F *hsign=new TH1F("hsign"+sqsstr,TString::Format("Signal efficiencies @ %.1f GeV",sqs),nlines,0,nlines);
	
	config_histo1d(hsig, "efficiency [%]", "Data Mode",hsig->GetLineColor(),hsig->GetLineColor(),3003);
	config_histo1d(hsigi, "efficiency [%]","Data Mode", 2, 2, 3003);
	config_histo1d(hsign, "efficiency [%]","Data Mode");
	
	// ********
	// background contributions
	// ********
	TH1F *hbg=new TH1F("hbg"+sqsstr,TString::Format("Background fractions @ %.1f GeV",sqs),nlines,0,nlines);
	config_histo1d(hbg, "acc. background [%]", "Trigger Line",1,1,3003);
			
	int N = t->GetEntries();
	int Nbg = 0;
	
	c3->cd(); config_pad(gPad); gPad->SetLogz();
	c1->cd(); config_pad(gPad); gPad->SetLogz();
	
	for (int i=0;i<N;++i)
	{
		if (fact>1 && i%fact) continue;
		t->GetEvent(i);
		
		int smode = mode;
		if (smode>1e6)  smode/=1000;
		if (smode>1000) smode%=1000;
				
		if (smode==900) Nbg++;
		
		hsign->Fill(codeidx[smode]);
		
		for (int j=0;j<nlines;++j)
			hall->Fill(j,nlines-codeidx[smode]-1);
	
		if (i%100000==0) 
		{
			cout <<i<<"/"<<N<<endl;
			
			c1->cd(); htag->Draw("colz"); c1->Update();
			c3->cd(); htagex->Draw("colz"); c3->Update();
		}
	
		if (tag>0)
		{
			// simultaneous tag
			htag->Fill(nlines-1,nlines-codeidx[smode]-1);
			hsig->Fill(codeidx[smode]);
			if (smode==900) hbg->Fill(nlines-1);
			
			// individual tags
			for (int j=0;j<nlines-1;++j) 
			{
				//if (codes[j]==410) continue;
				if (tagline[j]>0)
				{
					htag->Fill(j,nlines-codeidx[smode]-1);
					if (tagline[j]==tagall) htagex->Fill(j,nlines-codeidx[smode]-1);
					if (smode==900) hbg->Fill(j);
					if (j==codeidx[smode]) hsigi->Fill(j);
				}
			}
		}
	}
	
	// ********
	// matrix of tag line vs data mode
	// ********
	c1->cd();
	htag->Divide(hall);	htag->SetMinimum(0.00001); htag->SetMaximum(1.0);
	htag->Draw("colz");

	resizePalette(htag);
	htag->Draw("colz");	
	
	c1->Update();
	
	// ********
	// matrix of tag line vs data mode, containing the exclusive triggers (= events not tagged by any other line) for certain data modes 
	// ********
	c3->cd();
	htagex->Divide(hall); htagex->SetMinimum(0.00001); htagex->SetMaximum(1.0);
	htagex->Draw("colz");

	resizePalette(htagex);
	htagex->Draw("colz");	
	
	c3->Update();
		
	gStyle->SetTitleH(0.06);
	c2->cd(1);
	config_pad(gPad,0.14,0.03,0.08,0.08);
	hsig->Divide(hsign);
	hsig->Scale(100.);
	hsig->SetMaximum(100.);

	hsigi->Divide(hsign);
	hsigi->Scale(100.);
	hsigi->SetMaximum(100.);
	
	hsig->Draw();
	hsigi->Draw("same");
	
	TLegend *leg=new TLegend(0.08,0.78,0.3,0.92);
	leg->AddEntry("hsigi"+sqsstr,"dedicated trigger","l");
	leg->AddEntry("hsig"+sqsstr,"total","l");
	leg->Draw("same");
	
	c2->cd(2);
	config_pad(gPad,0.14,0.03,0.08,0.08);
	hbg->Scale(100./Nbg);
	hbg->Draw();
	
	TLine l;
	l.SetLineColor(1);
	l.SetLineWidth(2);
	
	for (int i=1;i<nlines;++i)
	{		
		if (drawline.find(codes[i])!=drawline.end() || i==nlines-1) 
		{
			c1->cd();
			l.SetLineWidth(2);
			l.DrawLine(i,0,i,nlines);
			l.DrawLine(0,nlines-i,nlines,nlines-i);
			l.SetLineWidth(1);
			c3->cd();
			l.SetLineWidth(2);
			l.DrawLine(i,0,i,nlines);
			l.DrawLine(0,nlines-i,nlines,nlines-i);
			l.SetLineWidth(1);
			c2->cd(1);
			l.DrawLine(i,0,i,hsig->GetMaximum()*0.75);
			c2->cd(2);
			l.DrawLine(i,0,i,hbg->GetMaximum()*0.75);
		}
	}
	c1->Update();
	c2->Update();
	c3->Update();
	
	if (saveplots>0)
	{
		c1->SaveAs(TString::Format("cross_tag_%d.pdf",int(sqs*100)));
		c1->SaveAs(TString::Format("cross_tag_%d.gif",int(sqs*100)));
		c2->SaveAs(TString::Format("cross_tag_sigbg_%d.pdf",int(sqs*100)));
		c2->SaveAs(TString::Format("cross_tag_sigbg_%d.gif",int(sqs*100)));
		
		htag->Write();
		hall->Write();
		hsig->Write();
		hsign->Write();
		hsigi->Write();
		hbg->Write();
		ff->Close();
	}
}

