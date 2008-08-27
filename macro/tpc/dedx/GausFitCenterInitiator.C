#include "GausFitCenterInitiator.h"
#include "GausFitCenter.h"
#include "PDGMap.h"

#include <string>
#include <sstream>
#include <iostream>
#include "TH1F.h"
#include "TH2F.h"

using namespace std;


GausFitCenterInitiator::GausFitCenterInitiator()
{
	ParticleFitCreator=new GausFitCenter;
	SetMomenta();
}

GausFitCenterInitiator::~GausFitCenterInitiator()
{
	delete ParticleFitCreator;
}

void GausFitCenterInitiator::SetPDGMap(PDGMap *pPDGMap)
{ 
	ParticleFitCreator->SetPDGMap(pPDGMap);
}

void GausFitCenterInitiator::InitGausFitCenter()
{
	int binTolerance=2;
	
	std::vector<TH2F *>::const_iterator cit;
	for(cit=fParticleHistos.begin();cit!=fParticleHistos.end();++cit)
	{
		TH2F *h = (*cit);
		if(h)	{
			string ParticleName=h->GetName();
			Int_t nbin = h->GetNbinsX();	//number of bins
			//cout << "nbin: " << nbin << endl;
			
			for(Int_t i=0; i<=nbin;i++)	
			{	
				//das Histogram zu einem Teilchen wird für viele Impulswerte projeziert
				Int_t first_bin=i-binTolerance;
				Int_t last_bin=i+binTolerance;
				
				Double_t P=BinToMomentum(h,i);
				//cout << "Checking Momentum: " << P << endl;
				if(!IsInVector(P,vP_set))	{
					//cout << "Invalid Momentum!" << endl;
					continue;
				}
				
				//cout << "ProjectionY from bin number " << first_bin << " to " << last_bin << endl; 
				TH1F * pHisto=(TH1F*)h->ProjectionY("temp", first_bin,last_bin );  //z.b. bin 1 , 2, 3        //pHisto will get another name in DeDxFromExistingHistogram
	
				PrepareGausFitHistogram(P, ParticleName, pHisto);			
				ParticleFitCreator->AddGausHistogram(P, ParticleName, pHisto);
			}
		}
	}
	ParticleFitCreator->PrintGausFitMap();
}

void GausFitCenterInitiator::PrepareGausFitHistogram(double P_set, const string &strName, TH1F *pHisto) const
{	//it is important that every histogram gets its own name
	ostringstream HistoName;			
	HistoName << strName.c_str() << "_de_dx_" << P_set;
	pHisto->SetNameTitle(HistoName.str().c_str(), HistoName.str().c_str());
	pHisto->GetXaxis()->SetTitle("dE/dx");
	//cout << "PrepareHistogram " << HistoName.str().c_str() << endl; 
}

double GausFitCenterInitiator::BinToMomentum(TH2F *h, int bin) const
{
	Double_t momentum = h->GetBinLowEdge(bin);	
	return momentum;
}

bool GausFitCenterInitiator::IsInVector(Double_t momentum, const vector<Double_t> &vP_set) const
{
	vector<Double_t>::const_iterator cit;
	for(cit=vP_set.begin();cit!=vP_set.end(); cit++)
	{
		if(fabs((*cit)-momentum)<0.0001)	{
			return true;
		}
	}
	return false;
} 


void GausFitCenterInitiator::ClearMomenta()
{
	vP_set.clear();
}

void GausFitCenterInitiator::AddMomentum(double P)
{
	vP_set.push_back(P);
}

void GausFitCenterInitiator::SetMomenta()
{
	vP_set.clear();
	vP_set.push_back(0.2);
	vP_set.push_back(0.25);	
	vP_set.push_back(0.3);
	vP_set.push_back(0.35);
	vP_set.push_back(0.4);
	vP_set.push_back(0.5);
	vP_set.push_back(0.6);
	vP_set.push_back(0.7);	
	vP_set.push_back(0.8);
	vP_set.push_back(0.9);
}

void GausFitCenterInitiator::SetMomentaV2()
{
	vP_set.clear();
	vP_set.push_back(0.2);
	vP_set.push_back(0.25);	
	vP_set.push_back(0.3);
	vP_set.push_back(0.35);
	vP_set.push_back(0.4);
	vP_set.push_back(0.5);
	vP_set.push_back(0.6);
	vP_set.push_back(0.7);	
	vP_set.push_back(0.8);
	vP_set.push_back(0.9);
	vP_set.push_back(1.0);
	vP_set.push_back(1.1);	
	vP_set.push_back(1.2);
	vP_set.push_back(1.4);
	vP_set.push_back(1.8);
	vP_set.push_back(2.0);
	vP_set.push_back(2.2);
	vP_set.push_back(2.4);	
	vP_set.push_back(2.6);
	vP_set.push_back(2.8);
	vP_set.push_back(3.0);		
}
