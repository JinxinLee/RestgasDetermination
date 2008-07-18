#include "GausFitCenter.h"
#include "GausFit.h"
#include "PDGMap.h"
#include "PndTpcDEDXFits.h"

#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

#include <iostream>
#include <sstream>
#include <assert.h>
#include <cmath>

using namespace std;

GausFitCenter::GausFitCenter():BBFits(new PndTpcDEDXFits),fPDGMap(new PDGMap)
{

}

GausFitCenter::~GausFitCenter()
{
	map<double, map< string, GausFit*> >::iterator it1;
	for(it1=fGausFitMap.begin(); it1!=fGausFitMap.end(); ++it1)
	{
		map< string, GausFit*> ParticleMap;
		ParticleMap=it1->second;
		map< string, GausFit*>::iterator itParticleMap;
		map< string, GausFit*>::iterator endParticles=ParticleMap.end();
		for(itParticleMap=ParticleMap.begin();itParticleMap!=endParticles; ++itParticleMap)
		{
			delete itParticleMap->second;
			itParticleMap->second=NULL;
		}
	}
	map<double, TH1 *>::iterator it2;		
	for(it2=fProjectionHistogram.begin(); it2!=fProjectionHistogram.end(); ++it2)
	{
		delete it2->second;
		it2->second=NULL;
	}
	map<string,TGraphErrors *>::iterator it3;
	for(it3=fBBGraphs.begin(); it3!=fBBGraphs.end(); ++it3)
	{
		delete it3->second;
		it3->second=NULL;		
	}	
	map<string,TF1 *>::iterator it4;
	for(it4=fBBFit.begin(); it4!=fBBFit.end(); ++it4)
	{
		delete it4->second;
		it4->second=NULL;			
	}	
	delete fPDGMap;
	delete BBFits;
}

void GausFitCenter::SetPDGMap(PDGMap *pPDGMap)
{
	*fPDGMap=*pPDGMap;
	fPDGMap->Print();
}

void GausFitCenter::AddGausHistogram(double P,const std::string &ParticleName, TH1F* projection)
{	
	map< string, GausFit*> ParticleMap;
	ParticleMap=fGausFitMap[P];	//get or create the particle map for momentum p
	if(ParticleMap.find(ParticleName)!=ParticleMap.end())	{
		cout << "Can' t add gaus histogram " << P << " - " << ParticleName;
		cout << " -> i found it in the GausFitMap already!" << endl;
		return;
	}
	GausFit *newGausFit=new GausFit(P,ParticleName,projection);
	newGausFit->Fit(fPDGMap);
	ParticleMap[ParticleName]=newGausFit; //add new Particle
	fGausFitMap[P]=ParticleMap;
}

void GausFitCenter::CreateBBGraphs()
{
	if(fBBGraphs.size() != 0)	{
		return;
	}
	map<double, map<string, GausFit*> >::const_iterator itGausFitMap;
	map<double, map<string, GausFit*> >::const_iterator end=fGausFitMap.end();
	
	int iPoint=0;
	for(itGausFitMap=fGausFitMap.begin();itGausFitMap!=end;++itGausFitMap)
	{	//i assume that GausFitMap is sorted in P
		map< string, GausFit*> ParticleMap;
		ParticleMap=itGausFitMap->second;
		
		map< string, GausFit*>::const_iterator itParticleMap;
		map< string, GausFit*>::const_iterator endParticles=ParticleMap.end();
		for(itParticleMap=ParticleMap.begin();itParticleMap!=endParticles; ++itParticleMap)
		{
			GausFit* fit=itParticleMap->second;	
		     string Particle=itParticleMap->first;
			map<string, TGraphErrors*>::const_iterator pGraph=fBBGraphs.find(Particle);
			
			TGraphErrors *TheGraphToFill=NULL;			
			if(pGraph==fBBGraphs.end() )	{
				TGraphErrors *p=new TGraphErrors;	//(fGausFitMap.size());
				fBBGraphs[Particle]=p;
			}
			TheGraphToFill=fBBGraphs[Particle];
			assert(TheGraphToFill);
			TheGraphToFill->SetPoint(iPoint, fit->GetP(), fit->GetMean());
			TheGraphToFill->SetPointError(iPoint, 0.02, fit->GetSigma()/std::sqrt((float)fit->GetN()));
			/*
			cout << "GausFitCenter::CreateBBGraphs: fill Point Nr ";
			cout << iPoint << "(" << fit->GetP() << "," << fit->GetMean() << ")" 
			<< "<- " << fit->GetN() << endl;
			*/
		}
		iPoint++;
	}
}

TGraphErrors *GausFitCenter::GetBBGraph(const std::string &ParticleName) const
{
	map<string, TGraphErrors*>::const_iterator pGraph=fBBGraphs.find(ParticleName);
	if(pGraph!=fBBGraphs.end() )	{
		return pGraph->second;
	}
	return NULL;
}

