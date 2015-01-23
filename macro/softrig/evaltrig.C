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
#include "TKey.h"
#include "TGraph.h"

const int nlines = 57;

// maps energies to signal and background efficiencies
typedef std::map<double, TH1F*> Hmap;
typedef std::map<double, TH1F*>::iterator Hmapit;
std::map<int,int> codeidx;

TGraph geff[100];
int cnt[]= {0,0,0,0,0,0,0,0};

Hmap sigmap, sigimap, bgmap;

// ---------------------------------------------------------------------
int modes[] = {  100,   101,   102,   103,   104,
                 110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
                 120,   121,   122,   123,   
                 130,   131,   132,   133,   134,   135,   136,   137,   138,
                 140,   141,   150,   151,
                 200,   201,   220,   221,   222,   223,   224,   240,   241,
                 400,   410,   420,   421,   422,
                 500,
                 600,   601,   602,   620,   621,   622,   640,   641,   642,   660};
// BR in %				
double BR[] = {  3.9,  13.9,   8.1,   2.0,   3.7,
                 2.4,   8.6,   5.0,   1.2,   2.3,   1.5,   5.3,   3.1,  0.76,   1.4,
                 9.4,   6.1,   4.8,   2.1,
                 2.6,   9.4,   5.5,   1.4,   2.5,   6.4,   4.1,   3.2,   1.4,
                 5.5,   5.6,   5.4,   5.5,
                 5.9,   5.9,   1.2,   1.7,  6e-5,   3.5,   1.9,   1.8,   1.7,
                63.9,  51.4,   5.0,   3.4,  1.7,
                48.9,
                100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.,  100.};

int panel[] = {  0,  0,  0,   0,   0,
                 1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
                 2,   2,   2,   2,   
                 3,   3,   3,   3,   3,   3,   3,   3,   3,
                 4,   4,   4,   4,
                 5,   5,   5,   5,   5,   5,   5,   5,   5,
                 6,   6,   6,   6,   6,
                 6,
                 7,   7,   7,   7,   7,   7,   7,   7,   7,   7};
				
TString datatype[] = {"D^{0}","D^{*0}","D^{#pm}","D^{*#pm}","D_{s}^{(*)#pm}","c#bar{c}","bayrons/#phi","e/#mu/#gamma"};	
double range[]     = { 2.5,     3.5,          3.0,       3.5,         3.5,              2.0,        2.0,             2.0  };
				 
int cols[10] = {1,2,4,6,kOrange+5,9,kGreen+2, kBlue+3,kCyan-2,kMagenta+2};

// ---------------------------------------------------------------------

void config_graph(TGraph *g, int m=0, TString name="")
{
	int color=m;
	if (m<10) color=cols[m]; 
	g->SetMarkerStyle(m%15+20);
	g->SetMarkerColor(1);
	g->SetMarkerSize(1.2);
	g->SetLineWidth(2);
	g->SetLineColor(color);
	g->SetMarkerColor(color);
	g->SetName(name);
}

// ---------------------------------------------------------------------

void init()
{
	for (int i=0;i<nlines;++i) 
	{	
		codeidx[modes[i]]=i;
		config_graph(&geff[i],cnt[panel[i]]++,TString::Format("M%d",modes[i]));
	}
	
	config_graph(&geff[nlines],0,"DPM");
}

// ---------------------------------------------------------------------
				
