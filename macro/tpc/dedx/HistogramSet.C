#include "HistogramSet.h"

#include "TH2F.h"

#include <iostream>

using namespace std;

HistogramSet::HistogramSet()
{
	fIterator=fHistoMap.begin();
	fSummary=NULL;
	
	fVon_x=0.;
	fBis_x=0.;
	fNbin_x=0;
	fVon_y=0.;
	fBis_y=0.;
	fNbin_y=0;
	
	ReturnSummary=false;
}

HistogramSet::~HistogramSet()
{
	for(fIterator=fHistoMap.begin(); fIterator!=fHistoMap.end(); ++fIterator)
	{	
		delete fIterator->second;
		fIterator->second = NULL;
	}
	if(fSummary)	{
		delete fSummary;
	}
}

bool HistogramSet::Add(TH2F *pHisto,const std::string &HistogramName)
{
	if(fHistoMap.find(HistogramName)!=fHistoMap.end())	{
		return false;	
	}
	if(fHistoMap.empty())	{
		cout << "HistogramSet: Adding first histogram." << endl;	
		GetHistoProperties(pHisto, fNbin_x, fVon_x, fBis_x, fNbin_y, fVon_y, fBis_y);
	}
	else	{
		if( !IsAllowed(pHisto))	{
			return false;
		}
	}
	cout << "HistogramSet: Adding histogram" << HistogramName << endl;
	fHistoMap[HistogramName]=pHisto;
	fIterator=fHistoMap.begin();
	assert(fIterator!=fHistoMap.end());
	return true;
}

bool HistogramSet::IsAllowed(TH2F *pHisto)
{
	int nbin_x=0;
	double von_x=0.;
	double bis_x=0.;
	int nbin_y=0;
	double von_y=0.;
	double bis_y=0.;
	GetHistoProperties(pHisto, nbin_x, von_x, bis_x,nbin_y, von_y, bis_y);
	if( fNbin_x == nbin_x && fVon_x == von_x && fBis_x == bis_x &&
	    fNbin_y == nbin_y && fVon_y == von_y && fBis_y == bis_y )	{
	    		return true;
	    }
	return false;
}


bool HistogramSet::Has(const std::string &HistogramName)
{
	if(fHistoMap.find(HistogramName)!=fHistoMap.end())	{
		cout << "HistogramSet::Has " << HistogramName << endl;
		return true;
	}
	cout << "HistogramSet::Has not " << HistogramName << endl;
	return false;
}

TH2F *HistogramSet::Get(const std::string &HistogramName)
{
	if(Has(HistogramName))	{
		return (fHistoMap.find(HistogramName))->second;
	}
	return NULL;
}

void HistogramSet::GetHistoProperties(TH2F *histo, 	int &nbin_x, double &von_x, double &bis_x,
										int &nbin_y, double &von_y, double &bis_y	)
{
	if(histo)	{
	von_x=histo->GetBinLowEdge(1);
	bis_x=histo->GetBinLowEdge(histo->GetNbinsX())+histo->GetXaxis()->GetBinWidth(histo->GetNbinsX());
	nbin_x=histo->GetNbinsX();
	von_y=histo->GetYaxis()->GetBinLowEdge(1);
	bis_y=histo->GetYaxis()->GetBinLowEdge(histo->GetNbinsY()) + 
		histo->GetYaxis()->GetBinWidth(histo->GetNbinsY());
	nbin_y=histo->GetNbinsY();	
	}
}

void HistogramSet::CreateSummary()
{
	if( fSummary )	{
		delete fSummary;
	}
	string SummaryName="Summary"+fNameAddOn;
	fSummary = new TH2F(SummaryName.c_str(),SummaryName.c_str(), fNbin_x , fVon_x ,fBis_x, fNbin_y, fVon_y ,fBis_y);
	assert(fSummary);
	
	std::map<std::string, TH2F*>::const_iterator cit;
	for(cit=fHistoMap.begin();cit!=fHistoMap.end();++cit)
	{
		TH2F *particleHisto=cit->second;
		if(particleHisto) {
			cout << "Adding Histogram " << particleHisto->GetName() << endl;
			fSummary->Add( particleHisto ); 			
		}
	}
}

bool HistogramSet::GetNextParticleHistogram(TH2F *&pHisto)
{
	cout << "Size of fHistoMap: " << fHistoMap.size() << endl;
	if(fIterator==fHistoMap.end())	{
		fIterator=fHistoMap.begin();
		cout << "no particles left" << endl;
		return false;
	}
	pHisto=fIterator->second;
	assert(pHisto);
	++fIterator;
	return true;
}

HistogramSet *HistogramSet::GetScaledHistogramSet(double ScalingFactor) const
{	
	HistogramSet *newSet=new HistogramSet; 
	std::map<std::string, TH2F*>::const_iterator cit;
	for(cit=fHistoMap.begin();cit!=fHistoMap.end();++cit)
	{
		TH2F *pCurHisto=cit->second;
		double von_x=fVon_x;
		double bis_x=fBis_x;
		int nbin_x=fNbin_x;
		double von_y=fVon_y;
		double bis_y=fBis_y;
		int nbin_y=fNbin_y;
		
		
		string name=pCurHisto->GetName();
		string title=pCurHisto->GetTitle();
		cout << name << ": von_x(" << von_x << ") bis_x(" << bis_x 
			<< ") von_y(" << von_y << ") bis_y(" << bis_y << ")" << endl;
		name=name+"scaled";
		title=name+"scaled";
		von_y/=ScalingFactor;
		bis_y/=ScalingFactor;
		cout << " ===> " << name << ": von_x(" << von_x << ") bis_x(" << bis_x 
			<< ") von_y(" << von_y << ") bis_y(" << bis_y << ")" << endl;	
		
		TH2F *pScaledHisto = new TH2F(name.c_str(),title.c_str(), nbin_x , von_x ,bis_x, nbin_y, von_y ,bis_y);	

		//i have to get every bin in the old histo, and get it to the new that should be all
		for(int i=0; i<nbin_x; ++i)
		{
			for(int j=0; j<nbin_y; ++j)
			{
				Double_t content=pCurHisto->GetBinContent(i,j);
				pScaledHisto->SetBinContent(i,j,content);
			}
		}
		pScaledHisto->GetYaxis()->SetRangeUser(0,22);	//hardcoded
		newSet->Add(pScaledHisto, name);
		newSet->SetNameAddOn("scaled");
		cout << endl;
	}
	return newSet;
}

/*
bool HistogramSet::GetNext(TH2F *pHisto)
{ 
	if(ReturnSummary)	{
	}
	if( !GetNextParticleHistogram(pHisto) )	{
		if(fSummary)	{
			pHisto=fSummary;
			ReturnSummary=true;
			return true;
		}
	}
	return false;
}
*/

bool HistogramSet::GetSummary(TH2F *&pHisto)
{
	if(fSummary)	{
		pHisto=fSummary;
		return true;
	}
	return false;
}