void GausFitCenter::CreateBBFits()
{
	if(fBBFit.size() != 0)	{
		return;
	}
	std::map<std::string,TGraphErrors *>::const_iterator cit;
	std::map<std::string,TGraphErrors *>::const_iterator end=fBBGraphs.end();
	
	for(cit=fBBGraphs.begin();cit!=end; ++cit)
	{
		TGraphErrors *graph=cit->second;
		BBFits->CreateFitForGraph(graph, cit->first);
		TF1 *BetheBloch=BBFits->GetFunction( cit->first );
		fBBFit[cit->first]=BetheBloch;
	}
}

TF1* GausFitCenter::GetBBFit(const std::string &ParticleName) const
{
	std::map<std::string,TF1 *>::const_iterator pFit=fBBFit.find(ParticleName);
	if(pFit!=fBBFit.end() )	{
		return pFit->second;
	}
	return NULL;
}

void GausFitCenter::CreateResolutionGraphs()
{
	if(fResolutionGraphs.size() != 0)	{
		return;
	}

	map<double, map< string, GausFit*> >::const_iterator end=fGausFitMap.end();
	map<double, map< string, GausFit*> >::const_iterator cit;
	int iPoint=0;
	for(cit=fGausFitMap.begin(); cit!=end; ++cit)
	{
		//double P=cit->first;
		map< string, GausFit*> ParticleMap;
		ParticleMap=cit->second;
		
		map< string, GausFit*>::const_iterator itParticleMap;
		map< string, GausFit*>::const_iterator endParticles=ParticleMap.end();
		for(itParticleMap=ParticleMap.begin();itParticleMap!=endParticles; ++itParticleMap)
		{
			GausFit* fit=itParticleMap->second;	
			assert(fit);
		     string Particle=itParticleMap->first;
			
			map<string, TGraph*>::const_iterator pGraph=fResolutionGraphs.find(Particle);
			
			TGraph *TheGraphToFill=NULL;			
			if(pGraph==fResolutionGraphs.end() )	{
				TGraph *p=new TGraph(fGausFitMap.size());
				p->SetMarkerColor(fPDGMap->GetParticleColor(Particle));
				p->SetLineColor(fPDGMap->GetParticleColor(Particle));
				p->SetMarkerStyle(20);
				p->SetMarkerSize(0.6);
				fResolutionGraphs[Particle]=p;
			}
			TheGraphToFill=fResolutionGraphs[Particle];
			assert(TheGraphToFill);
			TheGraphToFill->SetPoint(iPoint, fit->GetP(), fit->GetResolution()); 
		}
		iPoint++;
	}
}

TGraph *GausFitCenter::GetResolutionGraph(const std::string &ParticleName) const
{
	map<string, TGraph*>::const_iterator pGraph=fResolutionGraphs.find(ParticleName);
	if(pGraph!=fResolutionGraphs.end() )	{
		return pGraph->second;
	}
	return NULL;
}

void GausFitCenter::CreateSeparationPowerGraphs()
{
	if(fSeppoGraphs.size() != 0)	{
		return;
	}
	if(fGausFitMap.size()<2)	{
		return;
	}
	map<double, map< string, GausFit*> >::const_iterator end=fGausFitMap.end();
	map<double, map< string, GausFit*> >::const_iterator cit;
	int iPoint=0;
	for(cit=fGausFitMap.begin(); cit!=end; ++cit)
	{
		//double P=cit->first;
		map< string, GausFit*> ParticleMap;
		ParticleMap=cit->second;
		
		map< string, GausFit*>::const_iterator itParticleMap;
		map< string, GausFit*>::const_iterator itParticleMap2=ParticleMap.begin();
		map< string, GausFit*>::const_iterator endParticles=ParticleMap.end();
		map< string, GausFit*>::const_iterator endParticles2=ParticleMap.end();

		--endParticles;
		for(itParticleMap=ParticleMap.begin();itParticleMap!=endParticles; ++itParticleMap)
		{
			map< string, GausFit*>::const_iterator itParticleMap2=ParticleMap.begin();
			++itParticleMap2;
			for(;itParticleMap2!=endParticles2; ++itParticleMap2)
			{
				if(itParticleMap==itParticleMap2)	{
					continue;
				}
				GausFit* fit=itParticleMap->second;
				GausFit* fit2=itParticleMap2->second;	
				assert(fit);
				assert(fit2);
				string Particle1=itParticleMap->first;
				string Particle2=itParticleMap2->first;
				
				
				map<pair<string,string> , TGraph*>::const_iterator pGraph=fSeppoGraphs.find(pair<string, string>(Particle1, Particle2));
				
				TGraph *TheGraphToFill=NULL;			
				if(pGraph==fSeppoGraphs.end() )	{
					TGraph *p=new TGraph(fGausFitMap.size());
					p->SetMarkerColor(fPDGMap->GetSeparationPowerColor(Particle1, Particle2 ));
					p->SetLineColor(fPDGMap->GetSeparationPowerColor(Particle1, Particle2));
					p->SetMarkerStyle(20);
					p->SetMarkerSize(0.6);
					fSeppoGraphs[pair<string, string>(Particle1, Particle2)]=p;
				}
				TheGraphToFill=fSeppoGraphs[pair<string, string>(Particle1, Particle2)];
				assert(TheGraphToFill);
				TheGraphToFill->SetPoint(iPoint, fit->GetP(), fit->GetSeparationPower(*fit2)); 
				
			}	
		}
		iPoint++;	//next Momentum
	}
}

