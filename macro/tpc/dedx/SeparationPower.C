#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <map>
#include <iostream>

#include "TF1.h"
#include "TH2F.h"
#include "TGraph.h"

#include "PDGMap.h"
#include "SeparationPower.h"
#include "SeparationPower.h"



string SeparationPower::GetTheOtherParticle(const string &ParticleName) const
{
	string Ret;
	if( ParticleName==ParticleName1	)	{
		Ret=ParticleName2;
	}
	else if( ParticleName==ParticleName2	)	{
		Ret=ParticleName1;
	}
	return Ret;
}

void SeparationPower::FillSeparationPower(const map<Double_t, map<string, TF1*> > &FitMap)
{
	map<Double_t, map <string, TF1*> >::const_iterator cit;
//	dedxlog << "FillSeparationPower" << endl;	

	for(cit=FitMap.begin(); cit!=FitMap.end(); cit++)
	{
		map<string,TF1*> Momentum_Fits=cit->second;
		Double_t Momentum=cit->first;
		if(!Momentum_Fits.empty())	{ //there are no fits for this Momentum P
			ostringstream P1;
			ostringstream P2;
			P1 << ParticleName1.c_str() << "_" << Momentum;			//TODO
			P2 << ParticleName2.c_str() << "_" << Momentum;
			map<string, TF1*>::iterator FirstParticleFit=Momentum_Fits.find(P1.str());
			map<string, TF1*>::iterator SecondParticleFit=Momentum_Fits.find(P2.str());
			if( FirstParticleFit!=Momentum_Fits.end() && SecondParticleFit!=Momentum_Fits.end())	{
				//find the fit functions for our Particles, (find gives back a iterator, so we need second)
				FillSeparationPower( FirstParticleFit->second, SecondParticleFit->second, Momentum);
			}
		}
	}
} 

void SeparationPower::FillSeparationPower(TF1 *FitFunc1, TF1 *FitFunc2, Double_t X)
{
	if(FitFunc1 && FitFunc2)	{
		FillSeparationPower(FitFunc1->GetParameter("Mean"), FitFunc2->GetParameter("Mean"), FitFunc1->GetParameter("Sigma"),  FitFunc2->GetParameter("Sigma"), X );
	}
}

void SeparationPower::FillSeparationPower(Double_t Mean1, Double_t Mean2, Double_t Sigma1, Double_t Sigma2, Double_t X)
{
	if(Sigma1 + Sigma2)	{
		FillSeparationPower(abs(Mean1-Mean2)/(Sigma1+Sigma2), X);
//		dedxlog << "SeparationPower filled!";
	}
}


void SeparationPower::FillSeparationPower(Double_t SepPo, Double_t X)
{
	vSepPo.push_back(SepPo);
	vP.push_back(X);
}

void SeparationPower::FillHistogram(TH2F *pHisto)
{
	if(pHisto)	{
		assert(pHisto);
		assert(vSepPo.size() == vP.size());
//		dedxlog << "Attempting to fill (and draw) " << ParticleName1 << "-" << ParticleName2  << endl;
		vector<Double_t>::const_iterator citX, citY;
		citY=vSepPo.begin();
		for(citX=vP.begin(); citX!=vP.end(); citX++, citY++)
		{
			pHisto->Fill(*citX,*citY);
			//dedxlog << "Fill: x=" << *citX << "Fill: y=" << *citY << endl; 
		}
	}
}

TGraph *SeparationPower::CreateGraph() const
{
	assert(vSepPo.size() == vP.size());
	
	TGraph *pG=NULL;
	vector<Double_t>::const_iterator citX, citY;
	citY=vSepPo.begin();
	if(vP.size() != 0)	{	
		pG=new TGraph(vP.size());
		assert(pG);
		pG->SetMarkerColor(fColor);
		pG->SetLineColor(fColor);
		Int_t i=0;
		for(citX=vP.begin(); citX!=vP.end(); citX++, citY++)
		{
			cout << "Filling Sepograp: (" << *citX << "," << *citY << ")" <<  endl;
			pG->SetPoint(i++,*citX, *citY);
		}
	}
	return pG;
}

double SeparationPower::GetMean() const
{
	double mean=0.0;
	if(vSepPo.size())	{
		mean=accumulate(vSepPo.begin(), vSepPo.end(), 0.0 );
		mean=mean/vSepPo.size();
	}
	return mean;
}

double SeparationPower::GetMax() const
{
	double max=0.0;
	vector<Double_t>::const_iterator cit=max_element(vSepPo.begin(), vSepPo.end());
	if(cit!=vSepPo.end())	{
		max=*cit;
	}
	return max;
}

