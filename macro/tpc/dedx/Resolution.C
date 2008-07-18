#include "Resolution.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include <string>
#include <numeric>
#include <assert.h>
using namespace std;

Resolution::~Resolution()
{

}

void Resolution::Fill(const string &ParticleName, double P, double Reso)
{
	if(ParticleName==m_ParticleName)	{
		Fill(P,Reso);
	}
}

void Resolution::Fill(double P, double Reso)
{
	vCoordinates.push_back(P);
	vReso.push_back(Reso);
}

void Resolution::Fill(double P, double Theta, double Reso)
{
	vCoordinates.push_back(Coordinates(P,Theta));
	vReso.push_back(Reso);
}

#if 0
void Resolution::Fill(Coordinates co, double Reso)
{
	vCoordinates.push_back(co);
	vReso.push_back(Reso);
}
#endif

void Resolution::Fill(double X, TF1* FitFunc )
{
	if(FitFunc && FitFunc->GetParameter("Mean"))	{
		Fill(X, FitFunc->GetParameter("Sigma")/FitFunc->GetParameter("Mean") );
		cout << "Sigma: " << FitFunc->GetParameter("Sigma") << " Mean: " << FitFunc->GetParameter("Mean") << endl;
		cout << "=> Resolution: " <<  FitFunc->GetParameter("Sigma")/FitFunc->GetParameter("Mean") << endl;
	}
}

void Resolution::Fill(double X, double Theta, TF1* FitFunc )
{
	if(FitFunc && FitFunc->GetParameter("Mean"))	{
		Fill(X, Theta, FitFunc->GetParameter("Sigma")/FitFunc->GetParameter("Mean") );
		cout << "Sigma: " << FitFunc->GetParameter("Sigma") << " Mean: " << FitFunc->GetParameter("Mean") << endl;
		cout << "=> Resolution: " <<  FitFunc->GetParameter("Sigma")/FitFunc->GetParameter("Mean") << endl;
	}
}

TGraph *Resolution::CreateGraph(int index) const
{
	assert(vReso.size() == vCoordinates.size());
	
	TGraph *pG=NULL;
	vector<Double_t>::const_iterator citY;
	vector<Coordinates>::const_iterator citX;
	citY=vReso.begin();
	if(vCoordinates.size() != 0)	{	
		pG=new TGraph(vCoordinates.size());
		assert(pG);
		pG->SetMarkerColor(fColor);
		pG->SetLineColor(fColor);
		Int_t i=0;
		for(citX=vCoordinates.begin(); citX!=vCoordinates.end(); citX++, citY++)
		{
			cout << "Creating Resograp: (" << (*citX)[index] << "," << *citY << ")" <<  endl;
			pG->SetPoint(i++,(*citX)[index], *citY);
		}
	}
	return pG;
}

TGraph2D *Resolution::CreateGraph2D(int index1, int index2) const
{
	assert(vReso.size() == vCoordinates.size());
	
	TGraph2D *pG=NULL;
	vector<Double_t>::const_iterator citY;
	vector<Coordinates>::const_iterator citX;
	citY=vReso.begin();
	if(vCoordinates.size() != 0)	{	
		pG=new TGraph2D(vCoordinates.size());
		assert(pG);
		pG->SetMarkerColor(fColor);
		pG->SetLineColor(fColor);
		Int_t i=0;
		for(citX=vCoordinates.begin(); citX!=vCoordinates.end(); citX++, citY++)
		{
			cout << "Creating Resograp2D: (" << (*citX)[index1] << "," <<(*citX)[index2] << "," << *citY << ")" <<  endl;
			pG->SetPoint(i++,(*citX)[index1],(*citX)[index2],*citY);
		}
	}
	return pG;
}

double Resolution::GetMean() const
{
	double mean=0.0;
	if(vReso.size())	{
		mean=accumulate(vReso.begin(), vReso.end(), 0.0 );
		mean=mean/vReso.size();
	}
	return mean;
}

std::ostream& operator<< (std::ostream& s, const Resolution &me)
{
	if(me.vReso.size() != me.vCoordinates.size())	{
		s << "Resolution Invalid!";
		return s;
	}
	//s << "R " << me.m_ParticleName;
	//s << " " << me.GetMean();
	
	s <<  me.m_ParticleName;
	for(unsigned int i=0; i < me.vReso.size(); i++)
	{
		Coordinates co=me.vCoordinates[i];
		s << " " << me.vCoordinates[i] << " " << me.vReso[i];
	}
	
	return s;	
}


