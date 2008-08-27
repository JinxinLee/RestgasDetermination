#include "DEDXHistoPainter.h"

#include "PDGMap.h"
#include "GausFitCenterInitiator.h"
#include "GausFitCenter.h"

#include "TH2F.h"
#include "TF1.h"
#include "TROOT.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "HistogramSet.h"

#include <map>
#include <string>
#include <iostream>

using namespace std;

DEDXHistoPainter::DEDXHistoPainter():fPDGMap(NULL), fCenter(new GausFitCenterInitiator) {}

DEDXHistoPainter::~DEDXHistoPainter()	
{
	delete fCenter;
}

void DEDXHistoPainter::ClearMomenta()
{
	fCenter->ClearMomenta();
}

void DEDXHistoPainter::AddMomentum(double P)
{
	fCenter->AddMomentum(P);
}

void DEDXHistoPainter::LoadHistograms()	
{
	if(!fPDGMap)	{
		cout << "No PDGMap set!" << endl;
		return;
	}
	map<Int_t,string>::const_iterator cit;
	for(cit=fPDGMap->Particles.begin(); cit!=fPDGMap->Particles.end(); cit++)	
	{
		string ParticleName=cit->second;
		TH2F *h = (TH2F *) gROOT->FindObject((ParticleName).c_str());
		if(h)	{
			fCenter->AddParticleHistogram(h);
		}
	}
	fCenter->InitGausFitCenter();
}

void DEDXHistoPainter::SetMomentaV2()
{
	fCenter->SetMomentaV2();
}

void DEDXHistoPainter::SetPDGMap(PDGMap *pPDGMap)	
{ 
	fPDGMap=pPDGMap;	
	fCenter->SetPDGMap(fPDGMap);
}

void DEDXHistoPainter::DrawResolution()
{
	GausFitCenter *gf=fCenter->GetGausFitCenter();
	gf->CreateResolutionGraphs();
	
	TMultiGraph *mg=new TMultiGraph("Resos", "Resos");
	TLegend *legend=new TLegend(0.39,0.6,0.9,0.8);
	legend->SetTextFont(72);
	legend->SetTextSize(0.04);
	
	bool anything=false;
	map<Int_t,string>::const_iterator cit;
	for(cit=fPDGMap->Particles.begin(); cit!=fPDGMap->Particles.end(); cit++)	
	{
		string ParticleName=cit->second;
		TGraph *graph=gf->GetResolutionGraph(ParticleName);
		if(graph)	{
			mg->Add(graph);
			legend->AddEntry( graph, ParticleName.c_str());
			anything=true;
		}
	}
	if(anything)	{
		mg->Draw("APC");
		mg->GetHistogram()->GetXaxis()->SetTitle("momentum in GeV/c");
		mg->GetHistogram()->GetYaxis()->SetTitleOffset(1.2);
		mg->GetHistogram()->GetYaxis()->SetTitle("resolution");
		mg->GetHistogram()->GetYaxis()->SetRangeUser(-0.002,0.195);
		legend->Draw();
	}
}

void DEDXHistoPainter::DrawSeparationPower()
{
	GausFitCenter *gf=fCenter->GetGausFitCenter();
	gf->CreateSeparationPowerGraphs();
	
	TGraph *graph=gf->GetSeparationPowerGraph(fPDGMap->GetParticleName(2212), fPDGMap->GetParticleName(211));
	TGraph *graph2=gf->GetSeparationPowerGraph(fPDGMap->GetParticleName(2212), fPDGMap->GetParticleName(321));	
	TGraph *graph3=gf->GetSeparationPowerGraph(fPDGMap->GetParticleName(2212), fPDGMap->GetParticleName(11));	
	TGraph *graph4=gf->GetSeparationPowerGraph(fPDGMap->GetParticleName(321), fPDGMap->GetParticleName(211));	
	TGraph *graph5=gf->GetSeparationPowerGraph(fPDGMap->GetParticleName(321), fPDGMap->GetParticleName(11));
	TGraph *graph6=gf->GetSeparationPowerGraph(fPDGMap->GetParticleName(211), fPDGMap->GetParticleName(11));
	
	
	TMultiGraph *mg=new TMultiGraph("Seppos", "Seppos");
	TLegend *legend=new TLegend(0.39,0.6,0.9,0.8);
	legend->SetTextFont(72);
	legend->SetTextSize(0.04);
	
	bool anything=false;
	if(graph)	{
		mg->Add(graph);
		legend->AddEntry( graph, "P+ - Pi+");
		anything=true;
	}
	if(graph2)	{
		mg->Add(graph2);
		legend->AddEntry( graph2, "P+ - K+");
		anything=true;
	}
	if(graph3)	{
		mg->Add(graph3);
		legend->AddEntry( graph3, "P+ - e-");
		anything=true;
	}
	if(graph4)	{
		mg->Add(graph4);
		legend->AddEntry( graph4, "K+ - Pi+");
		anything=true;
	}
	if(graph5)	{
		mg->Add(graph5);
		legend->AddEntry( graph5, "K+ - e+");
		anything=true;	
	}
	if(graph6)	{
		mg->Add(graph6);
		legend->AddEntry( graph6, "Pi+ - e+");
		anything=true;	
	}	
		
	if(anything)	{
		mg->Draw("APC");
		mg->GetHistogram()->GetXaxis()->SetTitle("|P|");
		mg->GetHistogram()->GetYaxis()->SetRangeUser(0,20);
		legend->Draw();
	}
}


