#include "GausFit.h"
#include "PDGMap.h"

#include "TH1D.h"
#include "TH1F.h"
#include "TF1.h"
#include <sstream>
#include <iostream>

GausFit::GausFit(double P,const std::string &ParticleName, TH1F* projection)
{
	fP=P;
	fParticleName=ParticleName;
	fProjection=projection;
	fSumMax=projection->GetBinLowEdge(projection->GetNbinsX()) + projection->GetXaxis()->GetBinWidth(projection->GetNbinsX());
	
	fGausFit=NULL;
	fSigma=0.;
	fMean=0.;
	fN=1;
	
	fMinEntries=10;
	
}

GausFit::~GausFit()
{
	if(fGausFit)	{
		delete fGausFit;
	}
	
	delete fProjection;	//to be or not to be
}

void GausFit::Fit(PDGMap *p_PDGMap)
{
	TH1F *pCurHisto=fProjection;
	if(!pCurHisto)	{
		cout << "GausFit - Skipping: " << fParticleName << "_" << fP << endl;
		return;
	}
	if( pCurHisto->GetEntries() >= fMinEntries )	{
		//cout << "GausFit - Creating Gaus Fits for " << fParticleName << "_" << fP << endl;
		//cout << "GausFit - Using upper fit limit: " << fSumMax << endl;
		TF1 *fgaus = new TF1("fgaus","gaus",0 , fSumMax+100);	
		assert(fgaus);
		PrepareGausFit(fgaus);
		//pCurHisto->Fit("fgaus","RNOLL");
		pCurHisto->Fit(fgaus, "RNOLL");
		fN=static_cast<unsigned int>(pCurHisto->GetEntries());
		if(p_PDGMap)	{
			fgaus->SetLineColor(p_PDGMap->GetParticleColor(fParticleName));
		}
		fGausFit=fgaus;
		fSigma=fGausFit->GetParameter("Sigma");
		fMean=fGausFit->GetParameter("Mean");
	}
	else	{
		cout << "GausFit - Skipping: " << fParticleName << "_" << fP << endl;
		cout << "Reason: Less than " << fMinEntries << " Entries" << endl;
	}
}

void GausFit::PrepareGausFit(TF1 *FitFcn) const
{
	ostringstream FitFcnName;			
	FitFcnName << fParticleName.c_str() << "_gausFit_" << fP;
	FitFcn->SetName(FitFcnName.str().c_str());
}

TH1F *GausFit::GetProjection() const
{
	return fProjection;
}

TF1 *GausFit::GetGausFit() const
{
	return fGausFit;
}

double GausFit::GetResolution() const
{
	if(fMean)	{
		return fSigma/fMean;
	}
	else	{
		cout << "GausFit: Mean value was zero" << endl;
		return 0.;
	}
}

double GausFit::GetSeparationPower(const GausFit &gausFit) const
{
	double otherMean=gausFit.GetMean();
	double otherSigma=gausFit.GetSigma();
	
	double seppo=abs(fMean-otherMean)/(fSigma+otherSigma);
	return seppo;
}
