#include "FitServices.h"
#include "TH2F.h"
#include "TF1.h"
#include "GausFitCenter.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

#include <sstream>

FitServices::FitServices()
{
	fParticleHisto=NULL;
	ParticleFitCreator=new GausFitCenter;
}

FitServices::~FitServices()
{
	delete ParticleFitCreator;
}

void FitServices::SetParticleHistogram(TH2F *ParticleHisto)
{
	fParticleHisto=ParticleHisto;
	InitGausFitCenter();
}

double FitServices::GetBBMinimum()
{
	ParticleFitCreator->CreateBBGraphs();
	TGraphErrors *graph=ParticleFitCreator->GetBBGraph(fParticleHisto->GetName());

	BBFits.CreateFitForGraph(graph, fParticleHisto->GetName());
	TF1 *BetheBloch=BBFits.GetFunction( fParticleHisto->GetName());
	
	double min_y=BetheBloch->GetMinimum();
	double min_x=BetheBloch->GetMinimumX();
	cout << "Found Minimum for " << fParticleHisto->GetName() << ": (" << min_x << "," <<  min_y << ")" << endl;
	return min_y;
}

void FitServices::DrawBBFit() const
{
	TCanvas *p=new TCanvas("fit","fit");
	TGraphErrors *graph=ParticleFitCreator->GetBBGraph(fParticleHisto->GetName());
	graph->Draw("ALP");
	TF1 *BetheBloch=BBFits.GetFunction( fParticleHisto->GetName());
	BetheBloch->Draw("SAME");
}

//evtl. ab hier eine Klasse GausFitCenterInitiator
void FitServices::InitGausFitCenter()
{
	SetMomenta();
	int binTolerance=2;
	string ParticleName=fParticleHisto->GetName();
	TH2F *h = fParticleHisto;
	if(h)	{
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
	ParticleFitCreator->PrintGausFitMap();
}

void FitServices::PrepareGausFitHistogram(double P_set, const string &strName, TH1F *pHisto) const
{	//it is important that every histogram gets its own name
	ostringstream HistoName;			
	HistoName << strName.c_str() << "_de_dx_" << P_set;
	pHisto->SetNameTitle(HistoName.str().c_str(), HistoName.str().c_str());
	pHisto->GetXaxis()->SetTitle("dE/dx");
	//cout << "PrepareHistogram " << HistoName.str().c_str() << endl; 
}

double FitServices::BinToMomentum(TH2F *h, Int_t bin) const
{
	Double_t momentum = h->GetBinLowEdge(bin);	
	return momentum;
}

bool FitServices::IsInVector(Double_t momentum, const vector<Double_t> &vP_set) const
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

void FitServices::SetMomenta()
{
	vP_set.push_back(0.2);
	vP_set.push_back(0.25);	
	vP_set.push_back(0.3);
	vP_set.push_back(0.35);
	//vP_set.push_back(0.37);
	vP_set.push_back(0.4);
	vP_set.push_back(0.5);
	vP_set.push_back(0.6);
	vP_set.push_back(0.7);	
}