TGraph *GausFitCenter::GetSeparationPowerGraph(const std::string &ParticleName1, const std::string &ParticleName2) const
{
	map< pair<string, string>, TGraph*>::const_iterator pGraph=fSeppoGraphs.find(pair<string,string>(ParticleName1, ParticleName2) );
	if(pGraph!=fSeppoGraphs.end() )	{
		return pGraph->second;
	}
	//don' t give up, yet
	pGraph=fSeppoGraphs.find(pair<string,string>(ParticleName2, ParticleName1) );
	if(pGraph!=fSeppoGraphs.end() )	{
		return pGraph->second;
	}	
	
	return NULL;	
}

void GausFitCenter::Draw() const
{
	map<double, map< string, GausFit*> >::const_iterator end=fGausFitMap.end();
	map<double, map< string, GausFit*> >::const_iterator cit;
	for(cit=fGausFitMap.begin(); cit!=end; ++cit)
	{
		map< string, GausFit*> ParticleMap;
		ParticleMap=cit->second;	
		double P_set=cit->first;
		//for every new Momentum a new Canvas
		ostringstream CanvasName;
		CanvasName << "dedxDistribution_" << P_set << "Canvas";
		ostringstream CanvasTitle;
		CanvasTitle << "dedx for set P value " << P_set << " GeV/c";	
		TCanvas *pCanvas = new TCanvas(CanvasName.str().c_str(), CanvasTitle.str().c_str() , 30,30,500,350);
		assert(pCanvas);
		
		int number_of_Canvases=ParticleMap.size() + 1; // an additional one for the summary
		if( number_of_Canvases%2 == 0  )	{	
			pCanvas->Divide(2,number_of_Canvases/2);
		}
		else	{
			pCanvas->Divide(2,(number_of_Canvases+1)/2);
		}	
		
		map< string, GausFit*>::const_iterator itParticleMap;
		map< string, GausFit*>::const_iterator endParticles=ParticleMap.end();
		
		Int_t nCanvas=1;
		bool First=true;
		for(itParticleMap=ParticleMap.begin(); itParticleMap!=endParticles; ++itParticleMap)	//draw a subpad for every particle + summary
		{
			string ParticleName=itParticleMap->first;
			GausFit *fit=itParticleMap->second;
			//cout << "Drawing GausFits for P=" << P_set << " and Particle " << ParticleName << endl;
			pCanvas->cd(nCanvas++);
			TH1F *pCurHisto=fit->GetProjection();
			assert(pCurHisto);
			pCurHisto->Draw();
			TF1 *pCurFit=fit->GetGausFit();
			assert(pCurFit);
			pCurFit->Draw("SAME");
			
			TLegend *legend=new TLegend(0.39,0.6,1.1,0.8);
			
			ostringstream legendText;
			legendText << ParticleName << " Mean: " << fit->GetMean() << ", Sigma: " << fit->GetSigma()
			<< " Resolution: " << fit->GetResolution();
			legend->AddEntry( pCurFit, legendText.str().c_str() );
			legend->Draw();
			
			//draw summary
			TLegend *SummaryLegend;
			pCanvas->cd(number_of_Canvases);
			if(First)	{
				SummaryLegend=new TLegend(0.39,0.6,1.1,0.8);
				SummaryLegend->AddEntry( pCurFit, legendText.str().c_str() );
				pCurHisto->Draw();
				pCurFit->Draw("SAME");
				First=false;
			}
			else	{
				SummaryLegend->AddEntry( pCurFit, legendText.str().c_str() );
				pCurHisto->Draw("SAME");
				pCurFit->Draw("SAME");
			}
			if(nCanvas==(number_of_Canvases-1))	{
				SummaryLegend->Draw();
			}
			
		}
	}
}

void GausFitCenter::PrintGausFitMap() const
{
	map<double,map< string, GausFit*> >::const_iterator itGausFitMap;
	map<double,map< string, GausFit*> >::const_iterator end=fGausFitMap.end();
	for(itGausFitMap=fGausFitMap.begin();itGausFitMap!=end; ++itGausFitMap)
	{
		map< string, GausFit*> ParticleMap;
		ParticleMap=itGausFitMap->second;
		double P=itGausFitMap->first;
		map< string, GausFit*>::const_iterator itParticleMap;
		map< string, GausFit*>::const_iterator endParticles=ParticleMap.end();
		for(itParticleMap=ParticleMap.begin();itParticleMap!=endParticles; ++itParticleMap)
		{
			cout << "P: " << P << " " << itParticleMap->first << endl;
		}
	}
}
