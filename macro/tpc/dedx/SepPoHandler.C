#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <map>
#include <iostream>
#include <assert.h>

#include "TH2F.h"
#include "TFile.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TF1.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMultiGraph.h" 
#include "TROOT.h"
#include "SepPoHandler.h"
#include "PDGMap.h"
#include "SeparationPower.h"

SepPoHandler::SepPoHandler()
{
	nHistograms=0;
	short int Colors[]={kRed, kGreen, kBlue, kMagenta, kYellow, kCyan, kRed, kGreen, kBlue, kMagenta, kYellow, kCyan, kRed, kGreen, kBlue, kMagenta, kYellow, kCyan, kRed, kGreen, kBlue, kMagenta, kYellow, kCyan, kRed, kGreen, kBlue, kMagenta, kYellow, kCyan, kRed, kGreen, kBlue, kMagenta, kYellow, kCyan, kRed, kGreen, kBlue, kMagenta, kYellow, kCyan};
	p_PDGMap=new PDGMap;
	assert( sizeof(Colors)/sizeof(short int) >= p_PDGMap->Particles.size() );
	Int_t nColor=0;
	map<Int_t, string>::const_iterator cit, cit2;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		for(cit2=cit; cit2!=p_PDGMap->Particles.end(); cit2++)
		{
			if(cit->first && cit2->first && cit->second!=cit2->second )	{	//no summary and not the same Particle Name
				short int color=p_PDGMap->GetSeparationPowerColor(cit->first, cit2->first);
				SeparationPower *pSepPo=new SeparationPower(cit->second, cit2->second, color);	
				vSepPos.push_back(pSepPo);
				cout << "Created SeparationPower for: " << cit->second << "-" << cit2->second << endl; 
			}
		}
	}	
}

SepPoHandler::~SepPoHandler()
{
	vector<SeparationPower*>::iterator it;
	for(it=vSepPos.begin(); it!=vSepPos.end(); it++)
	{
		delete(*it);
	}
}

void SepPoHandler::FillSeparationPower(map<Double_t, map<string, TF1*> > &FitMap)
{
	vector<SeparationPower*>::iterator it;
	for(it=vSepPos.begin(); it!=vSepPos.end(); it++)
	{
		(*it)->FillSeparationPower(FitMap);
	}
}

//Draw the SeparationPower for one Particlee with all the other Particles
void SepPoHandler::Draw(const string &strName) const
{	
	cout << "SepPoHandler::Draw - Start" << endl;

	vector<SeparationPower*>::const_iterator cit;
	ostringstream StackTitle;
	StackTitle << "SP_" << strName;
	
	TMultiGraph *mg=new TMultiGraph(StackTitle.str().c_str(), StackTitle.str().c_str());
	assert(mg);
	TLegend *legend=new TLegend(0.39,0.6,0.9,0.8);
	assert(legend);
	legend->SetTextFont(72);
	legend->SetTextSize(0.04);
	
	Int_t nGraph=0;
	for(cit=vSepPos.begin(); cit!=vSepPos.end(); cit++)
	{
		if ((*cit)->IsFor(strName) )	{
			TGraph *pG=(*cit)->CreateGraph();
			if(!pG)	{
				cout << "No graph exists for " << strName.c_str() << endl;
				continue;
			}
			nGraph++;
			cout << "Found Graph for " << strName.c_str() << endl;
			pG->SetMarkerStyle(20);
			pG->SetMarkerSize(0.6);
			//pG->SetMarkerColor(Colors[nColor++]);
			//pG->SetLineColor(Colors[nColor-1]);
			mg->Add(pG);
			ostringstream legendText;
			legendText <<  strName << " - " << (*cit)->GetTheOtherParticle(strName);
			legend->AddEntry( pG, legendText.str().c_str() );
		}
	}
	//cout << "Multigraph size: " << mg->Sizeof() << endl;
	//if(mg->Sizeof() == 0)	{
	if(nGraph == 0)	{
	/*
		delete mg;
		mg=NULL;
		delete legend;
		legend=NULL;
	*/
		cout << "SepPoHandler::Draw - Ende" << endl;
		return;
	}
	mg->Draw("APC");
	assert(mg->GetHistogram());
	assert(mg->GetHistogram()->GetXaxis());
	mg->GetHistogram()->GetXaxis()->SetTitle("|P|");
	legend->Draw();
	cout << "SepPoHandler::Draw - Ende" << endl;
}

TH2F *SepPoHandler::CreateHistogram()
{
	ostringstream HistogramName;
	HistogramName << "SP_" << nHistograms;
	nHistograms++;
	ostringstream HistoTitle;
	//the histograms are accesible over their names, i hope
	TH2F *pHisto = new TH2F(HistogramName.str().c_str(), HistogramName.str().c_str() , 300, 0, 3, 1400, 0, 14);
	assert(pHisto);
	pHisto->GetXaxis()->SetTitle("|P|");
	pHisto->GetYaxis()->SetTitle("SeparationPower");
	return(pHisto);
}

void SepPoHandler::Draw() const
{
	ostringstream CanvasName;
	CanvasName << "SepPo_Canvas_";
	ostringstream CanvasTitle;
	
	TCanvas *pCanvas = new TCanvas(CanvasName.str().c_str(), CanvasName.str().c_str() , 30,30,500,350);
	assert(pCanvas);
	
	if( (p_PDGMap->Particles.size())%2 == 0  )	{	
		pCanvas->Divide(2,(p_PDGMap->Particles.size())/2);
	}
	else	{
		pCanvas->Divide(2,(p_PDGMap->Particles.size()+1)/2);
	}	
	Int_t nCanvas=1;
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		if(cit->first)	{
			pCanvas->cd(nCanvas++);
			Draw(cit->second);
		}
	}
}

void SepPoHandler::GetSummary(string &summary) const	///TODO: Was soll das? 
{
	vector<SeparationPower*>::const_iterator cit;
	ostringstream Output;
	for(cit=vSepPos.begin(); cit!=vSepPos.end(); cit++)
	{
		string Name1, Name2;
		(*cit)->GetParticleNames(Name1, Name2);
		Output << "Seppo: " << Name1 << " - " << Name2 << ": " << (*cit)->GetMean() << ", " << (*cit)->GetMax() << endl;
		summary = Output.str();
	}
}

void SepPoHandler::GetSummary(vector<string> &summary) const
{
	vector<SeparationPower*>::const_iterator cit;
	for(cit=vSepPos.begin(); cit!=vSepPos.end(); cit++)
	{
		ostringstream Output;
		string Name1, Name2;
		(*cit)->GetParticleNames(Name1, Name2);
		Output << "Seppo: " << Name1 << " - " << Name2 << ": " << (*cit)->GetMean() << ", " << (*cit)->GetMax() << endl;
		summary.push_back(Output.str());
	}
}



