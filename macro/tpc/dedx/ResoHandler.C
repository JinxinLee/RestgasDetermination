#include "ResoHandler.h"
#include "Resolution.h"
#include "PDGMap.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH2F.h"
#include "DEDXDistribana.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>

using namespace std;

ResoHandler::ResoHandler():AddCoord(0.),Spec('N')
{
	p_PDGMap = new PDGMap;
	map<Int_t, string>::const_iterator cit, cit2;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		Resolution *pReso=new Resolution(cit->second, p_PDGMap->GetParticleColor(cit->first));
		vResos.push_back(pReso);
		cout << "Created Resolution for: " << cit->second << endl;
	}	
}

ResoHandler::~ResoHandler()
{
	vector<Resolution*>::iterator end=vResos.end();
	for(vector<Resolution*>::iterator it=vResos.begin(); it!=end; ++it)
	{
		delete (*it);
		*it=NULL;
	} 
	delete p_PDGMap;
}

void ResoHandler::SetAdditionalCoordinate(char Specifier, double Coordinate)
{
	AddCoord=Coordinate;
	Spec=Specifier;
}

void ResoHandler::Fill(DEDXDistribana *distribution)
{
	map<Double_t, map<string, TF1*> > FitMap;
	distribution->GetFitMap(FitMap);
	Fill(FitMap);
}

Resolution *ResoHandler::GetResolution(const std::string &ParticleName) const
{
	std::vector<Resolution*>::const_iterator citResolution;
	for(citResolution=vResos.begin();citResolution!=vResos.end();citResolution++)
	{
		if((*citResolution)->IsFor(ParticleName))	{
			return (*citResolution);
		}
		else	{
			cout << "Not Particle " << (*citResolution)->GetParticleName() << endl;
		}
	}
	
	return NULL;
}

void ResoHandler::Fill(map<Double_t, map<string, TF1*> > &FitMap)
{
	std::vector<Resolution*>::iterator itResolution;
	for(itResolution=vResos.begin();itResolution!=vResos.end();itResolution++)
	{
		map<Double_t, map <string, TF1*> >::const_iterator cit;
		for(cit=FitMap.begin(); cit!=FitMap.end(); cit++)
		{
			map<string,TF1*> Momentum_Fits=cit->second;
			Double_t Momentum=cit->first;
			if(!Momentum_Fits.empty())	{ //there are no fits for this Momentum P
				ostringstream P;
				P << (*itResolution)->GetParticleName().c_str() << "_" << Momentum;
				cout << "Searching for "<<P.str()<<endl;
				map<string, TF1*>::iterator ParticleFit=Momentum_Fits.find(P.str());
				if( ParticleFit!=Momentum_Fits.end() )	{
					cout << ParticleFit->first << "found!" << endl;
					//find the fit functions for our Particle, (find gives back a iterator, so we need second)
					(*itResolution)->Fill( Momentum, ParticleFit->second);
				}
			}	
		}
	}	
}

void ResoHandler::Fill(map<Double_t, map<string, TF1*> > &FitMap, double Theta)
{
	std::vector<Resolution*>::iterator itResolution;
	for(itResolution=vResos.begin();itResolution!=vResos.end();itResolution++)
	{
		map<Double_t, map <string, TF1*> >::const_iterator cit;
		for(cit=FitMap.begin(); cit!=FitMap.end(); cit++)
		{
			map<string,TF1*> Momentum_Fits=cit->second;
			Double_t Momentum=cit->first;
			if(!Momentum_Fits.empty())	{ //there are no fits for this Momentum P
				ostringstream P;
				P << (*itResolution)->GetParticleName().c_str() << "_" << Momentum;	
				cout << "Searching for "<<P.str()<<endl;
				map<string, TF1*>::iterator ParticleFit=Momentum_Fits.find(P.str());
				if( ParticleFit!=Momentum_Fits.end() )	{
					cout << ParticleFit->first << "found!" << endl;
					//find the fit functions for our Particle, (find gives back a iterator, so we need second)
					(*itResolution)->Fill(Momentum, Theta, ParticleFit->second);
				}
			}	
		}
	}	
}

void ResoHandler::Draw() const
{
	ostringstream CanvasName;
	CanvasName << "Reso_Canvas";
	ostringstream CanvasTitle;
	
	TCanvas *pCanvas = new TCanvas(CanvasName.str().c_str(), CanvasName.str().c_str() , 30,30,500,350);
	assert(pCanvas);
	
	vector<Resolution*>::const_iterator cit;
	ostringstream StackTitle;
	StackTitle << "RL";
	
	TMultiGraph *mg=new TMultiGraph(StackTitle.str().c_str(), StackTitle.str().c_str());
	assert(mg);
	TLegend *legend=new TLegend(0.39,0.6,0.9,0.8);
	assert(legend);
	legend->SetTextFont(72);
	legend->SetTextSize(0.04);
	cout << "Resolution::Draw - Start" << endl;
	Int_t nGraph=0;
	for(cit=vResos.begin(); cit!=vResos.end(); cit++)
	{
		TGraph *pG=(*cit)->CreateGraph();
		if(!pG)	{
			cout << "No graph exists for " << (*cit)->GetParticleName() << endl;
			continue;
		}
		nGraph++;
		cout << "Found Graph for " << (*cit)->GetParticleName() << endl;
		pG->SetMarkerStyle(20);
		pG->SetMarkerSize(0.6);
		//pG->SetMarkerColor(Colors[nColor++]);
		//pG->SetLineColor(Colors[nColor-1]);
		mg->Add(pG);
		ostringstream legendText;
		legendText << (*cit)->GetParticleName();
		legend->AddEntry( pG, legendText.str().c_str() );
		}

	//cout << "Multigraph size: " << mg->Sizeof() << endl;
	//if(mg->Sizeof() == 0)	{
	if(nGraph == 0)	{
		delete mg;
		mg=NULL;
		delete legend;
		legend=NULL;
		cout << "ResoHandler::Draw - Ende" << endl;
		return;
	}
	mg->Draw("APC");
	assert(mg->GetHistogram());
	assert(mg->GetHistogram()->GetXaxis());
	mg->GetHistogram()->GetXaxis()->SetTitle("|P|");
	legend->Draw();
	cout << "Resolution::Draw - Ende" << endl;

}


void ResoHandler::Save(const std::string &filename) const
{
	ofstream ofs;
	ofs.open (filename.c_str(), ios_base::app);
	if(!ofs)	{
		cout << "Error opening output file!" << endl;
		return;
	}
	std::vector<Resolution*>::const_iterator cit; 
	for(cit=vResos.begin();cit!=vResos.end();cit++)
	{
		//M -> Mean value of resolution for particle ParticleName
		ofs <<"R"<<Spec<<" "<<AddCoord<<" "<< (*cit)->GetParticleName() << " " << (*cit)->GetMean() << "\n";
	}
	ofs.close();
}