void DEDXHistoPainter::DrawBBFitting()
{
	//________________________________________
	//Create a new Canvas, there is a histogram and a profile Subpad for every Particle
	
	GausFitCenter *gf=fCenter->GetGausFitCenter();
	gf->CreateBBGraphs();
	gf->CreateBBFits();
	
	string CanvasName="bb_trace_Canvas";
	TCanvas *pCanvas=new TCanvas(CanvasName.c_str(), "dedx and fits");
	if(fPDGMap->Particles.find(0)!=fPDGMap->Particles.end() )	{ 
		pCanvas->Divide(2,fPDGMap->Particles.size()-1);
	}
	else	{
		pCanvas->Divide(2,fPDGMap->Particles.size());
	}
		
	int nSubpad_dedx=1;		//left Pads
	int nSubpad_profile=2;		//right Pads
	map<Int_t,string>::const_iterator cit;
	for(cit=fPDGMap->Particles.begin(); cit!=fPDGMap->Particles.end(); cit++)
	{
		if(cit->first)	{
			//________________________________________
			//Draw the Histogram in left pad
			pCanvas->cd(nSubpad_dedx); 
			string HistoName=cit->second;
			printf("HistoName: %s\n", HistoName.c_str());
			TH2F *pCurHisto=(TH2F*)gDirectory->Get(HistoName.c_str());
			if(!pCurHisto)	{
				continue;
			}
			if(pCurHisto->GetEntries() < 1)	{
				continue;
			}
			pCurHisto->Draw();	
			
			//________________________________________
			//Check the fit associated with the current Histogram Name
			//and draw the corresponding fit function
			
			TF1 *fit=gf->GetBBFit(HistoName);
			if(fit)	{
				fit->SetNpx(100);
				fit->SetLineWidth(1);
				fit->SetLineColor(fPDGMap->GetParticleColor(HistoName));
				fit->Draw("SAME");
			}
			else	{
				printf( "Tried to draw non-existing Fit for %s\n", (cit->second).c_str() );
				//continue;	//i want to draw the histogram even if i have no fit
			}
					
			//________________________________________
			//Draw the Graph in the right pad
			pCanvas->cd(nSubpad_profile);
			TGraphErrors *graph=gf->GetBBGraph(HistoName);
			if(graph)	{
				graph->Draw("SAMEALP");
			}
			
			//________________________________________
			// Draw the corresponding fit function again
			if(fit)	{
				fit->Draw("SAME");
			}
			
			nSubpad_dedx+=2;
			nSubpad_profile+=2;
		}
	}

}

void DEDXHistoPainter::DrawBBSummary()
{	
	GausFitCenter *gf=fCenter->GetGausFitCenter();
	gf->CreateBBGraphs();
	gf->CreateBBFits();
	map<Int_t,string>::const_iterator cit;
	
	HistogramSet *BBHistos=new HistogramSet;
	for(cit=fPDGMap->Particles.begin(); cit!=fPDGMap->Particles.end(); cit++)
	{
		string HistoName=cit->second;
		printf("HistoName: %s\n", HistoName.c_str());
		TH2F *pCurHisto=(TH2F*)gDirectory->Get(HistoName.c_str());
		if(pCurHisto && pCurHisto->GetEntries()>1)	{
			BBHistos->Add(pCurHisto, HistoName);
		}
	}	
	BBHistos->CreateSummary();
	TH2F *Summary=(TH2F*)gDirectory->Get("Summary");
	assert(Summary);
	Summary->Draw("COLZ");
	for(cit=fPDGMap->Particles.begin(); cit!=fPDGMap->Particles.end(); cit++)
	{
			string HistoName=cit->second;
			TF1 *fit=gf->GetBBFit(HistoName);
			if(fit)	{
				fit->SetNpx(100);
				fit->SetLineWidth(1);
				fit->SetLineColor(fPDGMap->GetParticleColor(HistoName));
				fit->Draw("SAME");
			}
			else	{
				printf( "Tried to draw non-existing Fit for %s\n", (cit->second).c_str() );
				//continue;	//i want to draw the histogram even if i have no fit
			}
	}
}

void DEDXHistoPainter::DrawGausFitting()
{
	GausFitCenter *gf=fCenter->GetGausFitCenter();
	gf->Draw();
}
