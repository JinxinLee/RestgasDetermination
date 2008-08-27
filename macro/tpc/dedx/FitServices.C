#include "FitServices.h"
#include "TH2F.h"
#include "TF1.h"
#include "GausFitCenterInitiator.h"
#include "GausFitCenter.h"
#include "TGraphErrors.h"
#include "TCanvas.h"

#include <sstream>

FitServices::FitServices()
{
	fParticleHisto=NULL;
	ParticleFitCreatorOwner=new GausFitCenterInitiator;
}

FitServices::~FitServices()
{
	delete ParticleFitCreatorOwner;
}

void FitServices::SetParticleHistogram(TH2F *ParticleHisto)
{
	fParticleHisto=ParticleHisto;
 	ParticleFitCreatorOwner->AddParticleHistogram(ParticleHisto);
	ParticleFitCreatorOwner->InitGausFitCenter();
}

double FitServices::GetBBMinimum()
{
	GausFitCenter *ParticleFitCreator=ParticleFitCreatorOwner->GetGausFitCenter();
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
	p->cd();	//remove compilation warning
	GausFitCenter *ParticleFitCreator=ParticleFitCreatorOwner->GetGausFitCenter();
	TGraphErrors *graph=ParticleFitCreator->GetBBGraph(fParticleHisto->GetName());
	graph->Draw("ALP");
	TF1 *BetheBloch=BBFits.GetFunction( fParticleHisto->GetName());
	BetheBloch->Draw("SAME");
}

void FitServices::ClearMomenta()
{
	ParticleFitCreatorOwner->ClearMomenta();
}

void FitServices::AddMomentum(double P)
{
	ParticleFitCreatorOwner->AddMomentum(P);
}
