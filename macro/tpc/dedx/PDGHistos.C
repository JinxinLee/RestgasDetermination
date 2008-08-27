#include <map>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "TFile.h"
#include "TH2F.h"
#include "PDGHistos.h"
#include "TObjArray.h"
#include "TCanvas.h"

#include "PDGMap.h"
#include "FitServices.h"

using namespace std;


PDGHistos::PDGHistos():p_PDGMap(new PDGMap) 
{
	HistogramSet* standardSet=new HistogramSet;
	fHistoSets.push_back(standardSet);
	CurrentHistograms=standardSet;
}

PDGHistos::~PDGHistos()
{
	vector<HistogramSet*>::iterator it;
	for(it=fHistoSets.begin(); it!=fHistoSets.end(); it++)
	{
		assert(*it);
		delete (*it);
		(*it)=NULL;	
	}
	delete p_PDGMap; 
}

bool PDGHistos::Add(TH2F *pHisto,const string &HistogramName)
{
	if(!pHisto)	{
		cout << "PDGHistos::Add - Error: Histogram Pointer is NULL Pointer" << endl;
		return false;
	}

	if(!CurrentHistograms->Add( pHisto, HistogramName ) )	{
		cout << "PDGHistos::Add - Error: Can' t add a Histogram that is already managed!" << endl;
		return false;	
	}
	//cout << "Setting Name of " << pHisto->GetName() << " to " << HistogramName << endl;
	pHisto->SetNameTitle(HistogramName.c_str(), HistogramName.c_str());
	return true;
}

std::string PDGHistos::GetParticleName(Int_t PDGCode)
{
	return p_PDGMap->GetParticleName(PDGCode);
}

void PDGHistos::Save(const string &strPathFilename)
{
	TObjArray Histos(0);
	std::vector<HistogramSet*>::const_iterator cit;
	for(cit=fHistoSets.begin(); cit!=fHistoSets.end(); ++cit)
	{
		cout << "Try to save histogram!" << endl;
		TH2F *pCurHisto=NULL;
		while((*cit)->GetNextParticleHistogram(pCurHisto))	{
			cout << "Got next Particle Histo" << endl;
			if(pCurHisto)	{
				Histos.Add(pCurHisto);
				printf("Saving HistoName: %s\n", pCurHisto->GetName());
			}
			assert(pCurHisto);
		}
		(*cit)->CreateSummary();
		if((*cit)->GetSummary(pCurHisto))	{
			if(pCurHisto)	{
				Histos.Add(pCurHisto);
				printf("Saving HistoName: %s\n", pCurHisto->GetName());
			}
		}
	}

	TFile f(strPathFilename.c_str(),"recreate");
	Histos.Write();
	f.Close();
}

//i don' t know, but Roots TH2::Scale function does just nothing
//so i have to do it myself :-(
void PDGHistos::Scale()
{
	double ScalingFactor=1.;
	if( CurrentHistograms->Has(GetParticleName(211)) )	{
		FitServices *PionFit = new FitServices;	
		PionFit->ClearMomenta();
		PionFit->AddMomentum(0.3);
		PionFit->AddMomentum(0.35);
		PionFit->AddMomentum(0.4);
		PionFit->AddMomentum(0.45);
		PionFit->AddMomentum(0.5);
		PionFit->AddMomentum(0.6);
		PionFit->AddMomentum(0.7);
		PionFit->AddMomentum(0.8);
		PionFit->AddMomentum(0.9);
		PionFit->AddMomentum(1.);
		PionFit->AddMomentum(1.0);
		PionFit->AddMomentum(1.2);
		PionFit->AddMomentum(1.4);
		PionFit->AddMomentum(1.6);
		PionFit->AddMomentum(1.8);
		PionFit->AddMomentum(2.0);
		PionFit->AddMomentum(2.2);
		PionFit->AddMomentum(2.4);
		PionFit->AddMomentum(2.6);
		PionFit->AddMomentum(2.8);
		PionFit->AddMomentum(2.9);

		PionFit->SetParticleHistogram( CurrentHistograms->Get(GetParticleName(211)) );
		ScalingFactor=PionFit->GetBBMinimum();
		cout << "Got " << ScalingFactor << " from PionFit" << endl;
		PionFit->DrawBBFit();
		delete PionFit;
	}
	Scale(ScalingFactor);
}

void PDGHistos::Scale(double ScalingFactor)
{
	HistogramSet* scaledSet=CurrentHistograms->GetScaledHistogramSet(ScalingFactor);
	fHistoSets.push_back(scaledSet);
}