void evaltrig(TString fname="crosstag_histos.root", bool makeplots=0)
{
	init();
	
	TFile *f = new TFile(fname,"READ");
	
	TKey *key;
	TIter next(f->GetListOfKeys());
	
	TRegexp rsig("hsig[0-9][0-9][0-9]");
	TRegexp rsigi("hsigi[0-9][0-9][0-9]");
	TRegexp rbg("hbg[0-9][0-9][0-9]");
	
	while ( (key = (TKey*)next()) )
	{
		TObject *obj = key->ReadObj();
		if (!obj->InheritsFrom("TH1F")) continue;
		
		TString name = obj->GetName();
		TH1F* h  = (TH1F*) obj; 
		
		double en = TString(name(name.Length()-3,3)).Atof()/100.;
		
		if (name(rsig)!="")       sigmap[en] = h;
		else if (name(rsigi)!="") sigimap[en] = h;
		else if (name(rbg)!="")   bgmap[en] = h;
	}

	int Nen = sigmap.size();
	int i=1;
	
	TCanvas *c1 = new TCanvas("c1","histos",10,10,1800,500);
	c1->Divide(Nen,2);
	
	for (Hmapit it=sigmap.begin(); it!=sigmap.end(); ++it)
	{
		TH1F *hs  = it->second;
		TH1F *hsi = sigimap[it->first];
		TH1F *hb  = bgmap[it->first];
		
		c1->cd(i);	hs->Draw(); hsi->Draw("same");
		c1->cd(i+Nen); hb->Draw();
		i++;
		
		for (int j=1;j<hs->GetNbinsX()+1;++j)
		{
			TString modestr = hs->GetXaxis()->GetBinLabel(j);
			if (modestr=="DPM")
			{
				TGraph &g = geff[nlines];
				g.SetPoint(g.GetN(),it->first, hs->GetBinContent(j));
			}
			else
			{
				int mode = TString(modestr(1,3)).Atoi();
				double eff = hs->GetBinContent(j);
				
				TGraph &g = geff[codeidx[mode]];
				g.SetPoint(g.GetN(),it->first, eff);
			}
		}
	}
	
	TCanvas *c2=new TCanvas("c2","sig eff",100,30,1600,800);
	c2->Divide(4,2);
	
	TH1F *hdisp = new TH1F("hdisp","Signal Efficiencies",100,2.0,6.0);
	hdisp->SetMaximum(100.);
	hdisp->SetStats(0);
	hdisp->SetXTitle("E_{cm} [GeV]");
	hdisp->SetYTitle("total effciency [%]");
	//hdisp->Draw();
	
	TLegend *leg[8];
	for (i=0;i<8;++i) 
	{
		c2->cd(i+1);
		gPad->SetTopMargin(0.1);
		gPad->SetGridx();
		gPad->SetGridy();
		hdisp->SetTitle("Signal efficiency: "+datatype[i]+" modes");
		hdisp->GetXaxis()->SetRangeUser(range[i],6.0);
		hdisp->DrawCopy();
		if (i<7)
		{
			leg[i]=new TLegend(0.14,0.9-cnt[i]*0.07,0.4,0.9);
			leg[i]->SetMargin(0.4);
		}
		else
		{
			leg[i]=new TLegend(0.14,0.12,0.9,0.45);
			leg[i]->SetNColumns(3);
			leg[i]->SetMargin(0.5);
		}
	}
	
	for (i=0;i<nlines;++i) 
	{
		c2->cd(panel[i]+1);
		geff[i].Draw("PL same");
		leg[panel[i]]->AddEntry(geff[i].GetName(),geff[i].GetName(),"EPL");
	}
	for (i=0;i<8;++i) 
	{
		c2->cd(i+1);
		leg[i]->Draw("same");
	}
	c2->Update();
	
	TCanvas *c3=new TCanvas("c3","bg eff",150,40,700,500);
	c3->cd();
	gPad->SetGridx();
	gPad->SetGridy();
	hdisp->SetMaximum(0.5);
	hdisp->SetMinimum(0.0);
	hdisp->SetTitle("Background level");
	hdisp->SetYTitle("acc. background [%]");
	hdisp->DrawCopy();
	geff[nlines].Draw("PL same");
	TLine l;
	l.SetLineColor(2);l.SetLineStyle(2);l.SetLineWidth(2);
	l.DrawLine(2,0.1,6,0.1);
	
	if (makeplots)
	{
		c2->SaveAs("effsig_graphs.gif");
		c2->SaveAs("effsig_graphs.pdf");
	
		c3->SaveAs("effbg_graphs.gif");
		c3->SaveAs("effbg_graphs.pdf");
	}
}

