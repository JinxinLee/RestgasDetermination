#include <sstream>
#include <map>
#include <iostream>
#include <assert.h>
#include <fstream>

#include "TH2.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TF1.h"
#include "TLegend.h"
#include "TDirectory.h"

#include "PDGMap.h"
#include "DEDXFitCreator.h"
#include "PndTpcDEDXFits.h"

DEDXFitCreator::DEDXFitCreator():p_PDGMap(new PDGMap),p_Fits(new PndTpcDEDXFits) {}

DEDXFitCreator::~DEDXFitCreator()
{
	delete p_PDGMap;
}

void DEDXFitCreator::CreateFits()
{
/*
	short int Colors[]={kRed, kGreen, kBlue, kMagenta, kYellow, kCyan, kRed, kGreen, kBlue, kMagenta, kYellow, kCyan};
	assert( sizeof(Colors)/sizeof(short int) >= p_PDGMap->Particles.size() );
	int nColor=0;
*/	
	map<Int_t,string>::const_iterator cit;
	
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		if( cit->first )	{	// the summary histogram cannot be fitted
			ostringstream HistoName;
			HistoName << cit->second;	//the histograms are named like the Particles they show
			printf("Creating fits for HistoName: %s\n", HistoName.str().c_str());
			TH2F *pCurHisto=(TH2F*)gDirectory->Get(HistoName.str().c_str());
			if(!pCurHisto)	{
				cout << "CreateFits: Skipping " << HistoName.str().c_str() << endl;
				continue;
			}
			cout << "CreateFits: Creatin Fits for " << HistoName.str().c_str() << endl;
			TProfile *pPro=pCurHisto->ProfileX("_pfx__", -1, -1, "b");
			assert(pPro);
			p_Fits->CreateFitForProfile(pPro, cit->second);
			cout << "HHHHHHHHHH   " << *p_Fits << endl;
			
			TF1 *fitFcn=NULL;
			string FunktionName;
			fitFcn = p_Fits->GetFunction(HistoName.str());
			if(fitFcn)	{
				fitFcn->SetNpx(100);
				fitFcn->SetLineWidth(1);
				fitFcn->SetLineColor(p_PDGMap->GetParticleColor(cit->second));
				vFits.push_back(fitFcn);
				
				ostringstream FitFcnName;
				FitFcnName << cit->second;
				FitMap[FitFcnName.str().c_str()]=fitFcn;
			}
			
		}
	}
	//test.SaveFitParameters("test.txt");
}

double DEDXFitCreator::GetMinimum() const
{
	if(FitMap.size()==0)	{
		return 1.;	//Maybe the function should be named GetScalingFactor?
	}
	std::map<std::string, TF1*>::const_iterator cit=FitMap.begin();
	std::map<std::string, TF1*>::const_iterator end=FitMap.end();
	double currentMin=(cit->second)->GetMinimum();
	double min=0.;
	double xmin=0.;
	for(cit=FitMap.begin();cit!=end;++cit)
	{
		TF1 *current=cit->second;
		string ParticleName=cit->first;
		min=current->GetMinimum();
		if( (min <= currentMin || currentMin < 0.) && min > 0. )	{
			currentMin=min;
			xmin=current->GetMinimumX();
			cout << "Found Minimum for " << ParticleName << ": (" << xmin << "," <<  min << ")" << endl;
		}
	}
	if(currentMin<=0.)	{
		cout << "Error: i am to stupid to find minimum!" << endl;
		currentMin=1.;
	}	
	return currentMin;
}

void DEDXFitCreator::DrawSummary()
{
	if(p_PDGMap->Particles.find(0)==p_PDGMap->Particles.end())	{
		return;
	}
	//________________________________________
	//Draw Summary in new Canvas
	ostringstream CanvasName;
	CanvasName << "Summary_Canvas";
	ostringstream CanvasTitle;
	CanvasTitle << "dedx";
	TCanvas *pCanvas = new TCanvas(CanvasName.str().c_str(), CanvasTitle.str().c_str() , 30,30,500,350);
	pCanvas->cd(1);
	
	//________________________________________
	//Draw the Summary Histogram and all fits stored in vFits
	ostringstream HistoName;
	HistoName << "Summary";
	TH2F *pCurHisto=(TH2F*)gDirectory->Get(HistoName.str().c_str());	//the Summary histogram is filled with all Particles
	if(pCurHisto) {
		pCurHisto->Draw("COLZ");
		vector<TF1*>::const_iterator citFitFcn;		//vFits is filled with all fit-functions
		for(citFitFcn=vFits.begin(); citFitFcn!=vFits.end(); citFitFcn++)
		{
			(*citFitFcn)->Draw("SAME");
		}
	}
}

void DEDXFitCreator::traceDrawing()
{
	//________________________________________
	//Create a new Canvas, there is a histogram and a profile Subpad for every Particle
	ostringstream CanvasName;
	CanvasName << "trace_Canvas_fc";
	TCanvas *pCanvas=new TCanvas(CanvasName.str().c_str(), "dedx and profiles");
	if(p_PDGMap->Particles.find(0)!=p_PDGMap->Particles.end() )	{ 
		pCanvas->Divide(2,p_PDGMap->Particles.size()-1);
	}
	else	{
		pCanvas->Divide(2,p_PDGMap->Particles.size());
	}
		
	int nSubpad_dedx=1;		//left Pads
	int nSubpad_profile=2;		//right Pads
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		if(cit->first)	{
			//________________________________________
			//Draw the Histogram in left pad
			pCanvas->cd(nSubpad_dedx); 
			ostringstream HistoName;
			HistoName << cit->second;
			printf("HistoName: %s\n", HistoName.str().c_str());
			TH2F *pCurHisto=(TH2F*)gDirectory->Get(HistoName.str().c_str());
			if(!pCurHisto)	{
				continue;
			}
			pCurHisto->Draw();
			
			//________________________________________
			//Check the fit associated with the current Histogram Name
			//and draw the corresponding fit function
			ostringstream FitFcnName;
			FitFcnName << cit->second;
			if( FitMap.find(FitFcnName.str().c_str() )!=FitMap.end() )	{
				assert(FitMap[FitFcnName.str().c_str()]);
				(FitMap[FitFcnName.str().c_str()])->Draw("SAME");
				
			}
			else	{
				printf( "Tried to draw non-existing Fit for %s\n", (cit->second).c_str() );
				//continue;	//i want to draw the histogram even if i have no fit
			}
			
			
			//________________________________________
			//Draw the profile in the right pad
			pCanvas->cd(nSubpad_profile);
			TProfile *pPro=pCurHisto->ProfileX("_pfx", -1, -1, "b");
			assert(pPro);
			pPro->Draw();
			
			//________________________________________
			// Draw the corresponding fit function again
			if( FitMap.find(FitFcnName.str().c_str() )!=FitMap.end() )	{
				assert(FitMap[FitFcnName.str().c_str()]);
				(FitMap[FitFcnName.str().c_str()])->Draw("SAME");
			}
			
			nSubpad_dedx+=2;
			nSubpad_profile+=2;
		}
	}
}

void DEDXFitCreator::PrintChi2()
{
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		if( FitMap.find(cit->second)!=FitMap.end() )	{
				assert(FitMap[cit->second]);
				Double_t chi2=(FitMap[cit->second])->GetChisquare();	
				cout << cit->second << "-> BB-Fit chi2: " << chi2 << endl;
		}
	}
}
